向量是Qdrant向量搜索引擎的核心概念，向量决定了对象在向量空间相似度。

为了获取对象的向量表示，你需要对对象应用向量算法。通常，该算法是一个神经网络（针对某一对象训练的特征提取的Embedding模型），将对象转坏为固定大小的矢量。

## Vector Types

现代神经网络可以根据模型/配置不同输出不同形状和大小的向量，Qdrant能支持大多数。

### 稠密向量

这是最常见的载体类型。它是一个简单的数字列表，长度固定，列表中的每个元素都是浮点数。

```
// A piece of a real-world dense vector
[
    -0.013052909,
    0.020387933,
    -0.007869,
    -0.11111383,
    -0.030188112,
    -0.0053388323,
    0.0010654867,
    0.072027855,
    -0.04167721,
    0.014839341,
    -0.032948174,
    -0.062975034,
    -0.024837125,
    ....
]
```
稠密向量的大小可以介于1~65536之间。

大多数神经网络会生成密集向量，所以你可以在 Qdrant 上使用它们，无需额外处理。**大多数向量模型生成的向量都是稠密向量，也就是对象信息被均匀地存储到向量地每一个元素数值当中（即没有很多0），且大多数是浮点数的特征**

### 稀疏向量

稀疏向量是一种特殊类型的向量。在数学上，它们与稠密向量相同，但包含许多零，因此以特殊格式存储。

Qdrant 中的稀疏向量没有固定长度，因为它在向量插入时动态分配。稀疏向量中非零值的数量目前限制在 u32 数据类型范围（4294967295）。

**稀疏向量虽然也能表示对象特征，但是向量本身包含很多的0，只有一部分元素非0，介于此我们只需要两个数组分别存储其位置&数值即可，其他部分默认全为0**，比如
```
// A sparse vector with 4 non-zero elements
{
    "indexes": [1, 3, 5, 7],
    "values": [0.1, 0.2, 0.3, 0.4]
}
```
Qdrant 中的稀疏向量被保存在专用存储中并被单独索引，因此它们的配置不同于稠密向量。

要创建包含稀疏向量的集合：

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
向集合中插入一个向量稀疏的点：
```python
from qdrant_client import QdrantClient, models

client = QdrantClient(url="http://localhost:6333")

client.upsert(
    collection_name="{collection_name}",
    points=[
        models.PointStruct(
            id=1,
            payload={},  # Add any additional payload if necessary
            vector={
                "text": models.SparseVector(
                    indices=[1, 3, 5, 7],
                    values=[0.1, 0.2, 0.3, 0.4]
                )
            },
        )
    ],
)
```

利用稀疏向量搜索：
```python
from qdrant_client import QdrantClient, models

client = QdrantClient(url="http://localhost:6333")

result = client.query_points(
    collection_name="{collection_name}",
    query=models.SparseVector(indices=[1, 3, 5, 7], values=[0.1, 0.2, 0.3, 0.4]),
    using="text",
).points
```

### 多向量(Multivectors)

即在单个Point中存储不同数量的相同形状的向量，也就是针对一个Point的单个字段可以上传多个稠密向量组成的矩阵（二维）。**这里的多向量是指的一个字段，每一个命名向量都是一个点的字段，也就一个字段能够上传相同的大小的多个向量形成的矩阵，而不是命名向量的各个向量需要保持一样维度&大小，各自命名空间可以完全不同。**

矩阵的长度是固定的，但矩阵中每个点的向量数量可以不同。

#### 应用场景
1. 同一个对象的多重表示：将某个图片经过不同的model提取的向量特征也不一样，也就是不同角度的特征提取，那么后续检索，也可以针对不同的特征维度进行检索，扩大匹配率。
2. 迟交互向量模型：比如一些文本嵌入模型将单个文本输出为多个向量，比如ColBERT模型家族将文本中的每个token都输出一个较小的向量。


MaxSim 每点返回一个综合分数，而不是每个子向量。

