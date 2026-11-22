# Qdrant Quantization：向量量化原理、配置与实践

> **官方文档**：[Qdrant Quantization](https://qdrant.tech/documentation/manage-data/quantization/)
> 
> **前置知识**：Collection、Point、Segment、HNSW、向量相似度、Float32。
> 
> **学习目标**：理解为什么要量化、四种量化算法如何工作、量化如何影响向量检索，以及如何使用 Python 评估压缩率、Recall@K 和搜索延迟。

---

## 1. 为什么需要 Quantization？

### 问题 1：既然 HNSW 已经能加速向量检索，为什么还需要 Quantization？

HNSW 是一种近似最近邻（ANN）索引，通过构建多层近邻图，减少搜索期间需要访问的向量数量。

但它并没有直接解决另一个问题：

**每条被访问的向量依然可能占用大量内存，并需要进行高维浮点计算。**

假设一个 RAG 系统拥有：

- 100 万个 Chunk。
    
- 每个 Chunk 对应一个 1536 维 Embedding。
    
- 每个维度使用 Float32 存储。
    

每个 Float32 占用 4 字节：

[  
S=N\times D\times B  
]

其中：

- (N)：向量数量。
    
- (D)：向量维度。
    
- (B)：每个维度占用的字节数。
    

代入：

[  
S=10^6\times1536\times4  
=6.144\ GB  
]

仅向量主体就需要约 6.14 GB（十进制），还不包括 HNSW 索引、Payload、Segment 等开销。

即使 HNSW 减少了比较次数，每次向量比较仍需要读取大量浮点数据。

所以 Qdrant 引入 Quantization。

### 问题 2：Quantization 到底是什么？

**Quantization（量化）是将原始高精度向量转换成更紧凑的近似表示，以降低存储和搜索成本的技术。**

例如：

```
Float32:
[0.123456, 0.562891, -0.829146]

Quantized:
[31, 143, 22]
```

这里的整数只是示意。实际映射会涉及数值范围、比例或更复杂的变换，并不是直接去掉小数。

量化之后：

- 向量存储表示更小。
    
- 搜索时需要读取的数据可能更少。
    
- 可以利用低精度计算优化。
    
- 但不可避免地引入量化误差。
    

### 问题 3：量化优化的是哪些资源？

|资源|影响|
|---|---|
|RAM|压缩向量更容易驻留内存|
|Disk|量化表示本身更小，但可能额外保留原始向量|
|CPU|低精度计算和 SIMD 指令可能提升效率|
|I/O|减少搜索阶段需要读取的数据|
|Recall|量化误差可能改变最近邻排序|

**需要建立的认识：**

Indexing 与 Quantization 解决不同问题：

- Indexing：减少需要探索的 Point。
    
- Quantization：减少向量表示和相似度比较的成本。
    

两者可以结合使用。

---

## 2. 量化究竟牺牲了什么？

### 问题 4：为什么向量压缩后可能导致召回率下降？

量化会将连续浮点数映射到有限的离散表示。

假设两个原始数值：

```
0.101
0.103
```

在某种低精度映射中，它们可能得到相同编码。

原本不同的值变得无法区分。

向量搜索依赖距离或相似度：

[  
\operatorname{sim}(q,x)  
]

当向量 (x) 被量化为 (\hat{x}) 后：

[  
\operatorname{sim}(q,x)  
\ne  
\operatorname{sim}(q,\hat{x})  
]

因此，原本排名第一的 Point，量化后可能排到第二或更后。

例如：

|Point|原始相似度|量化相似度|
|---|---|---|
|A|0.952|0.931|
|B|0.947|0.941|
|C|0.901|0.897|

这些数字是教学示例，不是实测数据。

量化后的排序发生了变化：

```
原始：A > B > C
量化：B > A > C
```

**量化并不改变 Embedding 模型的语义能力，而是改变向量相似度计算的精确程度。**

最终需要权衡三个指标：

1. 压缩率（Compression Ratio）。
    
2. 检索延迟（Latency）。
    
3. 召回率（Recall）。
    

---

## 3. Scalar Quantization：标量量化

### 问题 5：能不能简单地把每个 Float32 数值改成 8-bit 整数？

可以，这就是 Scalar Quantization 的基本思路。

Qdrant 的 Scalar Quantization 将每个向量维度由 Float32 映射到 8-bit 整数表示。

理论压缩倍率：

[  
\frac{32}{8}=4  
]

### 3.1 量化原理

假设我们知道某个数值的范围：

[  
x \in [x_{\min},x_{\max}]  
]

使用 8 bit，可以提供 256 个离散等级。

一种简化的线性量化形式：

[  
q=\operatorname{round}  
\left(  
\frac{x-x_{\min}}{x_{\max}-x_{\min}}\times255  
\right)  
]

反量化：

[  
\hat{x}=x_{\min}+  
\frac{q}{255}(x_{\max}-x_{\min})  
]

实际 Qdrant 的量化实现和范围估计更复杂，这里用于说明低精度映射机制。

### 3.2 实操：自己实现一个简单的 8-bit 量化

```
import numpy as np

vector = np.array(
    [0.12, 0.56, 0.98, 0.34],
    dtype=np.float32
)

v_min = 0.0
v_max = 1.0

# 量化
quantized = np.round(
    (vector - v_min) / (v_max - v_min) * 255
).astype(np.uint8)

# 反量化
restored = (
    quantized.astype(np.float32) / 255
    * (v_max - v_min)
    + v_min
)

print("原始向量:", vector)
print("量化向量:", quantized)
print("恢复向量:", restored)
print("绝对误差:", np.abs(vector - restored))
```

观察：

- Float32 每维需要 4 字节。
    
- Uint8 每维只需要 1 字节。
    
- 恢复向量不一定等于原始向量。
    

### 3.3 在 Qdrant 中配置 Scalar Quantization

```
from qdrant_client import QdrantClient, models

client = QdrantClient(
    url="http://localhost:6333"
)

client.create_collection(
    collection_name="notes_scalar",
    vectors_config=models.VectorParams(
        size=768,
        distance=models.Distance.COSINE,
    ),
    quantization_config=models.ScalarQuantization(
        scalar=models.ScalarQuantizationConfig(
            type=models.ScalarType.INT8,
            quantile=0.99,
            memory=models.Memory.PINNED,
        )
    ),
)
```

主要参数：

|参数|作用|
|---|---|
|`type`|使用的标量量化类型，目前为 INT8|
|`quantile`|确定量化范围时使用的分位数|
|`memory`|量化向量采用的内存层级|

### 问题 6：为什么需要 quantile？

因为向量分布中可能存在极端值。

假设：

```
[0.11, 0.12, 0.13, 0.14, 100.0]
```

如果完全根据最大、最小值建立量化范围，大量离散等级可能浪费在少数极端值上。

`quantile=0.99` 表示估计范围时排除一部分极端值，使主要数值分布得到更有效的表示。

但这样可能使极端值产生更大误差。

因此该参数需要结合实际 Embedding 分布测试。

---

## 4. Binary Quantization：二值量化

### 问题 7：如果 8-bit 还不够小，能不能进一步压缩到 1-bit？

可以。

Binary Quantization 是更加激进的量化方式。

在最简单的 1-bit 情况下，可以通过数值正负符号生成编码：

```
原始向量：
[0.8, -0.2, 0.5, -0.7]

二值编码：
[1, 0, 1, 0]
```

每个维度从 32 bit 压缩为 1 bit：

[  
\frac{32}{1}=32  
]

即理论压缩 32 倍。

### 4.1 为什么 Binary Quantization 可能非常快？

二进制向量可以利用 CPU 的位运算与 POPCNT 等指令快速计算相似关系。

对于适当的编码，点积排序与 Hamming Distance（汉明距离）之间可以建立对应关系。

汉明距离用于计算两个二进制序列不同的位置数量：

```
A = 101100
B = 100110

不同位置：2
Hamming Distance = 2
```

### 4.2 Binary 的主要问题

1-bit 只能表达有限状态。

例如：

```
0.001 → 1
0.999 → 1
```

幅值信息被大量丢弃。

这使得 Binary Quantization 对向量维度和数值分布较敏感。

官方指出，它更适合高维、各分量分布相对居中的向量。

### 4.3 为什么还存在 1.5-bit 和 2-bit？

为了减少二值量化的信息损失。

例如 2-bit 量化可以表示：

```
-1 → 00
 0 → 01
 1 → 11
```

这让接近零的数值能够得到更合理的表示。

对比：

|位宽|理论压缩倍率|
|---|---|
|1 bit|32×|
|1.5 bit|约 21×|
|2 bit|16×|

位宽越低不一定越好，需要考虑 Recall 和查询延迟。

### 4.4 Qdrant 配置示例

```
client.create_collection(
    collection_name="notes_binary",
    vectors_config=models.VectorParams(
        size=1536,
        distance=models.Distance.COSINE,
    ),
    quantization_config=models.BinaryQuantization(
        binary=models.BinaryQuantizationConfig(
            memory=models.Memory.PINNED,
        )
    ),
)
```

该配置使用 Binary Quantization 的默认编码参数。

**注意：Binary Quantization 通常建议配合 Rescoring，避免过大的精度损失。**

---

## 5. Product Quantization：乘积量化

### 问题 8：有没有办法不逐维压缩，而是将一组维度一起编码？

有，这就是 Product Quantization（PQ）。

### 5.1 核心思想

假设有一个 8 维向量：

```
vector = [
    0.1, 0.2,
    0.3, 0.4,
    0.5, 0.6,
    0.7, 0.8
]
```

把它划分为多个子向量：

```
Group 1: [0.1, 0.2]
Group 2: [0.3, 0.4]
Group 3: [0.5, 0.6]
Group 4: [0.7, 0.8]
```

对每个子向量位置建立聚类中心集合（Codebook）。

原始子向量不再保存完整浮点表示，而是保存最接近的聚类中心编号。

例如：

```
Group 1 → Code 17
Group 2 → Code 83
Group 3 → Code 12
Group 4 → Code 41
```

Qdrant 使用 256 个聚类中心，每个中心编号可以由一个字节表示。

### 5.2 PQ 为什么能够压缩？

假设一个子向量原本有 4 个 Float32：

[  
4\times4=16\ bytes  
]

如果能通过一个字节的聚类中心编号表示这个子向量，那么编码本身就由 16 字节下降为 1 字节。

但是还需要额外保存共享的 Codebook。

因此，实际空间成本不仅取决于每条向量的编码长度，还需要考虑码本及其他存储开销。

### 5.3 PQ 的缺点

- 需要训练或构建聚类中心。
    
- 距离计算不如 Scalar Quantization 那样适合 SIMD 优化。
    
- 量化误差可能较大。
    
- 查询速度不一定比其他方案快。
    

所以官方更倾向于在**内存预算非常紧张，而且可以接受一定搜索速度与精度损失**时使用 PQ。

### 5.4 Qdrant 配置示例

```
client.create_collection(
    collection_name="notes_product",
    vectors_config=models.VectorParams(
        size=768,
        distance=models.Distance.COSINE,
    ),
    quantization_config=models.ProductQuantization(
        product=models.ProductQuantizationConfig(
            compression=models.CompressionRatio.X16,
            memory=models.Memory.PINNED,
        )
    ),
)
```

这里 `X16` 表示目标压缩倍率为 16×。

---

## 6. TurboQuant：为什么还要先做随机旋转？

### 问题 9：Scalar 和 Binary 都能压缩，TurboQuant 解决了什么问题？

前面的方法都受到原始向量数值分布的影响。

尤其是 Binary Quantization：

- 某些维度数值集中。
    
- 某些维度具有更多变化。
    
- 不同维度的信息分布不均匀。
    

直接降低 bit 数，可能造成比较大的信息损失。

TurboQuant 的解决思路是：

**先对向量进行随机旋转，重新分布信息，再进行低比特量化。**

### 6.1 基本过程

```
Original Float32 Vector
         |
         v
Fast Random Rotation
         |
         v
Quantization Mapping
         |
         v
Compressed Vector
```

假设：

[  
x\in\mathbb{R}^{d}  
]

应用旋转矩阵：

[  
y=Rx  
]

如果 (R) 是正交矩阵，则满足：

[  
R^TR=I  
]

对于两个向量 (x_1,x_2)：

|x_1-x_2|_2  
]

