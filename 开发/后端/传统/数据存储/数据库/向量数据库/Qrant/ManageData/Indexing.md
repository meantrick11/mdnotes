# Qdrant Indexing：索引机制与检索原理

> **官方文档**：[Qdrant Indexing](https://qdrant.tech/documentation/manage-data/indexing/)
> 
> **相关文档**：[Optimizer](https://qdrant.tech/documentation/ops-optimization/optimizer/) · [Search](https://qdrant.tech/documentation/search/) · [Filtering](https://qdrant.tech/documentation/search/filtering/)
> 
> **核心内容**：Segment、Payload Index、Full-Text Index、HNSW、Filterable HNSW、Sparse Vector Index、查询规划及索引优化。

## 1. Indexing 概述

Qdrant 的一个核心特性是**向量索引与传统索引的高效结合**。

这一点至关重要，因为要让向量搜索与过滤功能高效协同，仅依靠向量索引是不够的。

简单来说：

- **Vector Index（向量索引）**：加快向量相似度搜索。
    
- **Payload Index（载荷索引）**：加快基于 Payload 字段的过滤操作。
    
- **Sparse Vector Index（稀疏向量索引）**：支持稀疏向量的高效相似度检索。
    
- **Full-Text Index（全文索引）**：作为一种特殊 Payload Index，实现文本词项、短语匹配及可选的 BM25 排序。
    

### 1.1 为什么需要索引？

假设一个 Collection 包含 100 万条 Point。

如果没有索引：

- 向量检索可能需要遍历大量向量，逐一计算相似度。
    
- Payload 过滤可能需要检查大量 Point 的字段值。
    
- 带过滤条件的向量检索可能同时面临上述问题。
    

索引的本质是**使用额外的数据结构和存储开销，减少查询时需要访问或计算的数据量**。

### 1.2 索引体系

|索引类型|数据来源|主要用途|常见技术|
|---|---|---|---|
|Payload Index|Point.payload|精确匹配、范围过滤|字段专用索引|
|Full-Text Index|Payload 字符串字段|全文过滤、BM25 评分|分词、倒排索引|
|Dense Vector Index|Point.vector|语义相似度检索|HNSW|
|Sparse Vector Index|Point 的稀疏向量|稀疏语义或词项检索|专用稀疏索引|

需要注意：

**索引是加速数据访问的机制，不等于检索业务本身。**

例如，Hybrid Search 是通过组合 Dense、Sparse 或全文检索结果实现的，不存在一种必须单独创建的“混合索引”。

---

## 2. Collection、Shard、Segment 与 Point

### 2.1 Qdrant 的底层组织

Qdrant 中的 Segment（段）不是 Point 的 Payload 字段，而是 Qdrant 在底层组织、存储、索引和管理 Point 的物理单元。

基本层级：

```
Collection
 └── Shard
      ├── Segment A
      │    ├── Points
      │    ├── Vector Storage
      │    ├── Payload Storage
      │    ├── Vector Index
      │    ├── Payload Index
      │    └── ID Tracker
      │
      └── Segment B
           ├── Points
           ├── Vector Storage
           ├── Payload Storage
           ├── Vector Index
           ├── Payload Index
           └── ID Tracker
```

各层作用：

|层级|含义|职责|
|---|---|---|
|Collection|逻辑数据集合|配置向量、距离函数、索引参数等|
|Shard|数据分片|分布和管理部分 Point|
|Segment|底层存储管理单元|管理 Point、索引及存储结构|
|Point|基本数据记录|存储 ID、Vector、Payload|

### 2.2 Segment 为什么存在？

假设整个数据库使用一个巨大的存储和索引结构。

随着 Point 不断增加，索引构建、数据压缩、删除清理和结构重组会产生较高成本。

Qdrant 使用 Segment 将数据拆分为多个管理单元，从而让这些维护操作可以分批进行。

**但并不是每插入一个 Point，都需要重建整个 Segment。**

Qdrant 的 Segment 主要分为：

- **Appendable Segment**：允许新增 Point，适合接收持续写入的数据。
    
- **Non-appendable Segment**：不直接接收新 Point，可支持读取、删除等操作，通常具有优化后的索引结构。
    

典型过程：

1. 新 Point 写入可追加 Segment。
    
2. Segment 中数据量逐渐增加。
    
3. 达到一定阈值后，Optimizer 进行后台优化。
    
4. 构建 HNSW 等索引，或合并、压缩 Segment。
    
5. 后续新 Point 继续写入可追加 Segment。
    

Optimizer 通过后台重建及代理机制处理数据变更，以保持读取服务。

参见：[Qdrant Optimizer](https://qdrant.tech/documentation/ops-optimization/optimizer/)。

### 2.3 索引到底在哪个层面？

Qdrant 官方说明：

**索引参数在 Collection 级别配置，而各 Segment 内部的索引独立存在。**

例如：

```
client.create_payload_index(
    collection_name="notes",
    field_name="user_id",
    field_schema=models.PayloadSchemaType.INTEGER,
)
```

含义是：在整个 `notes` Collection 中，为 `user_id` 字段配置整数索引。

假设数据分布如下：

```
Collection: notes

Segment A
  Point 1: user_id = 1001
  Point 2: user_id = 1002

Segment B
  Point 3: user_id = 1001
  Point 4: category = "AI"
```

逻辑上的索引关系：

```
Segment A:
  1001 → Point 1
  1002 → Point 2

Segment B:
  1001 → Point 3
```

Point 4 没有 `user_id`，不要求补充该字段，也不会匹配 `user_id = 1001`。

以上是帮助理解的逻辑映射，并不是所有类型索引的真实内部结构。

执行查询时，Qdrant 可以分别利用各 Segment 的索引定位候选 Point，再汇总相关结果。

另外，**并非每个 Segment 都会自动构建全部索引**。对于向量索引，是否值得构建由 Optimizer 的配置和数据规模等因素决定。

---

## 3. Payload Index（有效载荷索引）

### 3.1 基本原理

Payload Index 与传统面向文档的数据库索引类似。

它针对 Point 的 Payload 中指定字段及数据类型建立索引，帮助根据过滤条件快速定位 Point。

例如：

```
{
    "id": 1,
    "vector": [0.12, 0.31, 0.82],
    "payload": {
        "user_id": 1001,
        "category": "AI",
        "score": 90
    }
}
```

我们可以对不同字段创建索引：

- `user_id`：用于用户过滤。
    
- `category`：用于分类过滤。
    
- `score`：用于数值范围过滤。
    

**每个 Point 的 Payload 可以拥有不同的 Key，Collection 并不要求 Payload 结构完全一致。**

### 3.2 Payload Index 的两种作用

**作用一：加速过滤（Filtering）**

假设有 100 万条 Point：

```
query_filter = models.Filter(
    must=[
        models.FieldCondition(
            key="user_id",
            match=models.MatchValue(value=1001),
        )
    ]
)
```

没有 Payload Index 时，仍然可能执行过滤，但需要检查更多数据。

存在对应索引时，Qdrant 可以更高效地定位满足条件的 Point。

**作用二：估计过滤基数（Cardinality Estimation）**

基数指满足过滤条件的 Point 数量。

例如：

|过滤条件|假设匹配数量|
|---|---|
|`category = "AI"`|300000|
|`user_id = 1001`|10000|
|`document_id = "doc_001"`|30|

不同过滤条件的候选集合规模不同，适合的搜索策略也不同。

- 如果只有 30 条，直接计算这 30 条向量的相似度可能比较高效。
    
- 如果还有 30 万条，使用 HNSW 等索引往往更合适。
    

Qdrant 的 Query Planner 可以利用索引估计信息，选择合适的查询策略。

参考：[Query Planning](https://qdrant.tech/documentation/search/search/#query-planning)。

### 3.3 支持的 Payload Index 类型

|类型|主要用途|示例|
|---|---|---|
|`keyword`|字符串匹配|`category = "AI"`|
|`integer`|整数匹配、范围过滤|`score >= 80`|
|`float`|浮点数范围过滤|`price < 20.5`|
|`bool`|布尔过滤|`is_public = True`|
|`geo`|地理范围过滤|指定半径内的位置|
|`datetime`|时间范围过滤|最近七天的数据|
|`text`|全文检索|文本包含指定词项|
|`uuid`|UUID 精确匹配|文档或租户标识|

其中，`keyword` 还可以配置前缀匹配等能力；`text` 则属于特殊的全文索引类型。

### 3.4 创建 Payload Index

```
from qdrant_client import QdrantClient, models

client = QdrantClient(
    url="http://localhost:6333"
)

client.create_payload_index(
    collection_name="notes",
    field_name="user_id",
    field_schema=models.PayloadSchemaType.INTEGER,
)
```

也可以创建多个字段索引：

```
client.create_payload_index(
    collection_name="notes",
    field_name="category",
    field_schema=models.PayloadSchemaType.KEYWORD,
)

client.create_payload_index(
    collection_name="notes",
    field_name="created_at",
    field_schema=models.PayloadSchemaType.DATETIME,
)
```

注意：

- 配置发生在 Collection 层面。
    
- 不要求所有 Point 都拥有索引字段。
    
- 索引会增加内存、磁盘及维护成本。
    
- 也可以使用点号路径为嵌套 Payload 字段创建索引。
    

### 3.5 哪些字段应该创建索引？

创建索引需要额外的计算资源和存储空间，因此 Qdrant 将索引字段的选择交给用户。

基本原则：

1. 优先考虑经常出现在过滤条件中的字段。
    
2. 优先考虑能明显缩小候选集合的字段。
    
3. 不必为仅用于返回、展示的字段建立索引。
    
4. 资源有限时，避免为所有 Payload Key 无差别创建索引。
    

一般而言，字段不同取值越多，越有机会通过精确过滤显著缩小结果集合，但具体收益还取决于数据分布和查询条件。

例如，NoteAgent：

```
payload = {
    "user_id": "user_001",
    "document_id": "doc_001",
    "category": "AI",
    "content": "FastAPI 的异步处理机制",
    "created_at": "2026-11-22T10:00:00Z"
}
```

建议：

|字段|是否需要索引|原因|
|---|---|---|
|`user_id`|多用户系统建议|限定用户数据|
|`document_id`|建议|限定文档检索范围|
|`category`|按需求|分类检索|
|`created_at`|按需求|时间过滤|
|`content`|按需求|全文搜索时建立 `text` 索引|

### 3.6 创建索引的时机

**建议在导入数据前创建需要的 Payload Index。**

这是因为 Qdrant 的 Filterable HNSW 可以根据已有 Payload Index 为向量图构建额外的过滤感知边。

如果先导入数据并建立了 HNSW，再创建 Payload Index，那么已有 HNSW 图可能无法立即充分利用这些额外边。

此时需要重建 HNSW 才能充分利用相关优化。

注意：

- 后创建 Payload Index 仍然可以使用它进行过滤。
    
- 需要重建的主要是与过滤条件协同的 HNSW 图结构，而不是必须重新上传全部 Point。
    

### 3.7 阻止未索引字段的过滤查询

Qdrant 提供 Strict Mode，可以阻止针对未创建索引的字段执行过滤查询。

原因是：

- 未索引字段的过滤可能非常缓慢。
    
- 大规模查询可能占用过多集群资源。
    
- 性能问题可能影响其他用户的请求。
    

例如，在支持的配置中设置：

```
{
  "strict_mode_config": {
    "enabled": true,
    "unindexed_filtering_retrieve": false
  }
}
```

该配置用于限制未经索引支持的检索过滤操作。实际部署还应根据 Qdrant 版本及其他查询类型检查相应 Strict Mode 参数。

价值：

- **Fail Fast**：错误查询直接失败。
    
- **性能保护**：减少意外的昂贵过滤查询。
    
- **运维可见性**：及时发现遗漏的字段索引。
    

### 3.8 参数化 Payload Index

Qdrant 允许针对不同字段索引配置额外参数。

#### Integer：Lookup 和 Range

整数索引支持：

- `lookup`：精确匹配。
    
- `range`：范围查询。
    

例如，只保留范围查询能力：

```
client.create_payload_index(
    collection_name="notes",
    field_name="score",
    field_schema=models.IntegerIndexParams(
        type=models.IntegerIndexType.INTEGER,
        lookup=False,
        range=True,
    ),
)
```

这适用于只需要查询 `score >= 80`、不需要精确匹配的场景。

#### Keyword：Prefix Matching

可以为 Keyword 字段启用前缀匹配。

例如：

```
/docs/python/fastapi
/docs/python/asyncio
/docs/java/spring
```

可以支持按 `/docs/python` 路径前缀筛选。

#### Tenant Index：多租户优化

对于经常通过 `tenant_id` 或 `user_id` 限定范围的系统，可以考虑 `is_tenant=True`。

它不仅是普通字段过滤，还可以帮助 Qdrant 优化同一租户数据的局部性。

#### Payload Index 的内存层级

|层级|特点|
|---|---|
|`pinned`|常驻内存，适合频繁查询|
|`cached`|使用缓存，允许在内存压力下被淘汰|
|`cold`|更依赖磁盘读取，节省内存|

索引始终需要持久化存储，但可以根据访问频率选择不同的内存管理策略。

---

## 4. Full-Text Index（全文索引）

### 4.1 与 Keyword Index 的区别

假设 Payload：

```
{
    "category": "AI",
    "content": "FastAPI supports asynchronous programming"
}
```

`keyword` 索引主要用于匹配字段值，例如：

```
category = "AI"
```

而全文索引允许根据正文中的词项、短语进行搜索，例如：

```
搜索 asynchronous
```

两者区别：

|类型|搜索对象|适合场景|
|---|---|---|
|Keyword Index|字段值|分类、ID、状态|
|Full-Text Index|文本词项、短语|文档正文搜索|
|Dense Vector Index|向量语义相似度|语义召回|

### 4.2 全文索引如何工作？

例如：

```
"FastAPI supports asynchronous programming"
```

经过 Tokenizer 分词后：

```
fastapi
supports
asynchronous
programming
```

概念上可以形成如下倒排关系：

```
fastapi       → Point 1、Point 5
asynchronous  → Point 1、Point 7
programming   → Point 1、Point 9
```

当搜索 `asynchronous` 时，就可以利用词项索引快速定位相关 Point。

**全文索引基于文本词项，不需要将文本转换成 Dense Embedding。**

### 4.3 创建 Full-Text Index

```
client.create_payload_index(
    collection_name="notes",
    field_name="content",
    field_schema=models.TextIndexParams(
        type=models.TextIndexType.TEXT,
        tokenizer=models.TokenizerType.WORD,
        lowercase=True,
    ),
)
```

其中：

- `tokenizer`：决定如何分词。
    
- `lowercase`：是否统一转换为小写。
    
- 还可以配置词干提取、停用词、短语匹配等功能。
    

对于中文内容，需要特别注意分词器是否能正确识别中文词语边界。

### 4.4 BM25 Scoring

普通全文索引主要帮助判断文本是否包含某些词。

而 BM25 用于进一步计算文本与 Query 的相关程度。

BM25 主要考虑：

- 查询词是否出现在文档中。
    
- 查询词出现的频率。
    
- 查询词在整个语料库中的稀有程度。
    
- 文档长度。
    

例如：

```
Query: FastAPI async
```

两个文档都包含这些词，但 BM25 可以根据相关性特征给出不同评分。

Qdrant 的 Full-Text Index 可通过启用 BM25 Scoring 实现相关性排序，且需要额外维护相关统计信息。

**全文过滤与 BM25 排序是两种独立能力，不应混为一谈。**

### 4.5 其他文本处理能力

|能力|作用|
|---|---|
|Tokenizer|将文本拆分为可检索词项|
|Lowercasing|大小写归一化|
|ASCII Folding|规范化部分带变音符号的字符|
|Stemmer|将单词归约为词干|
|Stopwords|过滤常见停用词|
|Phrase Matching|匹配连续、顺序一致的词项|

其中 Phrase Matching 需要对应索引配置支持，不能假设所有 Full-Text Index 默认支持。

---

## 5. Vector Index（向量索引）

### 5.1 向量索引解决什么问题？

假设 Collection 中包含 100 万条 Dense Vector。

现在输入一个 Query 向量：

```
query_vector = [0.12, 0.34, 0.78]
```

希望找到相似度最高的 5 条向量。

最简单的方式是：

1. 遍历全部向量。
    
2. 计算 Query 与每个向量的相似度。
    
3. 排序。
    
4. 返回 Top-5。
    

这属于全量扫描（Full Scan）。

问题是数据量越大，计算与读取成本越高。

向量索引则通过额外的数据结构，减少需要直接比较的向量数量。

### 5.2 HNSW

Qdrant 对 Dense Vector 采用的主要索引算法是 **HNSW（Hierarchical Navigable Small World）**。

HNSW 属于基于图的近似最近邻搜索算法（ANN）。

核心思想：

> 预先根据向量之间的距离构建多层近邻图，在查询时沿着图中的近邻关系逐渐接近目标向量。

HNSW 的多层结构：

- 上层图：节点较少，连接较稀疏，适合快速导航。
    
- 下层图：节点更多，邻居关系更细致。
    
- 底层图：包含完整的可搜索节点集合。
    

查询过程：

1. 从高层图的入口节点出发。
    
2. 寻找距离 Query 更近的邻居。
    
3. 逐渐下沉至更低层。
    
4. 在底层维护候选集合。
    
5. 返回近似最近邻结果。
    

**HNSW 不保证每次搜索得到精确 Top-K，但通常能显著减少计算量。**

### 5.3 HNSW 主要参数

|参数|作用|增大后的典型影响|
|---|---|---|
|`m`|图中节点连接数相关参数|图更密、内存开销更高，召回率可能提升|
|`ef_construct`|建图时探索候选的规模|索引构建更慢，图质量通常更好|
|`ef`|查询时的候选探索范围|召回率通常提升，查询开销增加|
|`full_scan_threshold`|全扫描与 HNSW 的切换阈值|影响查询规划策略|

其中：

- `m`、`ef_construct` 主要影响索引结构及构建过程。
    
- `ef` 主要影响查询时的搜索精度与性能。
    
- `full_scan_threshold` 决定小候选集合是否适合直接扫描。
    

### 5.4 HNSW 配置

```
client.create_collection(
    collection_name="notes",
    vectors_config=models.VectorParams(
        size=768,
        distance=models.Distance.COSINE,
    ),
    hnsw_config=models.HnswConfigDiff(
        m=16,
        ef_construct=100,
    ),
)
```

其中 `size=768` 是示例，需要与实际 Embedding 模型的输出维度一致。

---

## 6. Filterable HNSW：过滤与向量检索协同

这是整个 Indexing 文档的重点之一。

### 6.1 为什么仅有两个独立索引还不够？

假设你要搜索：

> 在 category = AI 的笔记中，找到与 Agent 上下文压缩最相似的内容。

查询同时包含：

```
Payload Filter:
category = AI

Vector Search:
similarity(query_vector, point_vector)
```

一种直观思路是：

1. 通过 Payload Index 筛选所有 AI 类型的 Point。
    
2. 在这些 Point 中寻找相似向量。
    

当满足条件的 Point 很少时，这样做比较高效。

但是，如果匹配几十万条 Point，直接计算所有相似度成本依然较高。

另一种思路是：

1. 使用 HNSW 搜索相似向量。
    
2. 在图遍历过程中排除不满足过滤条件的 Point。
    

问题是：HNSW 的连接基于向量近邻关系构建，而不是按照 Payload 字段构建。

### 6.2 过滤条件可能破坏 HNSW 图的连通性

例如：

```
A —— B —— C —— D —— E
```

其中：

```
A: AI
B: Python
C: Python
D: AI
E: AI
```

假设只能经过符合 `category = AI` 的节点。

原始近邻图中的某些路径会因为过滤条件而受到影响。

这样可能导致：

- 无法有效到达部分相关节点。
    
- 召回率下降。
    
- 查询需要探索更多候选节点。
    

### 6.3 Filterable HNSW 的解决方案

Qdrant 在建立 HNSW 图时，可以利用已经创建的 Payload Index，为图增加**与过滤条件相关的额外边**。

概念示例：

```
普通近邻连接：

A —— B —— C —— D —— E

额外的过滤感知连接：

A ————————— D —— E
```

这样在执行 `category = AI` 的向量检索时，图可以更有效地导航至满足过滤条件的节点。

需要注意：

- 额外边针对已索引的 Payload 字段构建。
    
- 它不是为所有可能的组合过滤条件分别构建一张完整的图。
    
- 创建额外边会增加构建时间、磁盘及内存开销。
    

### 6.4 查询策略如何选择？

可以概念性地分为以下三种情况：

|场景|典型策略|
|---|---|
|过滤后候选极少|Payload Index + 精确向量计算|
|过滤条件较宽松|HNSW 搜索并应用过滤|
|过滤程度居中|Filterable HNSW|

具体策略由 Qdrant Query Planner 和相关参数决定，并不存在固定的 Point 数量分界线。

### 6.5 ACORN 算法

即使采用 Filterable HNSW，复杂组合过滤仍可能导致图遍历困难。

例如：

```
category = AI
AND
user_id = 1001
AND
document_type = PDF
```

Qdrant 的额外边是按 Payload Index 分别构建的，不会为所有字段组合生成完整连接。

因此，在多个严格过滤条件共同作用下，仍可能出现图连接不足的问题。

ACORN 的核心思路是：

**当直接邻居无法满足过滤条件时，扩大对邻居的探索，例如进一步检查二跳邻居。**

它可以改善严格过滤条件下的搜索召回率，但会增加计算开销。

### 6.6 如何关闭过滤感知额外边？

如果某个 Payload 字段只用于 Sparse Vector 检索，而不用于 Dense Vector + HNSW 检索，那么可能没有必要构建对应的 HNSW 额外边。

可通过索引配置中的：

```
enable_hnsw=False
```

关闭相关优化。

这属于进一步的性能与资源调优。

### 6.7 重建 HNSW

如果数据已经建立了 HNSW 索引，之后才创建新的 Payload Index，那么可能需要重建 HNSW 才能充分利用过滤感知额外边。

官方建议通过小幅修改 HNSW 参数触发重建，例如调整 `ef_construct`。

但重建 HNSW 属于资源密集操作，应该尽量减少不必要的重建。

---

## 7. Sparse Vector Index（稀疏向量索引）

### 7.1 Dense Vector 与 Sparse Vector 的区别

Dense Vector：

```
[0.12, 0.54, 0.78, 0.31]
```

稠密向量通常具有固定维度，包含大量实际数值。

Sparse Vector：

```
indices = [3, 107, 892]
values = [0.8, 0.3, 0.9]
```

稀疏向量只需要记录具有非零权重的维度及其数值。

它适用于 BM25、SPLADE 等检索表示。

需要注意：BM25、SPLADE 是不同的表示和评分方法，并不是所有 Sparse Vector 都等价于传统的关键词匹配。

### 7.2 Sparse Index 如何工作？

Sparse Vector Index 使用针对大量零值的向量优化的数据结构。

其工作方式与传统搜索引擎中的倒排索引有相似之处。

例如：

```
维度 3   → Point 1、Point 4
维度 107 → Point 1、Point 8
维度 892 → Point 1、Point 5
```

查询时可以根据 Query 中实际存在的维度寻找候选 Point，避免检查大量无关维度。

### 7.3 Sparse Vector Index 的主要特征

官方文档指出：

- Sparse Vector Index 是精确索引，不使用 ANN 近似搜索。
    
- 新增稀疏向量可以立即进入可修改索引。
    
- Optimizer 可以将其优化为更紧凑的不可变索引。
    
- 不需要预先指定固定向量维度。
    
- 稀疏向量相似度检索仅支持 Dot Product。
    
- 支持 IDF Modifier，用于结合文档频率信息调整查询权重。
    

### 7.4 创建 Sparse Vector Index

```
client.create_collection(
    collection_name="notes_sparse",
    vectors_config={},
    sparse_vectors_config={
        "text": models.SparseVectorParams(
            index=models.SparseIndexParams()
        )
    },
)
```

这个 Collection 的 `text` 是命名稀疏向量字段，不是 Payload 中的普通 `text` 字段。

插入稀疏向量：

```
client.upsert(
    collection_name="notes_sparse",
    points=[
        models.PointStruct(
            id=1,
            vector={
                "text": models.SparseVector(
                    indices=[3, 107, 892],
                    values=[0.8, 0.3, 0.9],
                )
            },
        )
    ],
)
```

这里的索引维度和权重仅用于演示。实际项目需要由 BM25、SPLADE 或其他稀疏模型生成相应向量表示。

---

## 8. Qdrant 支持的检索能力

经过前面的学习，可以将常见检索能力归纳如下：

|检索方式|依赖的能力|示例|
|---|---|---|
|精确字段匹配|Payload Keyword Index|`category = AI`|
|范围过滤|Integer / Float / Datetime Index|`score >= 80`|
|全文检索|Full-Text Index|查找包含 FastAPI 的文档|
|BM25 排序|Full-Text Index + Scoring|按词项相关性排序|
|Dense Vector 搜索|HNSW|语义相似度搜索|
|Sparse Vector 搜索|Sparse Index|BM25 / SPLADE 检索|
|带过滤的向量检索|Payload Index + Vector Index|某用户知识库内的语义搜索|
|Hybrid Search|多路检索 + Fusion|Dense + Sparse 结果融合|

### 8.1 Hybrid Search

常见 RAG 混合检索过程：

```
Query
  ├── Dense Embedding
  │     └── Dense Vector Search
  │
  └── Sparse Representation
        └── Sparse Vector Search

           ↓

      Result Fusion
        RRF / DBSF

           ↓

     Top-K Candidates

           ↓

    Optional Reranker
```

其中：

- Dense Search 侧重语义相似性。
    
- Sparse Search 侧重稀疏表示中的词项或特征匹配。
    
- Fusion 合并不同检索通路的结果。
    
- Reranker 可以进一步优化候选结果的顺序。
    

**Hybrid Search 不是一种独立的索引结构，而是一套多路检索与排序策略。**

Qdrant 的 Query API 支持通过 Prefetch 和 Fusion 构建这种查询。

---

## 9. 实际项目：NoteAgent 的索引设计

假设 NoteAgent 将文本分片后写入 Qdrant：

```
point = {
    "id": 1,
    "vector": {
        "dense": [0.1, 0.2, 0.3]
    },
    "payload": {
        "user_id": "user_001",
        "document_id": "doc_001",
        "category": "AI",
        "content": "Agent 上下文管理的核心问题...",
        "created_at": "2026-11-22T10:00:00Z"
    }
}
```

推荐初期采用：

|数据|索引方案|
|---|---|
|Dense Vector|HNSW|
|`user_id`|Keyword Index（多用户时）|
|`document_id`|Keyword Index|
|`category`|按需 Keyword Index|
|`created_at`|按需 Datetime Index|
|`content`|全文搜索需要时创建 Text Index|
|Sparse Vector|需要混合检索时引入|

### 9.1 为什么不一次性启用所有索引？

索引并不是越多越好。

每个索引都可能增加：

- 写入时的维护开销。
    
- 内存消耗。
    
- 磁盘空间。
    
- 后台优化工作量。
    
- 索引构建时间。
    

因此应该围绕真实的过滤场景、检索质量和查询性能选择索引。

### 9.2 建议的学习与实验顺序

**实验一：Payload Index**

建立 1 万条具有 `category`、`document_id` 字段的 Point。

分别测试有索引、无索引时的过滤查询。

观察返回数量、查询耗时和内存占用。

**实验二：HNSW**

准备相同的向量数据，比较精确向量扫描与 HNSW 近似搜索的：

- Recall@K
    
- 查询延迟
    
- 内存占用
    

**实验三：Filterable HNSW**

使用不同选择性的过滤条件：

```
category = AI
document_id = doc_001
```

观察过滤条件对查询耗时及召回率的影响。

**实验四：Hybrid Search**

使用相同的语料库和测试问题，比较：

- Dense Only
    
- Sparse Only
    
- Dense + Sparse Fusion
    
- Dense + Sparse + Rerank
    

通过 Recall@K、MRR、NDCG 等指标评估检索效果。

---

## 10. 总结：Qdrant Indexing 的核心设计思想

Qdrant 的索引设计主要解决三个问题。

### 10.1 如何高效找到数据？

根据数据类型选择合适索引：

- Payload Index：字段过滤。
    
- Full-Text Index：全文词项检索与评分。
    
- HNSW：稠密向量相似度检索。
    
- Sparse Vector Index：稀疏向量相似度检索。
    

### 10.2 如何结合不同查询条件？

核心是 Query Planning 和 Filterable HNSW。

Qdrant 可以根据过滤基数、索引状态和相关配置选择检索策略。

对于带过滤条件的向量搜索，利用 Payload Index 提供的相关信息与 HNSW 图的额外边，改善检索效率和召回效果。

### 10.3 如何平衡索引收益与维护成本？

索引需要消耗额外的：

- 内存
    
- 磁盘
    
- CPU
    
- 构建及维护时间
    

Segment 和 Optimizer 用于组织数据、构建索引、合并和重组存储结构。

索引的选择与配置应根据具体业务和负载确定，而不是为所有字段和数据类型无差别启用索引。

**最终认识：**

Qdrant 并不是简单地保存 Embedding 向量，然后执行相似度计算。

它实际上是一个结合向量索引、结构化字段索引、文本索引、查询规划与后台存储优化的检索系统。

掌握 Indexing 的重点不是记住所有创建索引的 API，而是能够解释：

1. 为什么某种数据需要这种索引？
    
2. 该索引如何减少搜索范围或计算成本？
    
3. 带过滤条件时，如何协调 Payload 和 Vector Index？
    
4. 何时全量扫描反而更加高效？
    
5. 新增、删除和修改 Point 时，Segment 与 Optimizer 如何维护索引？
    
6. 如何通过实验判断索引设计是否真的改善了系统性能？
    

只有理解这些问题，才能进一步完成实际 RAG 系统的检索架构设计与性能优化。