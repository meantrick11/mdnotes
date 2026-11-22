# Qdrant Bulk Upload：批量写入原理、性能优化与实操

> **官方文档**：[Bulk Upload Vectors to a Qdrant Collection](https://qdrant.tech/documentation/manage-data/bulk-upload/)
> 
> **前置知识**：Collection、Shard、Segment、Point、WAL、Payload Index、HNSW、Optimizer。
> 
> **学习目标**：理解向量批量导入的性能瓶颈，掌握 Batch、Parallel Upload、Sharding、Cold Storage 与读写竞争的优化方式。

---

## 1. 为什么需要 Bulk Upload？

### 问题 1：普通 Upsert 不能一次插入大量 Point 吗？

可以，但需要区分两种写入模式：

- **单条 Upsert**：每个请求插入一个 Point。
    
- **Bulk Upload**：通过批处理和并行操作，高效导入大量 Point。
    

假设一个 RAG 系统需要导入 100 万个 Chunk。

每个 Chunk 已经生成 Embedding，并转换成 Qdrant Point：

```
from qdrant_client import models

point = models.PointStruct(
    id=1,
    vector=[0.12, 0.34, 0.56],
    payload={
        "document_id": "doc_001",
        "chunk_id": 1,
    },
)
```

最简单的方式：

```
for point in points:
    client.upsert(
        collection_name="notes",
        points=[point],
    )
```

这种方式功能上没有问题，但可能存在大量不必要的开销。

### 问题 2：为什么逐条 Upsert 的效率较低？

每个请求都需要经历：

1. 客户端构建并序列化请求。
    
2. 网络发送和响应。
    
3. Qdrant 内部请求路由。
    
4. WAL（Write-Ahead Log，预写日志）处理。
    
5. 将更新应用到相关数据结构。
    

即使只有一个 Point，请求仍然具有固定开销。

因此，100 万条 Point 如果逐条写入，就需要发起 100 万次请求。

**Bulk Upload 的核心目标，是减少请求固定开销，并提高客户端和 Qdrant 服务端的资源利用率。**

官方特别指出：大规模导入的瓶颈经常出现在客户端，因此建议使用高性能客户端。官方推荐 Rust Client 作为追求极高导入吞吐量时的选择。

---

## 2. Batch Upload：为什么要分批写入？

### 问题 3：如果把多个 Point 放在一个请求中，会发生什么？

假设有 1000 个 Point。

逐条写入：

```
1000 Points
  → 1000 Requests
```

按 100 条一批：

```
1000 Points
  → 10 Batches
  → 10 Requests
```

批量写入减少了网络请求和固定处理开销。

假设导入 100 万条 Point：

|Batch Size|理论请求次数|
|---|---|
|1|1000000|
|64|15625|
|128|7813|
|256|3907|
|512|1954|

公式：

[  
N_{\text{requests}}=  
\left\lceil\frac{N_{\text{points}}}{B}\right\rceil  
]

其中：

- (N_{\text{points}})：总 Point 数量。
    
- (B)：每批包含的 Point 数量。
    

注意：请求次数减少，并不意味着实际导入时间会同比例减少。

### 问题 4：Batch Size 是否越大越好？

不是。

批量过小：

- 网络利用率不足。
    
- 请求次数较多。
    
- 固定处理开销较大。
    

批量过大：

- 单次请求体积较大。
    
- 客户端和服务端内存压力增加。
    
- 请求失败时重试成本提高。
    
- 可能增加单次操作的等待时间。
    

Qdrant 官方建议从 **64～256 个 Point / Batch** 开始测试。

最终大小取决于：

- 向量维度。
    
- Payload 大小。
    
- 网络带宽。
    
- 服务器内存。
    
- 写入负载。
    

例如，128 个 768 维 Float32 向量，仅向量主体约为：

[  
128\times768\times4=393216\ bytes  
]

即 384 KiB。

实际网络传输大小还包括 Point ID、Payload 和协议序列化开销。

### 2.1 实操：普通批量 Upsert

```
from qdrant_client import QdrantClient, models

client = QdrantClient(
    url="http://localhost:6333"
)

points = [
    models.PointStruct(
        id=i,
        vector=[0.1] * 768,
        payload={"chunk_id": i},
    )
    for i in range(128)
]

client.upsert(
    collection_name="notes",
    points=points,
    wait=True,
)
```

这里一次请求插入 128 个 Point。

`wait=True` 表示等待服务端完成该写入操作。

### 2.2 实操：使用 upload_points()

Python 客户端提供了更方便的批量上传方法：

```
client.upload_points(
    collection_name="notes",
    points=points,
    batch_size=128,
    parallel=1,
    max_retries=3,
    wait=True,
)
```

`upload_points()` 可以自动将输入数据组织成多个 Batch，不需要手动分批。

---

## 3. Parallel Upload：为什么需要并行上传？

### 问题 5：既然 Batch 已经减少请求次数，为什么还需要并行？

因为单个上传流程可能无法充分利用服务端写入能力。

串行上传：

```
Batch 1 → 等待完成
Batch 2 → 等待完成
Batch 3 → 等待完成
...
```

每次请求存在网络等待和服务端处理时间。

并行上传则可以同时发送多个 Batch：

```
Worker 1 → Batch 1、Batch 4、Batch 7
Worker 2 → Batch 2、Batch 5、Batch 8
Worker 3 → Batch 3、Batch 6、Batch 9
```

这样多个上传任务可以共同利用客户端、网络及服务端的处理能力。

### 问题 6：并行上传与异步编程有什么区别？

从概念上看：

- **Batch**：减少请求次数。
    
- **Concurrency**：让多个请求存在时间上的重叠。
    
- **Parallelism**：允许多个执行单元同时执行任务。
    

Python Qdrant Client 的 `upload_points(parallel=...)` 提供了内置的并行上传能力，通常使用多进程机制，而不需要用户自行管理上传线程。

Qdrant 官方建议首先尝试 **2～4 个并行上传 Worker**。

### 3.1 实操：并行写入

```
client.upload_points(
    collection_name="notes",
    points=points,
    batch_size=128,
    parallel=4,
    max_retries=3,
    wait=True,
)
```

参数：

|参数|作用|
|---|---|
|`batch_size=128`|每次上传最多 128 个 Point|
|`parallel=4`|使用四个并行上传执行单元|
|`max_retries=3`|失败后最多重试三次|
|`wait=True`|等待写入操作完成|

### 问题 7：并发数越高，吞吐量一定越高吗？

不是。

可能出现以下瓶颈：

- 客户端 CPU 序列化能力不足。
    
- 网络带宽达到上限。
    
- WAL 写入能力不足。
    
- 服务端磁盘 I/O 达到上限。
    
- CPU 被 HNSW 构建和 Optimizer 占用。
    
- 内存压力增加。
    

因此，提高 Parallel 不一定能够线性增加写入速度。

可以用吞吐量衡量：

[  
Throughput=  
\frac{\text{成功写入的 Point 数量}}  
{\text{总耗时}}  
]

单位通常是：

```
points / second
```

---

## 4. Sharding：为什么多个 Shard 可以提高上传吞吐量？

### 问题 8：Shard 不是为了数据分片吗？为什么能够加速写入？

因为 Qdrant 的每个 Shard 都拥有独立的 WAL 和更新 Worker。

对于一个 Collection：

```
Collection
  └── Shard 1
       ├── WAL
       ├── Update Worker
       └── Segments
```

如果只有一个 Shard，大量更新操作会集中在这个 Shard 的更新处理流程中。

多个 Shard：

```
Collection
  ├── Shard 1
  │    ├── WAL 1
  │    └── Update Worker 1
  │
  ├── Shard 2
  │    ├── WAL 2
  │    └── Update Worker 2
  │
  └── Shard 3
       ├── WAL 3
       └── Update Worker 3
```

能够利用不同分片相对独立的写入处理能力。

这是通过**数据分区提高写入并行度**的思路。

### 4.1 创建多个 Shard

```
client.create_collection(
    collection_name="notes",
    vectors_config=models.VectorParams(
        size=768,
        distance=models.Distance.COSINE,
    ),
    shard_number=2,
)
```

`shard_number=2` 表示创建两个数据分片。

官方给出的经验起点是：

**每台机器可以考虑 2～4 个 Shard。**

如果一个 Collection 有多个 Shard，可以从每个 Shard 对应一个上传 Worker 开始测试。

### 问题 9：Shard 是否越多越好？

不是。

Shard 增加也会带来：

- 更多分片级管理开销。
    
- 更多索引和存储管理成本。
    
- 查询时可能需要汇总更多分片结果。
    
- 更多内存和 CPU 消耗。
    

另外，Shard 与 Segment 依然是不同层级：

|概念|主要解决的问题|
|---|---|
|Shard|数据分布、请求路由、写入并行|
|Segment|数据存储、索引维护、后台优化|

**不要为了提高批量写入速度而无限增加 Shard。**

---

## 5. 为什么应该提前创建 Payload Index？

### 问题 10：能不能先上传数据，之后再创建索引？

可以，但对需要过滤感知 HNSW 的场景，可能增加后续重建成本。

你在 Indexing 章节中已经学过：

**Qdrant 的 Filterable HNSW 可以根据 Payload Index，为向量图增加与过滤条件相关的额外边。**

假设已经导入 100 万条 Point，并建立了 HNSW 图。

之后再创建：

```
client.create_payload_index(
    collection_name="notes",
    field_name="document_id",
    field_schema=models.PayloadSchemaType.KEYWORD,
)
```

已有的 HNSW 图并不一定拥有利用这个字段建立的额外边。

如果希望充分利用对应过滤优化，就可能需要重建 HNSW。

而重建索引可能消耗大量 CPU、内存和磁盘 I/O。

### 5.1 官方推荐顺序

```
1. Create Collection
        ↓
2. Create Payload Indexes
        ↓
3. Bulk Upload Points
        ↓
4. Build / Optimize HNSW
```

例如：

```
from qdrant_client import QdrantClient, models

client = QdrantClient(
    url="http://localhost:6333"
)

client.create_collection(
    collection_name="notes",
    vectors_config=models.VectorParams(
        size=768,
        distance=models.Distance.COSINE,
    ),
)

client.create_payload_index(
    collection_name="notes",
    field_name="document_id",
    field_schema=models.PayloadSchemaType.KEYWORD,
)

client.create_payload_index(
    collection_name="notes",
    field_name="category",
    field_schema=models.PayloadSchemaType.KEYWORD,
)

# 最后再批量上传 Point
```

这样 Qdrant 在构建 HNSW 图时，可以利用已经存在的 Payload Index。

### 问题 11：是不是所有 Payload 字段都需要提前创建索引？

不是。

只需要为后续实际过滤场景中有价值的字段建立索引。

例如：

```
payload = {
    "document_id": "doc_001",
    "category": "AI",
    "content": "Agent Context Management...",
}
```

如果：

- `document_id` 经常用于限定文档范围。
    
- `category` 经常用于分类过滤。
    
- `content` 只是返回给用户展示。
    

那么通常只需要先建立 `document_id` 和 `category` 索引。

**索引本身也会增加存储、构建和维护开销。**

---

## 6. Upload Directly to Disk：为什么要直接写磁盘？

### 问题 12：如果上传的数据量超过 RAM，会发生什么？

假设有 1000 万条 768 维 Float32 向量。

仅向量主体理论大小：

[  
10^7\times768\times4  
=30.72\ GB  
]

而服务器只有 8 GB RAM。

如果配置要求这些向量大量常驻内存，就可能产生严重的内存压力。

此时 Qdrant 官方建议考虑：

**在创建 Collection 时，将原始向量配置为 Cold Memory Tier，使其直接以磁盘存储为主。**

### 6.1 创建 Cold Vector Storage

```
client.create_collection(
    collection_name="notes_cold",
    vectors_config=models.VectorParams(
        size=768,
        distance=models.Distance.COSINE,
        memory=models.Memory.COLD,
    ),
)
```

其中：

```
memory=models.Memory.COLD
```

表示使用 Cold 存储层级。

对于明确无法全部放入内存的向量数据，这种方式可以避免部分不必要的存储迁移操作。

### 问题 13：为什么不直接使用 memmap_threshold？

`memmap_threshold` 属于 Qdrant Optimizer 的存储优化参数。

它可以根据 Segment 的数据规模触发存储形式调整。

但在大规模持续导入场景下，如果频繁达到阈值，Optimizer 可能不断进行 Segment 转换。

这些操作会消耗：

- CPU。
    
- 内存。
    
- 磁盘 I/O。
    
- 后台优化时间。
    

因此，官方指出：

**如果事先知道向量数据不能全部放入 RAM，直接配置 Cold Memory Tier，通常比依赖 Optimizer 频繁转换 Segment 更合理。**

### 6.2 Cold 的代价是什么？

Cold 并不意味着性能一定更好。

它主要优化内存占用与导入时的存储管理成本。

如果查询需要频繁读取冷数据，就可能增加磁盘 I/O 和查询延迟。

所以它与前面学过的 Quantization 可以结合：

```
Original Vectors
  → Cold Storage

Quantized Vectors
  → Pinned RAM

Search
  → 使用压缩向量
  → 必要时读取原始向量 Rescoring
```

这属于一种常见的冷热分层设计，但并非所有工作负载都适合。

---

## 7. Read-Write Contention：为什么大量写入会影响查询？

### 问题 14：Bulk Upload 和 Search 是两个不同操作，为什么会互相影响？

虽然业务操作不同，但它们共享服务器资源。

Bulk Upload 期间可能发生：

- WAL 写入。
    
- Point 数据更新。
    
- Segment 合并。
    
- HNSW 图构建。
    
- Quantization。
    
- Optimizer 后台任务。
    

而 Search 需要：

- CPU 执行相似度计算。
    
- 内存读取向量和索引。
    
- 磁盘读取 Payload 或冷数据。
    
- HNSW 图遍历。
    

这些操作会争夺同一套 CPU、内存带宽和磁盘 I/O 资源。

这就是：

**Read-Write Contention（读写资源竞争）。**

### 7.1 为什么 Optimizer 是关键？

Optimizer 在后台负责：

1. 合并 Segment。
    
2. 构建或优化索引。
    
3. 处理量化表示。
    
4. 调整存储结构。
    

当持续写入大量新 Point 时，Optimizer 可能不断产生后台任务。

如果后台优化占用了大量资源，在线查询就可能变慢。

这种问题可能表现为：

```
Bulk Upload 开始
      ↓
CPU / Disk I/O 使用率升高
      ↓
Query P95 Latency 上升
      ↓
用户搜索体验下降
```

### 问题 15：应该如何避免读写竞争？

首先区分两种业务。

**场景 A：离线初始化知识库**

没有在线用户请求。

主要目标是提高写入吞吐量，可以允许系统更多地使用资源执行导入与索引优化。

**场景 B：线上持续导入**

系统需要同时响应用户搜索。

此时除了写入吞吐量，还需要关注：

- P95/P99 查询延迟。
    
- CPU 使用率。
    
- 内存占用。
    
- 磁盘 I/O。
    
- Optimizer 任务负载。
    

当出现资源竞争时，可以考虑：

- 降低客户端并行度。
    
- 控制批量上传速率。
    
- 调整后台优化配置。
    
- 在业务低峰期执行大规模导入。
    
- 对在线搜索与批量导入设置资源预算。
    

具体参数应根据实际部署和 Qdrant 官方优化指南进行调整。

参考：[Read-Write Contention](https://qdrant.tech/documentation/guides/optimize/#read-write-contention)。

---

## 8. 完整实操：批量上传性能测试

这一部分是基于官方 API 补充的教学实验。

### 8.1 实验目标

比较三种方式：

|方案|Batch Size|Parallel|
|---|---|---|
|A：串行小批量|16|1|
|B：串行大批量|128|1|
|C：并行批量|128|4|

重点测量：

- 总上传时间。
    
- Points/s。
    
- Batch Size 的影响。
    
- Parallel 的影响。
    

### 8.2 环境准备

安装：

```
pip install -U qdrant-client numpy
```

启动 Qdrant：

```
docker run -d \
  --name qdrant \
  -p 6333:6333 \
  -v qdrant_storage:/qdrant/storage \
  qdrant/qdrant:latest
```

建议固定镜像版本后进行可重复性能测试，避免不同版本影响实验结果。

### 8.3 完整实验代码

保存为 `bulk_upload_benchmark.py`：

```
import time
import uuid

import numpy as np
from qdrant_client import QdrantClient, models


URL = "http://localhost:6333"

DIM = 128
N_POINTS = 10000

client = QdrantClient(url=URL)

rng = np.random.default_rng(42)

vectors = rng.normal(
    size=(N_POINTS, DIM)
).astype(np.float32)


def create_collection(name):
    client.create_collection(
        collection_name=name,
        vectors_config=models.VectorParams(
            size=DIM,
            distance=models.Distance.COSINE,
        ),
    )


def generate_points():
    for i, vector in enumerate(vectors):
        yield models.PointStruct(
            id=i,
            vector=vector.tolist(),
            payload={
                "document_id": f"doc_{i // 100}",
                "chunk_id": i,
            },
        )


def benchmark(label, batch_size, parallel):
    collection_name = (
        f"bulk_{uuid.uuid4().hex[:12]}"
    )

    create_collection(collection_name)

    start = time.perf_counter()

    client.upload_points(
        collection_name=collection_name,
        points=generate_points(),
        batch_size=batch_size,
        parallel=parallel,
        max_retries=3,
        wait=True,
    )

    elapsed = time.perf_counter() - start

    info = client.get_collection(
        collection_name=collection_name
    )

    print(f"\n方案: {label}")
    print(f"Point 数量: {info.points_count}")
    print(f"总耗时: {elapsed:.3f}s")
    print(f"吞吐量: {N_POINTS / elapsed:.1f} points/s")

    return elapsed


if __name__ == "__main__":
    benchmark("Batch16-Serial", 16, 1)
    benchmark("Batch128-Serial", 128, 1)
    benchmark("Batch128-Parallel4", 128, 4)
```

注意：

- 每组使用不同 Collection，避免覆盖或重复写入。
    
- `uuid` 用于生成唯一的 Collection 名称。
    
- 代码使用随机向量，避免所有向量完全相同。
    
- 该实验没有隔离后台索引构建和磁盘缓存等因素。
    
- 在正式测试前应进行预热、多次重复测量，并记录服务器资源占用。
    
- 测试产生的 Collection 需要在实验后清理。
    
- Windows 上的并行上传应使用 `if __name__ == "__main__"` 保护入口。
    

### 问题 16：为什么测试可能出现 Parallel=4 比 Parallel=1 更慢？

可能原因：

1. 数据集太小。
    
2. 多进程启动和序列化成本占比较高。
    
3. 客户端或网络已经成为瓶颈。
    
4. 服务端只有有限的写入处理能力。
    
5. 后台 Optimizer 正在消耗资源。
    
6. 磁盘写入性能已经达到上限。
    

因此，不应该只根据一次实验结果判断并行上传是否有效。

---

## 9. 进阶实验：Shard 数量对吞吐量的影响

### 问题 17：多个 Shard 是否真的能提高导入速度？

实验设计：

|Collection|Shard 数量|Parallel|
|---|---|---|
|Test A|1|1|
|Test B|1|4|
|Test C|2|4|
|Test D|4|4|

创建 Collection 时：

```
def create_sharded_collection(
    name,
    shard_number,
):
    client.create_collection(
        collection_name=name,
        vectors_config=models.VectorParams(
            size=128,
            distance=models.Distance.COSINE,
        ),
        shard_number=shard_number,
    )
```

保持其他配置一致。

观察：

- 写入吞吐量。
    
- CPU 利用率。
    
- 磁盘 I/O。
    
- Segment 数量。
    
- 后台索引优化时间。
    

**实验重点不是证明 Shard 越多越好，而是理解写入并行度与分片管理开销之间的权衡。**

---

## 10. 应用到 RAG 文件入库系统

### 问题 18：Qdrant Bulk Upload 是否等于完整的文件异步入库系统？

不是。

在 RAG 中，上传文件到可检索状态通常包含多个阶段：

```
File Upload
    ↓
File Parsing
    ↓
Chunking
    ↓
Embedding
    ↓
Qdrant Bulk Upload
    ↓
Status Update
```

Bulk Upload 只优化 Qdrant 写入阶段。

真正的系统还需要解决：

- 大量文件并发上传。
    
- 任务排队与背压。
    
- Embedding API 限流。
    
- 失败重试。
    
- 部分成功写入。
    
- 幂等性。
    
- 入库状态回写。
    
- 用户取消或重新处理任务。
    

这些是更广泛的异步任务调度与数据处理流水线问题，不属于本篇 Qdrant 官方文档的直接讨论范围。

### 10.1 一个简单的入库函数

假设解析、切分和 Embedding 已经完成：

```
from qdrant_client import models

def write_chunks(
    client,
    collection_name: str,
    document_id: str,
    embeddings: list[list[float]],
    chunks: list[str],
):
    if len(embeddings) != len(chunks):
        raise ValueError(
            "embeddings and chunks 数量不一致"
        )

    points = [
        models.PointStruct(
            id=str(
                uuid.uuid5(
                    uuid.NAMESPACE_URL,
                    f"{document_id}:{i}"
                )
            ),
            vector=embedding,
            payload={
                "document_id": document_id,
                "chunk_index": i,
                "content": chunk,
            },
        )
        for i, (embedding, chunk) in enumerate(
            zip(embeddings, chunks)
        )
    ]

    client.upload_points(
        collection_name=collection_name,
        points=points,
        batch_size=128,
        parallel=2,
        wait=True,
        max_retries=3,
    )
```

这里使用确定性 UUID，使相同 `document_id + chunk_index` 对应相同 Point ID，从而方便重复上传时进行 Upsert。

不过，如果修改了分片策略或 Chunk 数量，仍需要额外处理旧 Point 的清理问题。

### 10.2 如何衡量整个入库流水线？

不能只看 Qdrant 上传速度。

还需要按阶段记录：

|阶段|指标|
|---|---|
|File Parsing|files/s、解析耗时|
|Chunking|chunks/s|
|Embedding|vectors/s、API 延迟|
|Qdrant Upload|points/s|
|Status Update|回写延迟、成功率|
|整体任务|端到端耗时、失败率|

例如：

- Embedding：100 vectors/s。
    
- Qdrant：3000 points/s。
    

此时整个流水线的主要吞吐瓶颈更可能是 Embedding，而不是 Qdrant。

单纯增加 Qdrant 的 Shard 或 Parallel，未必能提升整个系统的入库速度。

---

## 11. 学习验收

### 原理问题

1. 为什么逐条 Upsert 的吞吐量通常不如批量写入？
    
2. Batch Size 太小与太大分别有什么问题？
    
3. Batch 和 Parallel 分别优化什么？
    
4. 为什么多个 Shard 可能提高写入吞吐量？
    
5. 为什么每个 Shard 的 WAL 和 Update Worker 很重要？
    
6. 为什么建议在批量导入前创建 Payload Index？
    
7. 为什么数据量超过 RAM 时，可以考虑 Cold Memory Tier？
    
8. `memmap_threshold` 和直接使用 Cold Storage 的主要区别是什么？
    
9. 为什么 Optimizer 会影响 Bulk Upload 的整体性能？
    
10. 为什么大量写入可能导致查询 P95 延迟升高？
    

### 实操任务

- 创建一个 Qdrant Collection。
    
- 生成 10000 条随机向量。
    
- 使用不同 Batch Size 导入。
    
- 测量上传耗时与 Points/s。
    
- 对比单 Worker 与多 Worker。
    
- 对比 1、2、4 个 Shard。
    
- 提前创建 Payload Index 后重复上传实验。
    
- 观察 CPU、RAM、磁盘 I/O。
    
- 记录后台 Optimizer 对上传性能的影响。
    
- 总结当前服务器最合适的 Batch、Parallel、Shard 组合。
    

---

## 12. 总结：Bulk Upload 的核心设计思想

Qdrant Bulk Upload 从三个层面优化数据写入。

### 第一层：客户端优化

减少请求固定开销，提高网络和客户端资源利用率。

对应：

- Batch Upload。
    
- Parallel Upload。
    
- 高性能客户端。
    

### 第二层：服务端存储优化

合理组织分片、索引和向量存储方式。

对应：

- Multiple Shards。
    
- Payload Index Before Ingestion。
    
- Cold Memory Tier。
    

### 第三层：系统资源协调

避免大规模写入破坏在线检索性能。

对应：

- WAL。
    
- Segment。
    
- Optimizer。
    
- Read-Write Contention。
    

最终应该将 Bulk Upload 理解为：

**在正确性和资源预算约束下，通过批处理、并行化、数据分片和存储布局优化，提高向量数据库的大规模写入吞吐量。**

但对于实际 RAG 系统，还必须进一步考虑数据处理流水线的端到端性能、任务失败重试与幂等性。

**工程实践的重点不是把 Batch、Parallel、Shard 全部调到最大，而是通过实验找到写入吞吐量、查询延迟和资源消耗之间的合理平衡。**