也就是说，**理想的正交旋转本身不会改变向量之间的欧氏距离**。

之后再对旋转后的向量进行量化。

TurboQuant 借助这一性质改善低比特编码的表现。

### 6.2 TurboQuant 支持哪些位宽？

|编码|位宽|理论压缩倍率|
|---|---|---|
|`bits8`|8 bit|4×|
|`bits4`|4 bit|8×|
|`bits2`|2 bit|16×|
|`bits1_5`|1.5 bit|约 21×|
|`bits1`|1 bit|32×|

Qdrant 默认使用 `bits4`。

官方测试表明，它通常能够在较高压缩率下保持良好的召回效果，但并不保证对所有 Embedding 模型都有同样结果。

### 6.3 TurboQuant 的 Asymmetric Quantization

TurboQuant 默认采用非对称量化策略。

含义是：

- 数据库中存储的向量采用压缩表示。
    
- 查询向量的比较保留更高精度的信息。
    

这样可以减少查询向量进一步量化导致的误差。

### 6.4 Qdrant 配置示例

```
client.create_collection(
    collection_name="notes_turbo",
    vectors_config=models.VectorParams(
        size=768,
        distance=models.Distance.COSINE,
    ),
    quantization_config=models.TurboQuantization(
        turbo=models.TurboQuantQuantizationConfig(
            bits=models.TurboQuantBitSize.BITS4,
            memory=models.Memory.PINNED,
        )
    ),
)
```

