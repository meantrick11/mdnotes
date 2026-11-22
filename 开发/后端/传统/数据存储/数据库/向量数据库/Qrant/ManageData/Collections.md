
#### Collections(集合)

**集合**是一组具有名称的点(Points(vectors with a payload)),可以在其中进行搜索。同一集合内每个点的向量必须具有相同的维度，并通过单一指标进行比较。**命名向量（Named Vectors）**可用于在单个点中包含多个向量，每个向量可拥有自身的维度和指标要求。


**举例指标**是用于衡量向量之间的相似度，指标的选择取决于向量的获取方式，尤其神经网络编码器的训练方法。

Qdrant支持以下几种长江的指标类型：
- Dot product(点积)
- Cosine similarity(Cosine余弦相似度)
- Euclidean Distance(欧拉距离)
- Manhattan Distance(曼哈顿距离)

第一种&第二种很容易相互转换。

##### 设置Multitenancy


**应该创建多少个集合？**大多数情况下，仅仅需要一个基于有效payload的集合即可(**通过payload进行隔离)**，这种方法称为**multitenancy**，。他对大多数用户都很高效，但是需要额外配置。
**什么时候应该创建多个collection？**当你的用户数量有限且需要隔离时。这种方法具有灵活性，但成本可能更高，因为创建大量集合可能会导致资源开销。此外，你需要确保它们在任何方面都不会相互影响，包括性能方面。**完全隔离性，但是会浪费大量资源，如果没有必要的前提下**

##### 集合的创建

```python
from qdrant_client import QdrantClient, models

client = QdrantClient(url="http://localhost:6333")

client.create_collection(
    collection_name="{collection_name}",
    vectors_config=models.VectorParams(size=100, distance=models.Distance.COSINE),
)
```
除了基础的name、vectors_config,还有hnsw_config、wal_config、optimizers_config、shard_number、payload.memory、quantization_config、strick_mode_config


Qdrant 始终将向量存储在磁盘上。你可以为每个向量配置一个内存层，以控制有多少数据也驻留在内存中。

###### 单记录多向量

**同一个文本符合多个向量查询需求。**

每条记录可以拥有多个向量。此特性支持每个集合存储多个向量。为区分同一条记录中的向量，它们需具有唯一的名称。此模式下每个命名向量都有对应的距离和大小：

```python
from qdrant_client import QdrantClient, models

client = QdrantClient(url="http://localhost:6333")

client.create_collection(
    collection_name="{collection_name}",
    vectors_config={
        "image": models.VectorParams(size=4, distance=models.Distance.DOT),
        "text": models.VectorParams(size=8, distance=models.Distance.COSINE),
    },
)
```

##### 向量的数据类型


默认情况下，Qdrant 将每个向量维度存储为 32 位浮点数。内存和存储空间会随维度数量线性增长，因此对于大向量，成本会迅速累积。为降低该成本，或存储本身精度较低的向量，你可以配置不同的数据类型：float16（半精度）、uint8（无符号 8 位整数），或以下 TurboQuant 数据类型之一：turbo4（4 位）、turbo8（8 位）或 turbo16（16 位）。

```python
from qdrant_client import QdrantClient, models

client = QdrantClient(url="http://localhost:6333")

client.create_collection(
    collection_name="{collection_name}",
    vectors_config=models.VectorParams(
        size=1024,
        distance=models.Distance.COSINE,
        datatype=models.Datatype.UINT8,
    ),
    indexing_threshold=10000    #当集合的大小达到10000kb的时候，开始构建HNSW索引，<=时直接向量遍历。
)
```

示例中的向量的数据类型就是uint8.

##### 稀疏向量集合
Qdrant 将稀疏向量作为一等公民支持。

在单个数据点中，集合可将稀疏向量作为额外的命名向量与常规稠密向量一同包含。

**与密集向量不同，稀疏向量必须命名。**此外，在一个集合内，稀疏向量和密集向量必须具有不同的名称。

```python
from qdrant_client import QdrantClient, models

client = QdrantClient(url="http://localhost:6333")

client.create_collection(
    collection_name="{collection_name}",
    vectors_config={},
    sparse_vectors_config={
        "text": models.SparseVectorParams(),
    },
)
```

除了唯一名称外，稀疏向量没有必需的配置参数。

稀疏向量的距离函数始终为Dot，无需指定。


##### 从一个向量创建另一个向量

要从另一个集合创建集合，请使用 Migration 工具。你可以使用它在同一个 Qdrant 实例内复制集合，也可以将集合复制到另一个实例中。**应用很少**

##### exists&delete

上述讲了如何创建collections，还有检查是否存在&删除操作：
```python
client.collection_exists(collection_name="{collection_name}")   #检查集合是否存在
client.delete_collection(collection_name="{collection_name}")   #删除集合
```

##### 更新集合

动态参数更新可能会有所帮助，例如能更高效地实现向量的初始加载。比如，你可以在上传过程中禁用索引，然后在上传完成后立即启用它。这样一来，就不会在重建索引上浪费额外的计算资源。

以下命令可对存储向量超过 10000 千字节的段启用索引：
```python
client.update_collection(
    collection_name="{collection_name}",
    optimizers_config=models.OptimizersConfigDiff(indexing_threshold=10000),
)
```