**多向量就哦于匹配问题，query使用哪种向量？如何使用？**
为了使用多向量，我们需要指定一个函数，用于比较向量矩阵之间。
目前，Qdrant 支持 `max_sim` 函数，定义为矩阵中每对向量的最大相似度之和。

$$
score = \sum_{i=1}^{N} \max_{j=1}^{M} \text{Sim}(\text{vector}A_i, \text{vector}B_j)
$$

其中 𝑁 是第一个矩阵中的向量数， 𝑀 是第二个矩阵中的向量数，且 Sim 是一个相似函数，例如余弦相似度。
要使用多向量，创建一个带有多向量比较器的稠密向量：

```python
from qdrant_client import QdrantClient, models

client = QdrantClient(url="http://localhost:6333")

client.create_collection(
    collection_name="{collection_name}",
    vectors_config=models.VectorParams(
        size=128,
        distance=models.Distance.COSINE,
        multivector_config=models.MultiVectorConfig(
            comparator=models.MultiVectorComparator.MAX_SIM
        ),
    ),
)
```

创建一个带有多向量比较器的稠密向量：
```python
from qdrant_client import QdrantClient, models

client = QdrantClient(url="http://localhost:6333")

client.create_collection(
    collection_name="{collection_name}",
    vectors_config=models.VectorParams(
        size=128,
        distance=models.Distance.COSINE,
        multivector_config=models.MultiVectorConfig(
            comparator=models.MultiVectorComparator.MAX_SIM
        ),
    ),
)
```

插入带有多向量的点的方法如下：
```python
from qdrant_client import QdrantClient, models

client = QdrantClient(url="http://localhost:6333")

client.upsert(
    collection_name="{collection_name}",
    points=[
        models.PointStruct(
            id=1,
            vector=[
                [-0.013,  0.020, -0.007, -0.111],
                [-0.030, -0.055,  0.001,  0.072],
                [-0.041,  0.014, -0.032, -0.062]
            ],
        )
    ],
)
```

使用多向量进行搜索：
```python
from qdrant_client import QdrantClient, models

client = QdrantClient(url="http://localhost:6333")

client.query_points(
    collection_name="{collection_name}",
    query=[
        [-0.013,  0.020, -0.007, -0.111],
        [-0.030, -0.055,  0.001,  0.072],
        [-0.041,  0.014, -0.032, -0.062]
    ],
)
```

## 命名向量

在 Qdrant 中，你可以在同一个数据点中存储多个不同大小和类型的向量。当您需要用多个嵌入定义数据以表示不同特征或模态（例如图像、文本或视频）时，这非常有用。

要为每个点存储不同的向量，你需要在集合中创建独立命名的向量空间。您可以在创建集合时定义这些向量空间，或稍后添加并独立管理它们。
要创建包含命名向量的集合，您需要为每个向量指定配置：

```python
from qdrant_client import QdrantClient, models

client = QdrantClient(url="http://localhost:6333")

client.create_collection(
    collection_name="{collection_name}",
    vectors_config={
        "image": models.VectorParams(size=4, distance=models.Distance.DOT),
        "text": models.VectorParams(size=5, distance=models.Distance.COSINE),
    },
    sparse_vectors_config={"text-sparse": models.SparseVectorParams()},
)
```

插入带有命名向量的点：
```python
client.upsert(
    collection_name="{collection_name}",
    points=[
        models.PointStruct(
            id=1,
            vector={
                "image": [0.9, 0.1, 0.1, 0.2],
                "text": [0.4, 0.7, 0.1, 0.8, 0.1],
                "text-sparse": {
                    "indices": [1, 3, 5, 7],
                    "values": [0.1, 0.2, 0.3, 0.4],
                },
            },
        ),
    ],
)
```
使用命名向量进行搜索（可在 `query` API 中提供）：
```python
from qdrant_client import QdrantClient

client = QdrantClient(url="http://localhost:6333")

client.query_points(
    collection_name="{collection_name}",
    query=[0.2, 0.1, 0.9, 0.7],
    using="image",
    limit=3,
)
```