这里：

- `BITS4`：使用 4-bit TurboQuant。
    
- `PINNED`：让量化向量常驻内存。
    

### 问题 10：TurboQuant 是否一定比 Scalar、Binary 好？

不是。

在相同压缩倍率下，官方测试呈现的典型取舍：

- TurboQuant 往往比 Binary 提供更好的 Recall。
    
- Binary 的搜索速度可能更快。
    
- Scalar 的压缩率较低，但精度损失通常较小。
    
- PQ 适合追求更极端的压缩。
    

因此，算法选型仍需要实际基准测试。

---

## 7. 量化后的向量到底如何存储？

### 问题 11：量化是不是直接覆盖原始 Float32 向量？

默认不是。

Qdrant 的 `quantization_config` 机制通常会在原始向量之外，额外维护量化表示。

可以理解为：

```
Point
 ├── ID
 ├── Payload
 ├── Original Vector
 │     └── Float32
 └── Quantized Representation
       └── Turbo / Scalar / Binary / Product
```

例如：

```
Original:
[0.1234, 0.5678, 0.9123]

Quantized:
[31, 145, 233]
```

搜索期间可以使用量化表示，而需要更准确的排序时可以重新访问原始向量。

**重要：4× 或 8× 压缩率指量化表示相对 Float32 的编码压缩，并不意味着数据库总体磁盘占用直接缩小相同比例。**

