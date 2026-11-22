# Qdrant Multitenancy：多租户数据隔离、分片与检索优化

> 官方教程：[Configure Multitenancy](https://qdrant.tech/documentation/manage-data/multitenancy/)
> 
> 前置知识：Collection、Shard、Segment、Point、Payload Index、HNSW、Filtering。
> 
> 学习目标：理解多租户数据架构，掌握 Payload 分区、自定义 Shard、分层多租户三种方案及其适用场景。

## 1. 为什么需要 Multitenancy？

### 问题 1：什么是多租户？

多租户（Multitenancy）是指多个用户、组织或企业，共享同一套应用或数据库基础设施，但各自的数据需要保持隔离。

例如，一个企业知识库 SaaS 产品拥有三个用户：

|用户|文档数量|向量数量|
|---|---|---|
|User A|10|1000|
|User B|50|5000|
|User C|10000|1000000|

当 User A 搜索「Agent 上下文管理」时，只能返回属于 User A 的文档。

这里产生两个问题：

- **数据隔离**：如何保证不同租户只能访问自己的数据？
    
- **资源管理**：如何避免大型租户影响小型租户的查询性能？
    

Qdrant 的 Multitenancy 主要解决共享数据库中的数据组织、检索范围及资源分配问题。

注意：数据库多租户机制不等于完整的应用权限系统。身份认证与授权仍然需要由应用负责。

### 问题 2：为什么不为每个用户创建一个 Collection？

一种直观方案：

```
Collection_user_A
Collection_user_B
Collection_user_C
...
```

虽然逻辑清晰，但每个 Collection 都有自己的资源管理开销。

当用户规模达到数千、数万时，大量小 Collection 会增加资源成本和管理复杂度。

Qdrant 官方建议：

**对于大量租户，通常优先采用共享 Collection，而不是为每个租户创建独立 Collection。**

只有当租户数量有限且存在严格隔离需求时，才考虑独立 Collection。

官方介绍三种共享 Collection 的多租户方案：

|方案|基本思想|适用场景|
|---|---|---|
|Partition by Payload|通过 Payload 字段区分租户|大量小型租户|
|User-defined Sharding|每个租户拥有独立 Shard|少量大型租户|
|Tiered Multitenancy|小租户共享，大租户独立|租户规模差异明显|

---

## 2. Shard 与 Segment：先理解数据组织结构

### 问题 3：Shard 到底是什么？与 Segment 有什么区别？

Qdrant 的数据组织可以简化为：

```
Collection
  └── Shard
        ├── Segment A
        │     ├── Point
        │     ├── Vector Storage
        │     ├── Payload Storage
        │     ├── Vector Index
        │     └── Payload Index
        │
        └── Segment B
              ├── Point
              └── ...
```

各层职责：

|层级|作用|
|---|---|
|Collection|逻辑数据集合，定义向量和索引配置|
|Shard|将 Collection 数据拆分为不同分片|
|Segment|Shard 内部的实际存储与索引管理单元|
|Point|最基本的数据记录，包括 ID、Vector、Payload|

**Shard 主要解决数据如何分布与扩展的问题。**

**Segment 主要解决一批 Point 如何存储、索引和优化的问题。**

Shard 可以分布在不同节点，也可以多个 Shard 位于同一节点。

Segment 属于具体 Shard，不是独立的跨节点分片。

### 问题 4：既然有 Segment，为什么还需要 Shard？

因为两者优化的层面不同。

Segment 使 Qdrant 能够分批维护索引、合并数据、执行后台优化。

Shard 则允许 Qdrant 将整个 Collection 的数据分区，并支持将不同部分部署在不同节点。

例如：

```
Collection: notes

Node A
  Shard 1
    Segment A
    Segment B

Node B
  Shard 2
    Segment C
    Segment D
```

当查询需要覆盖整个 Collection 时，Qdrant 可以在相关 Shard 内检索并汇总结果。

如果查询已经明确指定了 Shard，则可以只访问目标分片。

**因此，多租户架构选择的一个核心问题是：租户数据应该共享 Shard，还是分配到专属 Shard？**

---

## 3. 方案一：Partition by Payload

### 问题 5：能不能不拆分 Collection 和 Shard，仅通过 Payload 区分不同租户？

可以。

这是 Qdrant 推荐用于大量小租户的方式。

假设一个 Collection 内有以下 Point：

```
Point 1:
    payload = {"tenant_id": "user_A"}

Point 2:
    payload = {"tenant_id": "user_A"}

Point 3:
    payload = {"tenant_id": "user_B"}

Point 4:
    payload = {"tenant_id": "user_C"}
```

所有用户共享 Collection。

查询时，根据 `tenant_id` 限定数据范围：

```
tenant_id = user_A
```

这样可以在同一套索引和存储基础设施中管理大量用户。

### 3.1 创建多租户 Collection

```
from qdrant_client import QdrantClient, models

client = QdrantClient(
    url="http://localhost:6333"
)

client.create_collection(
    collection_name="tenant_notes",
    vectors_config=models.VectorParams(
        size=3,
        distance=models.Distance.COSINE,
    ),
)
```

这里使用 3 维向量，方便进行教学实验。

实际项目中应该根据 Embedding 模型的输出维度配置，例如 768 或 1536。

### 3.2 创建租户 Payload Index

```
client.create_payload_index(
    collection_name="tenant_notes",
    field_name="tenant_id",
    field_schema=models.KeywordIndexParams(
        type=models.KeywordIndexType.KEYWORD,
        is_tenant=True,
    ),
)
```

关键参数：

```
is_tenant=True
```

### 问题 6：`is_tenant=True` 和普通 Payload Index 有什么区别？

普通 Keyword Index 主要用于加速字段过滤。

设置 `is_tenant=True`，意味着告诉 Qdrant：

> 这个字段是用于区分租户的，查询通常会限制在同一个租户的数据范围内。

Qdrant 可以利用这一信息，在底层优化数据布局时将同一租户的向量更集中地存储。

例如，普通布局可能是：

```
A B C A C B A B C
```

租户局部性优化后，可以概念性地理解为：

```
A A A B B B C C C
```

这种优化有利于：

- 减少磁盘随机访问。
    
- 提高顺序读取效率。
    
- 改善租户内部检索性能。
    

注意：`is_tenant=True` 不意味着每个租户立即获得独立 Segment 或 Shard，也不意味着自动创建权限隔离。

官方说明该能力从 Qdrant v1.11.0 开始提供。

### 3.3 插入不同租户的 Point

```
client.upsert(
    collection_name="tenant_notes",
    points=[
        models.PointStruct(
            id=1,
            vector=[0.9, 0.1, 0.1],
            payload={
                "tenant_id": "user_A",
                "document_id": "doc_001",
            },
        ),
        models.PointStruct(
            id=2,
            vector=[0.1, 0.9, 0.1],
            payload={
                "tenant_id": "user_A",
                "document_id": "doc_002",
            },
        ),
        models.PointStruct(
            id=3,
            vector=[0.1, 0.1, 0.9],
            payload={
                "tenant_id": "user_B",
                "document_id": "doc_003",
            },
        ),
    ],
    wait=True,
)
```

这里三个 Point 位于同一个 Collection，但属于两个不同用户。

### 3.4 查询时过滤租户

```
results = client.query_points(
    collection_name="tenant_notes",
    query=[0.1, 0.1, 0.9],
    query_filter=models.Filter(
        must=[
            models.FieldCondition(
                key="tenant_id",
                match=models.MatchValue(
                    value="user_A"
                ),
            )
        ]
    ),
    limit=10,
    with_payload=True,
)

for point in results.points:
    print(point.id, point.payload)
```

尽管查询向量可能与 User B 的 Point 3 很相似，但过滤条件只允许返回 User A 的 Point。

### 问题 7：如果忘记添加 tenant_id 过滤，会怎样？

数据库不会因为设置了 `is_tenant=True` 就自动拒绝跨租户检索。

因此，**租户过滤必须由受信任的服务端业务逻辑统一执行**。

对于 NoteAgent，推荐：

```
def search_user_notes(
    client,
    authenticated_user_id: str,
    embedding: list[float],
):
    return client.query_points(
        collection_name="tenant_notes",
        query=embedding,
        query_filter=models.Filter(
            must=[
                models.FieldCondition(
                    key="tenant_id",
                    match=models.MatchValue(
                        value=authenticated_user_id
                    ),
                )
            ]
        ),
        limit=5,
    )
```

其中 `authenticated_user_id` 必须来自服务端已验证的用户身份，而不是直接信任客户端自行提交的租户 ID。

同样，读取、更新、删除、批量导出等接口也需要进行租户授权。

---

## 4. 多租户场景中的 HNSW 优化

### 问题 8：共享 Collection 后，所有租户还需要共用一张全局 HNSW 图吗？

不一定。

假设 10000 个用户共享 Collection，每个用户只有少量向量。

但查询总是包含：

```
tenant_id = user_A
```

此时用户 A 的向量搜索并不需要与用户 B、C、D 等租户建立大量跨租户的近邻关系。

如果仍然为整个 Collection 建立全局 HNSW 图，会带来额外的索引构建和存储开销。

Qdrant 官方提供了一种优化方式：

```
client.create_collection(
    collection_name="tenant_optimized",
    vectors_config=models.VectorParams(
        size=768,
        distance=models.Distance.COSINE,
    ),
    hnsw_config=models.HnswConfigDiff(
        m=0,
        payload_m=16,
    ),
)
```

### 4.1 m 和 payload_m 分别是什么？

|参数|作用|
|---|---|
|`m`|控制普通 HNSW 图的连接|
|`payload_m`|控制 Payload-aware HNSW 图的连接|

配置：

```
m = 0
payload_m = 16
```

表示：

- 关闭普通的全局 HNSW 图。
    
- 仍允许为已建立索引的 Payload 字段构建过滤感知的 HNSW 连接。
    

`payload_m` 必须显式设置为非零值，否则如果默认继承 `m=0`，过滤感知连接也会被关闭。

### 问题 9：`m=0` 是不是意味着关闭向量索引、只能暴力搜索？

不是。

这是一个重要区别。

当 `payload_m > 0`，并且有对应 Payload Index 时，Qdrant 仍然可以构建过滤感知的 HNSW 连接。

所以：

**m=0 不等于 Exact Search。**

它主要是关闭全局 HNSW 结构，保留与过滤条件相关的索引能力。

### 4.2 这种优化有什么限制？

官方指出两个重要问题。

**第一：全局查询可能变慢。**

如果查询不带 `tenant_id`，就无法直接利用原本的全局近邻图。

因此，如果系统经常需要跨所有用户进行语义检索，这种配置可能不合适。

**第二：稀疏检索的统计信息不一定自动隔离。**

这涉及下一节的 Per-Tenant Scoring Statistics。

---

## 5. Per-Tenant Scoring Statistics

### 问题 10：为什么查询已经使用租户过滤，BM25 评分还可能受到其他租户影响？

因为 BM25 不仅检查某个文档是否包含 Query 词项，还需要使用语料库统计信息。

例如：

- 文档总数量。
    
- 某个词出现于多少篇文档。
    
- 平均文档长度。
    

其中一个重要概念是 IDF：

[  
IDF(t)  
]

它用于衡量某个词项在语料库中的稀有程度。

假设：

User A 的知识库包含大量 Python 文档。

User B 的知识库包含大量法律文档。

如果两个用户共享 Shard，而 BM25 统计信息基于整个 Shard 计算，那么 User B 的数据可能影响 User A 的词项评分。

### 5.1 Retrieval Filter 与 Statistics Corpus Filter

两者需要区别：

|类型|决定什么|
|---|---|
|Retrieval Filter|哪些 Point 可以进入结果|
|IDF Corpus Filter|使用哪些 Point 计算 BM25/IDF 统计信息|

例如：

```
Retrieval Filter:
  tenant_id = user_A
  AND year = 2024

IDF Corpus Filter:
  tenant_id = user_A
```

含义：

- 只返回 User A 在 2024 年的相关文档。
    
- 但评分统计使用 User A 的全部文档。
    

这样能够避免其他用户数据对该用户 BM25/IDF 统计造成不必要的影响。

### 5.2 官方 Python 查询示例

下面是官方文档中相关查询的简化形式，前提是 Collection 已配置可用的全文检索字段。

```
client.query_points(
    collection_name="tenant_notes",
    query=models.TextQuery(
        text="time travel"
    ),
    using="title",
    query_filter=models.Filter(
        must=[
            models.FieldCondition(
                key="tenant_id",
                match=models.MatchValue(
                    value="user_A"
                ),
            ),
            models.FieldCondition(
                key="year",
                match=models.MatchValue(
                    value=2024
                ),
            ),
        ]
    ),
    search_params=models.SearchParams(
        idf=models.IdfCorpusParams(
            corpus=models.Filter(
                must=[
                    models.FieldCondition(
                        key="tenant_id",
                        match=models.MatchValue(
                            value="user_A"
                        ),
                    )
                ]
            )
        )
    ),
    limit=10,
)
```

注意：这段代码不是前面 3 维演示 Collection 的可直接运行查询。它要求相应的全文检索配置与字段已存在。

官方说明 Per-Tenant Scoring Statistics 从 v1.19.0 开始支持。

---

## 6. 方案二：User-defined Sharding

### 问题 11：为什么有 Payload 隔离，还需要专属 Shard？

Payload 分区让租户共享底层分片。

但是当某个租户数据量非常大时，可能产生：

- 大量写入。
    
- 大规模索引构建。
    
- 较高的检索吞吐量。
    
- 更多内存与磁盘需求。
    

此时，多个大型租户继续共享同一个 Shard 可能出现资源竞争。

因此，可以为每个大型租户分配独立 Shard。

### 6.1 架构示例

```
Collection: enterprise_notes

  Shard company_A
    Segment 1
    Segment 2

  Shard company_B
    Segment 3
    Segment 4

  Shard company_C
    Segment 5
```

每个 Shard 管理对应租户的数据。

查询时可以直接路由到目标 Shard，而不是扫描其他企业的分片。

### 6.2 创建自定义分片 Collection

```
client.create_collection(
    collection_name="enterprise_notes",
    vectors_config=models.VectorParams(
        size=3,
        distance=models.Distance.COSINE,
    ),
    shard_number=1,
    sharding_method=models.ShardingMethod.CUSTOM,
)
```

关键参数：

```
sharding_method=models.ShardingMethod.CUSTOM
```

表示采用用户自定义分片方式。

### 6.3 为租户创建 Shard Key

```
client.create_shard_key(
    collection_name="enterprise_notes",
    shard_key="company_A",
)

client.create_shard_key(
    collection_name="enterprise_notes",
    shard_key="company_B",
)
```

Shard Key 是用于将请求路由至特定分片的标识。

**Shard Key 与 Payload 的 tenant_id 是两个不同概念。**

- `tenant_id`：Point 的业务字段，可以用于过滤。
    
- `shard_key`：分片路由信息，用于选择存储和查询的分片。
    

两者可以使用相同的租户 ID，但并不是同一种机制。

### 6.4 插入数据时指定 Shard

```
client.upsert(
    collection_name="enterprise_notes",
    points=[
        models.PointStruct(
            id=1,
            vector=[0.9, 0.1, 0.1],
            payload={
                "tenant_id": "company_A"
            },
        )
    ],
    shard_key_selector="company_A",
    wait=True,
)
```

此时 Point 会被路由到 `company_A` 对应的分片。

### 6.5 查询时指定 Shard

```
results = client.query_points(
    collection_name="enterprise_notes",
    query=[0.8, 0.2, 0.1],
    shard_key_selector="company_A",
    limit=5,
)
```

此查询只访问指定的 Shard Key 所对应的分片。

如果没有指定分片，查询可能需要覆盖更多 Shard。

### 问题 12：专属 Shard 是否意味着专属物理服务器？

不是。

Shard 是逻辑和物理存储分区，不等价于独占一台机器。

同一台服务器可以承载多个 Shard。

因此，专属 Shard 可以提供更明确的数据分片边界，但如果要求严格的 CPU、内存和资源隔离，还需要进一步规划集群节点与部署方式。

### 6.6 什么时候不适合独立 Shard？

如果每个用户只有几十条向量，却为每个用户创建独立 Shard，那么会产生大量额外管理和索引开销。

因此 User-defined Sharding 更适合：

- 租户数量相对有限。
    
- 单租户数据规模较大。
    
- 需要独立分片路由。
    
- 希望减少大型租户之间的相互影响。
    

---

## 7. 方案三：Tiered Multitenancy

### 问题 13：如果既有大量小用户，又有少量大型客户，应该采用哪种方案？

可以使用 Tiered Multitenancy（分层多租户）。

基本思想：

**小租户共享 Fallback Shard，大租户使用 Dedicated Shard。**

例如：

```
Collection: notes

  Fallback Shard
    User A
    User B
    User C
    User D
    ...

  Dedicated Shard: Enterprise X

  Dedicated Shard: Enterprise Y
```

这样能够避免大量小 Shard 的资源浪费，同时允许大型租户独立扩展。

### 7.1 创建分层多租户 Collection

```
client.create_collection(
    collection_name="tiered_notes",
    vectors_config=models.VectorParams(
        size=3,
        distance=models.Distance.COSINE,
    ),
    shard_number=1,
    sharding_method=models.ShardingMethod.CUSTOM,
)
```

这里 `shard_number=1` 是官方当前分层迁移方案的重要约束。

### 7.2 创建 Fallback Shard

```
client.create_shard_key(
    collection_name="tiered_notes",
    shard_key="default",
    shards_number=1,
)
```

`default` 是共享分片的路由标识。

所有没有专属 Shard 的小租户都可以使用它。

### 7.3 创建租户 Payload Index

```
client.create_payload_index(
    collection_name="tiered_notes",
    field_name="tenant_id",
    field_schema=models.KeywordIndexParams(
        type=models.KeywordIndexType.KEYWORD,
        is_tenant=True,
    ),
)
```

注意：分层多租户仍然需要 Payload 租户字段。

因为多个小租户共享同一 Fallback Shard，需要通过 Payload 区分各自的数据。

### 问题 14：写入时怎么知道应该使用哪个 Shard？

Qdrant 提供 Target + Fallback 的路由机制：

```
models.ShardKeyWithFallback(
    target="user_A",
    fallback="default",
)
```

其逻辑是：

1. 如果 `user_A` 对应的专属 Shard 存在且可用，写入该 Shard。
    
2. 如果不存在，则写入 `default` 共享 Shard。
    

### 7.4 分层写入示例

```
client.upsert(
    collection_name="tiered_notes",
    points=[
        models.PointStruct(
            id=1,
            vector=[0.9, 0.1, 0.1],
            payload={
                "tenant_id": "user_A"
            },
        )
    ],
    shard_key_selector=models.ShardKeyWithFallback(
        target="user_A",
        fallback="default",
    ),
    wait=True,
)
```

此时，如果还没有 `user_A` 的专属分片，Point 就进入 `default`。

### 7.5 分层查询示例

```
results = client.query_points(
    collection_name="tiered_notes",
    query=[0.8, 0.2, 0.1],
    query_filter=models.Filter(
        must=[
            models.FieldCondition(
                key="tenant_id",
                match=models.MatchValue(
                    value="user_A"
                ),
            )
        ]
    ),
    shard_key_selector=models.ShardKeyWithFallback(
        target="user_A",
        fallback="default",
    ),
    limit=5,
)
```

这里同时使用：

- Shard 路由。
    
- Payload 过滤。
    

两者并不冲突。

Shard 路由负责缩小需要访问的分片范围，Payload 过滤负责限制租户的数据范围。

---

## 8. Tenant Promotion：租户升级为专属 Shard

### 问题 15：某个小用户逐渐成长为大型客户，是否需要重新创建 Collection？

不需要。

Qdrant 支持将共享 Shard 中的租户数据迁移至新的独立 Shard。

这一过程称为 Tenant Promotion。

### 8.1 基本流程

```
阶段一：
User A 数据位于 Fallback Shard

阶段二：
创建 User A 专属 Shard（Partial）

阶段三：
将 User A 的 Point 复制到专属 Shard

阶段四：
完成同步，专属 Shard 进入 Active

阶段五：
后续请求路由至专属 Shard

阶段六：
清理共享 Shard 中的旧数据
```

### 8.2 创建专属 Shard

```
client.create_shard_key(
    collection_name="tiered_notes",
    shard_key="user_A",
    shards_number=1,
    replication_factor=1,
    initial_state=models.ReplicaState.PARTIAL,
)
```

关键参数：

|参数|作用|
|---|---|
|`shards_number=1`|创建一个目标分片|
|`replication_factor=1`|初始只创建一个副本|
|`PARTIAL`|表示分片尚未完成数据同步|

官方要求新专属分片初始使用单 Shard、单副本配置，以配合当前的数据迁移机制。

### 8.3 迁移租户数据

```
client.cluster_collection_update(
    collection_name="tiered_notes",
    cluster_operation=models.ReplicatePointsOperation(
        replicate_points=models.ReplicatePoints(
            from_shard_key="default",
            to_shard_key="user_A",
            filter=models.Filter(
                must=[
                    models.FieldCondition(
                        key="tenant_id",
                        match=models.MatchValue(
                            value="user_A"
                        ),
                    )
                ]
            ),
        )
    ),
)
```

这个操作告诉 Qdrant：

> 将 default 共享分片中属于 user_A 的 Point 复制到 user_A 专属分片。

这属于集群管理操作，需要合适的分布式部署环境。不能将它当成一定适用于本地单节点实验的普通写入 API。

迁移完成且目标分片进入 `Active` 状态后，才可以考虑清理共享分片中对应租户的旧数据。

### 8.4 当前方案的限制

官方文档明确指出：

- Fallback Shard 必须是单 Shard。
    
- 专属租户 Shard 在迁移时也必须是单 Shard。
    
- 新的专属分片初始需要单副本。
    
- Fallback Shard 里的所有小租户仍然受到单 Shard 的存储和写入能力限制。
    
- 专属 Shard 存在额外开销，不应无限增加。
    

官方建议单集群不要创建超过约 1000 个专属 Shard。

文档还给出一个经验参考：当单个租户达到约 20000 个 Point 时，可以考虑是否值得为它创建独立 Shard。

这只是指导性阈值，并非自动升级条件。实际选型仍应考虑向量维度、写入频率、查询吞吐量、硬件和索引配置。

---

## 9. 三种方案的对比与选型

|维度|Payload Partition|Dedicated Shard|Tiered Multitenancy|
|---|---|---|---|
|数据组织|所有租户共享分片|租户使用专属分片|小租户共享、大租户独立|
|租户区分|Payload Filter|Shard Key|Shard Key + Payload|
|资源开销|较低|较高|按需增加|
|扩展方式|优化共享 Collection|新建独立 Shard|租户升级与迁移|
|适用规模|大量小租户|少量大型租户|大小租户混合|
|管理复杂度|低|中|较高|

### 问题 16：什么时候需要从 Payload Partition 升级为 Sharding？

可以观察以下情况：

1. 单个租户数据量持续增长。
    
2. 租户查询造成明显资源竞争。
    
3. 某个租户的写入与索引构建成本很高。
    
4. 需要对租户的数据分布进行更明确的控制。
    
5. 共享 Shard 已经接近存储或吞吐能力限制。
    

不需要仅仅因为“用户变多”就立即采用独立 Shard。

关键是租户数据规模、负载差异与现有分片资源是否能够满足要求。

---

## 10. 应用到 NoteAgent

假设 NoteAgent 后续支持多个用户上传文档并进行 RAG 检索。

一种常见的 Point 设计：

```
payload = {
    "tenant_id": "user_001",
    "document_id": "doc_001",
    "chunk_id": "chunk_001",
    "category": "AI",
    "content": "Agent Context Management ...",
}
```

### 初期方案

使用：

- 一个 `notes` Collection。
    
- `tenant_id` Keyword Index。
    
- `is_tenant=True`。
    
- 查询时自动注入租户过滤条件。
    

这是最容易实现和维护的方案。

### 数据增长阶段

如果大量小租户主要执行各自知识库内的查询，可以评估：

```
m=0
payload_m=16
```

减少全局 HNSW 图的索引开销。

但需要比较跨租户查询性能，以及按租户过滤的 Recall 和延迟。

### SaaS 规模阶段

如果出现少量大型企业客户，可以考虑：

- 小用户继续共享 Shard。
    
- 大型企业使用独立 Shard。
    
- 通过 Tiered Multitenancy 完成升级。
    

### 应用层的安全边界

这里还需要区分：

**数据库逻辑隔离不等于业务权限验证。**

FastAPI 服务端应该统一负责：

- 验证当前用户的身份。
    
- 写入 Point 时绑定真实租户 ID。
    
- 查询时注入租户过滤。
    
- 对 Point 的读取、更新、删除进行授权。
    
- 限制客户端对 Qdrant 管理接口的直接访问。
    

不能直接相信前端传入的 `tenant_id`。

---

## 11. 动手实验

### 实验一：Payload 租户隔离

目标：验证同一 Collection 内的不同租户是否可以通过过滤条件进行隔离。

步骤：

- 创建 `tenant_notes` Collection。
    
- 创建 `tenant_id` Keyword Index，并设置 `is_tenant=True`。
    
- 写入 User A、User B 的 Point。
    
- 查询 User A 的 Point。
    
- 验证结果中不存在 User B 的数据。
    
- 去掉租户过滤，观察查询结果的变化。
    

思考：

> 为什么 `is_tenant=True` 本身不能保证数据访问安全？

### 实验二：HNSW 多租户优化

目标：比较普通 HNSW 和 Payload-aware HNSW 的性能。

分别创建：

```
# 方案 A：普通 HNSW
models.HnswConfigDiff(
    m=16
)

# 方案 B：租户优化
models.HnswConfigDiff(
    m=0,
    payload_m=16
)
```

插入同一批多租户向量数据。

比较：

|指标|普通 HNSW|租户优化 HNSW|
|---|---|---|
|索引构建时间|待测|待测|
|租户内 Recall@10|待测|待测|
|租户内 P95 延迟|待测|待测|
|全局查询 P95 延迟|待测|待测|
|索引存储开销|待测|待测|

需要确保数据量足以触发索引构建，并且索引优化完成后再进行测试。

### 实验三：Custom Sharding

目标：理解 Shard Key 的作用。

- 创建 Custom Sharding Collection。
    
- 创建两个 Shard Key。
    
- 向不同 Shard 写入数据。
    
- 指定 Shard Key 执行查询。
    
- 观察查询能否访问其他 Shard 的 Point。
    

### 实验四：Tiered Multitenancy

目标：理解共享分片到专属分片的路由与迁移机制。

在支持相关功能的测试集群中：

- 创建 Fallback Shard。
    
- 插入多个小租户的 Point。
    
- 配置 Target + Fallback 路由。
    
- 创建一个租户的专属 Shard。
    
- 调用 `replicate_points`。
    
- 检查目标分片是否进入 Active 状态。
    
- 验证迁移前后的检索结果是否一致。
    
- 在确认数据完整后，清理共享分片中的旧副本。
    

---

## 12. 学习验收

尝试回答以下问题：

1. 为什么不建议为大量小租户分别创建 Collection？
    
2. Shard 与 Segment 的区别是什么？
    
3. `tenant_id` 和 `shard_key` 分别用于什么？
    
4. 为什么 `is_tenant=True` 不仅仅是普通字段索引？
    
5. `m=0`、`payload_m=16` 为什么适合部分多租户场景？
    
6. 为什么关闭全局 HNSW 可能导致跨租户搜索变慢？
    
7. 为什么 Payload Filter 已经隔离了检索结果，BM25 统计信息仍可能混用其他租户数据？
    
8. 为什么 Dedicated Shard 不等于独立物理服务器？
    
9. Tiered Multitenancy 中 Fallback 的作用是什么？
    
10. Tenant Promotion 为什么需要 Partial 状态与数据复制？
    

---

## 13. 总结

Qdrant Multitenancy 解决的是多个租户如何在共享数据库中组织数据、分配存储资源及优化检索的问题。

三个主要方案：

**Partition by Payload**

通过 `tenant_id` 对共享 Collection 进行逻辑数据分区，适合大量小租户。

**User-defined Sharding**

为租户创建独立 Shard，通过 Shard Key 路由查询和写入，适合数据量较大的租户。

**Tiered Multitenancy**

小租户共享 Fallback Shard，大租户使用 Dedicated Shard，必要时通过 Tenant Promotion 进行数据迁移。

对应三个重要概念：

- **Payload Index**：加速字段过滤，并可优化租户数据局部性。
    
- **Shard**：控制 Collection 的数据分片与请求路由。
    
- **Segment**：管理分片内部的 Point、存储和索引结构。
    

最终可以将多租户架构理解成三个层面的设计问题：

**逻辑隔离**：哪些数据属于哪个用户？

**物理组织**：不同用户的数据如何分布在 Shard 和 Segment 中？

**资源优化**：如何根据租户规模调整数据组织和索引策略？

掌握这些问题后，就能够根据实际业务负载设计从单用户知识库到多租户企业 RAG 平台的逐步演进方案。