##### 更新schema

**一个文档支持两种/多种不同的向量查询**

可以**在现有集合中添加或移除命名向量，而无需重新创建集合**。这对于嵌入模型迁移非常实用：你可以为新模型添加一个新向量，在后台重新嵌入数据点，然后在准备就绪时移除旧向量。

**这些是schema操作，用于在集合的模式中添加或删除向量定义。要从特定点添加/删除向量值，请使用 更新 和 删除 向量操作。**

也就是一个改变一个point的结构，原本一个point仅仅存储一个向量，改变之后可能存储多个向量，通过不同的名称隔离，这个schema可以理解为关系型数据库的列/字段，改变整张collection/表的结构。

新增一个新命名的稠密向量：
```python
client.create_vector_name(
    collection_name="{collection_name}",
    vector_name="{vector_name}",
    vector_name_config=models.DenseVectorNameConfig(
        dense=models.DenseVectorConfig(
            size=256,
            distance=models.Distance.COSINE,
        ),
    ),
)
```

要向现有集合中添加一个新的稀疏命名向量：
```python
client.create_vector_name(
    collection_name="{collection_name}",
    vector_name="{vector_name}",
    vector_name_config=models.SparseVectorNameConfig(
        sparse=models.SparseVectorConfig(
            modifier=models.Modifier.IDF,
        ),
    ),
)
```


删除命名向量：
```python
client.delete_vector_name(
    collection_name="{collection_name}",
    vector_name="{vector_name}",
)
```
**删除命名向量会移除其架构及所有关联数据。现有数据点不受其他影响。**已有内容的point就算改变结构也不会影响已有的数据，比如类似增加字段之后已有的部分全为Null

##### 集合信息

Qdrant 允许确定现有集合的配置参数，以便更好地了解点的分布和索引方式。

获取现有集合的相关信息：
`client.get_collection(collection_name="{collection_name}")`

将向量插入集合后，status字段在优化期间可能会变为yellow。所有点均成功处理后，它将变为green。
🟢 green：集合已就绪
🟡 yellow：集合正在优化
⚫ grey：集合待优化（帮助）
🔴 red：发生了引擎无法恢复的错误

***灰色状态**：集合可能会显示灰色⚫状态，或在优化状态下显示“优化待处理，等待更新操作”。**该状态通常是在优化操作进行中重启Qdrant实例导致的**。
```python
client.update_collection(
    collection_name="{collection_name}",
    optimizer_config=models.OptimizersConfigDiff(),
)
```

##### HNSW&计数

对于集合的一些索引上层配置/集合的points的数量、索引数量等等的api接口。


##### 集合元数据

为了方便使用和优化数据组织，Qdrant 允许以键值对的形式为集合附加自定义元数据。添加元数据被视为集合配置的一部分，并通过共识协议在集群的所有节点间同步。

**集合级别元数据**可以在集合创建时指定：

```python
from qdrant_client import QdrantClient, models

client = QdrantClient(url="http://localhost:6333")

client.create_collection(
    collection_name="{collection_name}",
    metadata={
        "my-metadata-field": "value-1",
        "another-field": 123
    },
)
```

后续更新：

```python
client.update_collection(
    collection_name="{collection_name}",
    metadata={
        "my-metadata-field": {
            "key-a": "value-a",
            "key-b": 42
        }
    },
)
```
仅仅针对于元数据部分，其余部分保持不变，

##### 集合别名

**在生产环境中，有时需要无缝切换不同版本的向量。**例如，在升级到神经网络的新版本时。


在这些情况下，无法停止服务并使用新向量重建集合。别名是现有集合的额外名称。对集合的所有查询也可以完全相同地使用别名而非集合名称来完成。


1. 创建别名：
```python
client.update_collection_aliases(
    change_aliases_operations=[
        models.CreateAliasOperation(
            create_alias=models.CreateAlias(
                collection_name="example_collection", alias_name="production_collection"
            )
        )
    ]
)
```

2. 移除别名

```python
client.update_collection_aliases(
    change_aliases_operations=[
        models.DeleteAliasOperation(
            delete_alias=models.DeleteAlias(alias_name="production_collection")
        ),
    ]
)
```

##### 切换集合

多个别名操作是原子执行的。例如，你可以使用以下命令切换底层集合：

```python
client.update_collection_aliases(
    change_aliases_operations=[
        models.DeleteAliasOperation(
            delete_alias=models.DeleteAlias(alias_name="production_collection")
        ),
        models.CreateAliasOperation(
            create_alias=models.CreateAlias(
                collection_name="example_collection", alias_name="production_collection"
            )
        ),
    ]
)
```

##### 列出集合别名

```python
from qdrant_client import QdrantClient

client = QdrantClient(url="http://localhost:6333")

client.get_collection_aliases(collection_name="{collection_name}")
```

##### 列出所有别名

```python
from qdrant_client import QdrantClient

client = QdrantClient(url="http://localhost:6333")

client.get_aliases()
```

##### 列出所有集合

```pytnon
from qdrant_client import QdrantClient

client = QdrantClient(url="http://localhost:6333")

client.get_collections()
```