如果同时保存原始向量和量化副本，反而会产生额外的量化存储开销。

### 问题 12：它和 Vector Datatype（如 turbo4）有什么区别？

|对比|Vector Datatype|Quantization|
|---|---|---|
|配置位置|`vectors_config.datatype`|`quantization_config`|
|控制对象|原始向量存储表示|搜索使用的量化表示|
|是否保留完整 Float32|使用 Turbo 数据类型时不保留|默认 Float32 配置下保留|
|典型用途|降低原始向量存储成本|加速搜索和降低内存成本|

两者可以组合使用，但如果原始向量已经以有损 Turbo 格式保存，Rescoring 就无法恢复最初的 Float32 精度。

---

## 8. Quantized Search：量化之后如何搜索？

### 问题 13：量化向量和 HNSW 是什么关系？

需要区分：

- HNSW 是图索引，负责组织近邻搜索路径。
    
- Quantization 是向量编码，负责更紧凑、更低成本的相似度计算。
    

它们可以共同参与查询。

一种典型的查询过程：

```
Query Vector
     |
     v
HNSW / Candidate Search
     |
     v
Quantized Distance Scoring
     |
     v
Candidate Points
     |
     v
Optional Rescoring
     |
     v
Final Top-K
```

这是一种概念模型，具体执行计划还会受到 Segment、过滤条件、索引状态与查询参数影响，并不代表每个请求都有完全相同的阶段。