### 添加&移除命名向量（collection级）

命名向量可以在无需重新创建集合的情况下，添加或移除现有集合。比如：
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

## 推理

Qdrant 在导入或查询数据时不会明确提供向量，而是可以通过一种称为推理的过程生成向量。推断是利用机器学习模型，从文本、图像或其他数据类型创建向量嵌入的过程。

通常是通过自定义模型先完成推理形成向量，然后再嵌入到Qdrant当中，但是也可以直接指定模型&传入对象交给Qdrant做推理+嵌入的全部操作：
```python
from qdrant_client import QdrantClient, models

client.upsert(
    collection_name="{collection_name}",
    points=[
        models.PointStruct(
            id=1,
            vector={
                "my-bm25-vector": models.Document(
                    text="Recipe for baking chocolate chip cookies",
                    model="Qdrant/bm25",
                )
            },
        )
    ],
)
```
```
```
Qdrant 使用模型生成嵌入，并与所得向量一起存储该点。

同样，你也可以在查询时使用推理，提供用于查询的文本或图像以及嵌入模型：
```python
from qdrant_client import QdrantClient, models

client.query_points(
    collection_name="{collection_name}",
    query=models.Document(
        text="How to bake cookies?", 
        model="Qdrant/bm25",
    ),
    using="my-bm25-vector",
)
```

## 数据类型

默认情况下，Qdrant 将每个向量维度存储为 32 位浮点数。这并非总是必要的：你的用例可能不需要这种精度，或者你使用的嵌入模型已经产生了精度较低的向量，比如 8 位整数。

将向量存储为 32 位浮点数是有代价的：内存和存储空间会随着维度的增加线性增长。例如，在 OpenAI `text-embedding-3-large` 模型中，向量可达到 3072 维，累积速度很快。

为了减少向量大小或高效存储已经较低精度的向量，您可以配置每个向量数据类型：Float32（默认）、TurboQuant 数据类型之一（Turbo16、Turbo8 或 Turbo4）、Float16 或 Uint8。

注意，数据类型与量化特征不同。量子化会在原始向量之外，单独为向量创造一个独立的量化表示，而数据类型则决定原始向量本身的表示方式。

### Float32
这是 Qdrant 中向量的默认数据类型。它是一个 32 位（4 字节）浮点数。标准的 OpenAI 嵌入 1536 维度，将需要 6KB 的内存来存储在 `float32` 中。

在 Qdrant 中，你无需为向量明确指定此数据类型，因为它默认设置为 `float32`。

### Float16

`float16` 数据类型将每个维度存储为 16 位（2 字节）浮点数，也称为半精度浮点数。这会将向量存储缩小到原始大小的一半，但每个值的精度略有损失：
```python
float32 -> float16 delta (float32 - float16).abs

0.79701585 -> 0.796875   delta 0.00014084578
0.7850789  -> 0.78515625 delta 0.00007736683
0.7775044  -> 0.77734375 delta 0.00016063452
0.85776305 -> 0.85791016 delta 0.00014710426
0.6616839  -> 0.6616211  delta 0.000062823296
```

**`float16` 的主要优点是它需要 `float32` 一半的内存，同时几乎不影响向量搜索的质量。**

```python
from qdrant_client import QdrantClient, models

client = QdrantClient(url="http://localhost:6333")

client.create_collection(
    collection_name="{collection_name}",
    vectors_config=models.VectorParams(
        size=128,
        distance=models.Distance.COSINE,
        datatype=models.Datatype.FLOAT16
    ),
    sparse_vectors_config={
        "text": models.SparseVectorParams(
            index=models.SparseIndexParams(datatype=models.Datatype.FLOAT16)
        ),
    },
)
```

### Uint8