### 问题 14：什么是 Oversampling？

Oversampling（超额采样）用于扩大第一阶段的候选集合。

假设用户需要：

```
limit = 10
```

设置：

```
oversampling = 3.0
```

那么量化搜索可以先选择约 30 个候选，再使用原始向量进行重新评分，最终返回 10 条。

为什么？

因为量化会带来排序误差。

如果真正相关的 Point 因量化误差排在第 15 名，直接取 Top-10 就可能丢失它。

通过 Oversampling，它有机会进入后续精确重新评分阶段。

### 问题 15：什么是 Rescoring？

Rescoring（重新评分）是指：

**对量化搜索得到的候选 Point，重新读取原始向量计算相似度并排序。**

例如：

|Point|量化排名|Rescoring 后|
|---|---|---|
|A|3|1|
|B|1|2|
|C|2|3|

Rescoring 能改善一部分排序误差，但无法找回根本没有进入候选集的 Point。

此外，读取原始向量会产生额外成本，尤其在原始向量位于磁盘时。

### 8.1 实操：控制量化搜索行为

```
from qdrant_client import models

result = client.query_points(
    collection_name="notes_turbo",
    query=[0.12] * 768,
    limit=10,
    search_params=models.SearchParams(
        quantization=models.QuantizationSearchParams(
            ignore=False,
            rescore=True,
            oversampling=3.0,
        )
    ),
)

for point in result.points:
    print(point.id, point.score)
```

参数解释：

|参数|含义|
|---|---|
|`ignore=False`|允许使用量化向量|
|`rescore=True`|使用原始向量重新评分|
|`oversampling=3.0`|扩大候选集合至约 3 倍|

注意，`oversampling` 主要用于候选扩展与后续重排，不等于单纯提高 `limit`。

### 8.2 如何关闭某次查询的量化？

```
result = client.query_points(
    collection_name="notes_turbo",
    query=[0.12] * 768,
    limit=10,
    search_params=models.SearchParams(
        quantization=models.QuantizationSearchParams(
            ignore=True,
        )
    ),
)
```

`ignore=True` 表示本次查询不使用量化表示。

这个能力特别适合做 A/B 测试。

但注意：关闭量化不等于关闭 HNSW。要建立精确搜索基线，还需要设置 `exact=True`。

### 8.3 TurboQuant 默认 Rescoring 行为

|位宽|默认 Rescoring|
|---|---|
|8-bit|关闭|
|4-bit|关闭|
|2-bit|开启|
|1.5-bit|开启|
|1-bit|开启|

位宽越低，误差通常越大，因此 Qdrant 倾向于对更低 bit 的量化结果进行重新评分。

这些默认值可以通过查询参数覆盖。

---

## 9. Memory Tuning：为什么要区分 RAM 和 Disk？

### 问题 16：既然量化表示已经很小，为什么还要配置 memory？

因为向量检索不只受存储大小影响，还受到内存访问和磁盘 I/O 影响。

假设：

- 原始 Float32 向量：6 GB。
    
- 量化向量：约 0.75 GB。
    

如果量化向量能够常驻 RAM，而原始向量放在磁盘，那么搜索期间可以优先利用内存中的量化表示。

需要 Rescoring 时，再读取原始向量。

Qdrant 提供三种 Memory Tier：

|类型|作用|
|---|---|
|`pinned`|尽量确保数据常驻 RAM|
|`cached`|使用可回收的缓存|
|`cold`|主要保留在磁盘中|

### 9.1 常见部署策略

**方案 A：原始向量 Cached + 量化向量 Pinned**

特点：

- 查询通常比较快。
    
- 内存占用可能较高。
    
- 原始向量也可以利用系统缓存。
    

**方案 B：原始向量 Cold + 量化向量 Pinned**

特点：

- 内存占用更低。
    
- 量化搜索速度有保障。
    
- Rescoring 可能增加磁盘读取延迟。
    

方案 B 适合内存预算有限的大规模向量检索。

需要注意：这里的 Memory Tier 参数是 Qdrant 新版本的配置方式，旧版本可能使用 `on_disk`、`always_ram` 等不同参数。

---

## 10. 完整实操：建立量化搜索评测实验

这一部分不是官方文档的原样代码，而是基于其 API 整理的教学实验。

### 实验目标

创建两个 Collection：

1. `bench_float32`：不启用量化。
    
2. `bench_turbo4`：启用 TurboQuant 4-bit。
    

向两者插入相同的向量，比较搜索结果、Recall@10 和查询耗时。

为了更清楚地评估量化误差，先使用随机生成的向量。完成基础实验后，再替换成真实 Embedding 数据。

### 10.1 准备环境

启动支持 TurboQuant 的 Qdrant 服务端，建议使用兼容的较新版本。

例如使用 Docker：

```
docker run -d \
  --name qdrant \
  -p 6333:6333 \
  -p 6334:6334 \
  -v qdrant_storage:/qdrant/storage \
  qdrant/qdrant:v1.20.0
```

安装 Python 依赖：

```
pip install -U qdrant-client numpy
```

确认服务端：

```
curl http://localhost:6333
```

### 10.2 完整实验代码

保存为 `quantization_benchmark.py`：

```
import time

import numpy as np
from qdrant_client import QdrantClient, models


URL = "http://localhost:6333"

DIM = 768
N_POINTS = 3000
N_QUERIES = 30
TOP_K = 10

BASE = "bench_float32"
TURBO = "bench_turbo4"

client = QdrantClient(url=URL)

rng = np.random.default_rng(42)


# 1. 生成可重复的测试数据
def normalize(x):
    return x / np.maximum(
        np.linalg.norm(x, axis=1, keepdims=True),
        1e-12,
    )


vectors = normalize(
    rng.normal(size=(N_POINTS, DIM)).astype(np.float32)
)

queries = normalize(
    rng.normal(size=(N_QUERIES, DIM)).astype(np.float32)
)


# 2. 创建 Collection
def create_collection(name, quantized=False):
    if client.collection_exists(name):
        client.delete_collection(name)

    kwargs = {}

    if quantized:
        kwargs["quantization_config"] = (
            models.TurboQuantization(
                turbo=models.TurboQuantQuantizationConfig(
                    bits=models.TurboQuantBitSize.BITS4,
                    memory=models.Memory.PINNED,
                )
            )
        )

    client.create_collection(
        collection_name=name,
        vectors_config=models.VectorParams(
            size=DIM,
            distance=models.Distance.COSINE,
        ),
        **kwargs,
    )


create_collection(BASE)
create_collection(TURBO, quantized=True)


# 3. 插入完全相同的原始向量
def insert_vectors(name):
    batch_size = 128

    for start in range(0, N_POINTS, batch_size):
        end = min(start + batch_size, N_POINTS)

        points = [
            models.PointStruct(
                id=i,
                vector=vectors[i].tolist(),
                payload={"source_id": i},
            )
            for i in range(start, end)
        ]

        client.upsert(
            collection_name=name,
            points=points,
            wait=True,
        )


insert_vectors(BASE)
insert_vectors(TURBO)


# 4. 查询函数
def search(name, vector, params):
    start = time.perf_counter()

    result = client.query_points(
        collection_name=name,
        query=vector.tolist(),
        limit=TOP_K,
        search_params=params,
    )

    elapsed_ms = (
        time.perf_counter() - start
    ) * 1000

    ids = [p.id for p in result.points]

    return ids, elapsed_ms


# 5. 精确 Float32 基线
exact_params = models.SearchParams(
    exact=True,
)

# 6. Turbo4 搜索，不重新评分
quant_params = models.SearchParams(
    quantization=models.QuantizationSearchParams(
        ignore=False,
        rescore=False,
    ),
)

# 7. Turbo4 + 超额采样 + 重新评分
rescore_params = models.SearchParams(
    quantization=models.QuantizationSearchParams(
        ignore=False,
        rescore=True,
        oversampling=3.0,
    ),
)


# 8. Recall@K
def recall_at_k(actual, expected):
    return len(set(actual) & set(expected)) / len(expected)


# 9. 预热
for q in queries[:5]:
    search(BASE, q, exact_params)
    search(TURBO, q, quant_params)
    search(TURBO, q, rescore_params)


stats = {
    "turbo": {"recall": [], "time": []},
    "rescore": {"recall": [], "time": []},
    "exact": {"time": []},
}


# 10. 执行测试
for q in queries:
    ground_truth, t = search(
        BASE, q, exact_params
    )
    stats["exact"]["time"].append(t)

    ids, t = search(
        TURBO, q, quant_params
    )
    stats["turbo"]["recall"].append(
        recall_at_k(ids, ground_truth)
    )
    stats["turbo"]["time"].append(t)

    ids, t = search(
        TURBO, q, rescore_params
    )
    stats["rescore"]["recall"].append(
        recall_at_k(ids, ground_truth)
    )
    stats["rescore"]["time"].append(t)


# 11. 汇总
for name, item in stats.items():
    latency = np.asarray(item["time"])

    print(f"\n{name}")
    print("平均延迟(ms):", round(latency.mean(), 3))
    print("P95延迟(ms):", round(np.percentile(latency, 95), 3))

    if "recall" in item:
        print(
            f"Recall@{TOP_K}:",
            round(float(np.mean(item["recall"])), 4),
        )
```