`uint8` 数据类型将每个维度存储为无符号的 8 位整数（1 字节），其值范围从 0 到 255。这会将向量存储缩小到原始大小的四分之一。该范围的影响取决于你使用的是稠密矢量还是稀疏矢量：
- 密度高的向量需要在0到255之间。并非所有嵌入模型都能生成 0 到 255 范围的向量，因此在使用 `uint8` 数据类型时需要小心。
- 稀疏向量可以在处理过程中进行量子化。
对于稠密向量，`uint8` 仅在它们已经被量子化到该范围时才有用。一些嵌入服务提供商提供这种格式的嵌入，例如 Cohere 的 8 位整数嵌入。要减小正则浮点向量的大小，请改用 TurboQuant 数据类型。

### TurboQuant Datatypes

TurboQuant 数据类型将每个向量以压缩形式存储，每维度使用比 32 位浮点数更少的位数。它们是基于 TurboQuant 构建的。每个向量首先进行数学旋转，将每个维度的信息均匀分布在整个向量中，这样在压缩过程中没有单一维度丢失不成比例的细节。每个旋转后的值随后以更少的位存储。
TurboQuant 的数据类型接受常规浮点向量，并在存储时对其进行压缩。**它们支持密集向量和多向量，但无法用于配置稀疏向量。**

**Qdrant 的 Turbo 数据类型就是用少量的向量精度损失，换取更低的存储成本，以及潜在的检索性能提升。**

**当向量数据库存储了几百万、几千万条向量时，内存和磁盘开销过大的问题。**
#### Turbo16

`turbo16` 数据类型将每个向量以每维度 16 位表示的形式存储，而非 32 位浮点数。这会将向量存储缩小到原始大小的大约一半。

要使用 `turbo16`，在创建集合或向现有集合架构中添加新向量时，请为该向量指定 `turbo16` 数据类型：

```python
from qdrant_client import QdrantClient, models

client.create_collection(
    collection_name="{collection_name}",
    vectors_config=models.VectorParams(
        size=1024,
        distance=models.Distance.COSINE,
        datatype=models.Datatype.TURBO16,
    ),
)
```

与 float16 相比，`turbo16` 的大小几乎相同，但仍然比你原始的 `float32` 向量更接近 7.5 倍。`float16` 对每个值保持相同的相对误差，因此在少数非常大的值占主导分数的嵌入中，它们的四舍五入误差不会相互抵消。这种情况发生在 OpenAI `text-embedding-ada-002`、CLIP 和 Cohere 等模型上。由于每个向量先旋转，其数值分布相似，因此一个16位网格（每个向量只有一个刻度）可以很好地覆盖它们。

#### Turbo8
`turbo8` 数据类型将每个向量以每维度 8 位表示的形式存储，而非 32 位浮点数。这会将向量存储缩小到原有容量的约四分之一。

```python
from qdrant_client import QdrantClient, models

client.create_collection(
    collection_name="{collection_name}",
    vectors_config=models.VectorParams(
        size=1024,
        distance=models.Distance.COSINE,
        datatype=models.Datatype.TURBO8,
    ),
)
```

#### Turbo4
`turbo4` 数据类型将每个向量以每维 4 位的紧凑表示形式存储，而非 32 位浮点数。这会将向量存储缩小到原有大小的约八分之一。每个旋转后的值都存储为16个级别之一（最多4位可表示）。
```python
from qdrant_client import QdrantClient, models

client.create_collection(
    collection_name="{collection_name}",
    vectors_config=models.VectorParams(
        size=1024,
        distance=models.Distance.COSINE,
        datatype=models.Datatype.TURBO4,
    ),
)
```

## 量子化

除了改变原始向量的数据类型外，Qdrant 还可以在原始向量旁边创建量化的向量表示。这种量化表示方式可用于快速选择候选对象，用原始矢量重新计分，甚至直接用于搜索。

量化是在优化过程中的后台应用的。