### 10.3 如何理解实验结果？

执行：

```
python quantization_benchmark.py
```

你会得到三组结果：

- Exact Float32
    
- Turbo4（不重新评分）
    
- Turbo4 + Rescoring
    

需要观察：

|指标|解释|
|---|---|
|Recall@10|量化结果与精确 Top-10 的重合比例|
|Average Latency|平均查询耗时|
|P95 Latency|95% 查询不超过的耗时|
|Collection Storage|实际数据库存储开销|

注意：

- 示例使用 HTTP 客户端，测得的是端到端请求延迟，包含通信及序列化等开销。
    
- 3000 条随机向量属于教学规模，可能触发全扫描或尚未完成索引构建。
    
- HNSW 可能在后台优化后才生效。
    
- 量化副本也可能需要等待后台构建完成。
    
- 不同实验之间应保持相同的硬件、数据、查询和索引配置。
    
- 此实验不能直接证明大规模生产环境中的性能收益。
    

如果要隔离量化误差，可以在相同 Collection 中比较 `ignore=True` 和 `ignore=False`，并进一步控制 `exact` 参数。

### 10.4 如何查看 Collection 信息？

```
info = client.get_collection(
    collection_name=TURBO
)

print(info.points_count)
print(info.vectors_count)
print(info.segments_count)
print(info.config.quantization_config)
```

还可以检查 Docker Volume 的实际磁盘占用与 Qdrant 运行进程的内存指标。

不要仅通过 `points_count × 维度 × 位宽` 推断数据库的最终占用。

---

## 11. 进阶实验：研究 Rescoring 的作用

### 问题 17：Oversampling 越大，Recall 是否一定越高？

不一定严格单调，也不意味着所有数据集都有明显收益。

建议固定：

```
rescore = True
```

然后测试：

```
oversampling_values = [
    1.0,
    2.0,
    3.0,
    5.0,
]
```

对每一个参数组合记录：

|Oversampling|Recall@10|P95 Latency|
|---|---|---|
|1.0|待测|待测|
|2.0|待测|待测|
|3.0|待测|待测|
|5.0|待测|待测|

思考：

1. 为什么候选数量增加可能提高 Recall？
    
2. 为什么 Rescoring 无法找回未被第一阶段召回的 Point？
    
3. 为什么 Oversampling 增大可能提高查询延迟？
    
4. 什么时候即使开启 Rescoring，也无法恢复接近原始向量的搜索质量？
    

---

## 12. 如何选择量化方案？

基于 Qdrant 官方建议，可以从目标压缩倍率开始选择。

|目标压缩率|建议优先测试|
|---|---|
|4×|TurboQuant 8-bit|
|8×|TurboQuant 4-bit|
|16×|TurboQuant 2-bit / Binary 2-bit|
|约 21×|TurboQuant / Binary 1.5-bit|
|32×|TurboQuant / Binary 1-bit|
|最高 64×|Product Quantization|

特殊情况：

如果使用 Manhattan（L1）距离，在低压缩倍率场景中可以优先考虑 Scalar Quantization。TurboQuant 对 L1 需要更昂贵的计算过程。

对于小规模 RAG 知识库，通常应该先建立 Float32 检索基线，再评估量化是否必要。

对于数据量大、内存预算有限的系统，可以优先测试 TurboQuant 4-bit，并结合 Rescoring 和 Memory Tier 做进一步优化。

---

## 13. 和 Indexing 知识体系的关系

通过前面的两篇官方文档，可以形成一个比较完整的认识：

### Indexing

问题：**如何减少搜索范围？**

对应：

- Payload Index
    
- HNSW
    
- Filterable HNSW
    
- Sparse Vector Index
    

### Quantization

问题：**如何降低单个向量的存储和比较成本？**

对应：

- Scalar Quantization
    
- Binary Quantization
    
- Product Quantization
    
- TurboQuant
    

### Search

问题：**如何组织完整的检索过程？**

对应：

- Query Planning
    
- Filtering
    
- ANN Search
    
- Rescoring
    
- Oversampling
    
- Hybrid Search
    

### Storage / Optimizer

问题：**如何维护索引、压缩表示和底层存储结构？**

对应：

- Segment
    
- HNSW Index Building
    
- Quantized Representation
    
- Background Optimization
    
- Memory Tier
    

---

## 14. 学习验收

完成本章后，尝试不看资料回答以下问题。

### 原理题

1. HNSW 已经能够减少向量比较次数，为什么还需要 Quantization？
    
2. 为什么 Scalar Quantization 能达到 4× 压缩？它牺牲了什么？
    
3. Binary Quantization 为什么适合高维、数值分布居中的向量？
    
4. Product Quantization 与 Scalar Quantization 最大的机制区别是什么？
    
5. 为什么 TurboQuant 要先进行随机旋转？
    
6. 为什么 Quantization 不一定降低数据库的总体磁盘占用？
    
7. Vector Datatype 与 Quantization Config 有什么区别？
    

### 查询执行题

8. Oversampling 与 Rescoring 分别解决什么问题？
    
9. 为什么 Rescoring 不能保证恢复原始 Top-K？
    
10. 量化向量常驻 RAM、原始向量放在 Disk 的设计有什么好处和代价？
    
11. 使用 `ignore=True` 是否等于关闭 HNSW？为什么？
    
12. 如果关闭 Rescoring 后查询速度提升，但 Recall 明显降低，你会优先调整哪些参数？
    

### 实验任务

- 完成 Scalar Quantization 的 NumPy 手动实现。
    
- 创建不量化的 Float32 Collection。
    
- 创建 TurboQuant 4-bit Collection。
    
- 插入相同测试向量。
    
- 比较量化与非量化的搜索结果。
    
- 计算 Recall@10。
    
- 测试 Oversampling = 1、2、3、5。
    
- 比较 Rescoring 开启与关闭的差异。
    
- 记录查询耗时、内存及磁盘占用。
    
- 用真实 Embedding 向量重复实验。
    

---

## 15. 最终总结

Qdrant Quantization 主要解决高维向量在存储和搜索过程中产生的资源消耗问题。

四种量化方式：

- **Scalar**：逐维降低数值精度。
    
- **Binary**：使用极少 bit 编码向量。
    
- **Product**：对子向量进行聚类编码。
    
- **TurboQuant**：随机旋转后执行低比特量化。
    

量化搜索的关键机制：

- **Quantized Search**：利用低精度表示快速检索。
    
- **Oversampling**：扩大候选集合，降低相关结果被遗漏的风险。
    
- **Rescoring**：使用原始向量重新计算候选结果的相似度。
    
- **Memory Tier**：协调原始向量、量化向量在 RAM 与磁盘之间的存储策略。
    

量化方案的正确评价方式，不是只看压缩倍率，而是联合评估：

[  
\boxed{  
\text{Recall@K}  
+  
\text{Latency}  
+  
\text{Memory}  
+  
\text{Storage}  
}  
]

**核心结论：量化是一种通过表示精度和额外处理开销，换取更低搜索资源成本的工程优化技术。是否值得启用、采用哪一种算法，最终都应通过真实数据集与实际负载验证。**