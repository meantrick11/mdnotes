# Points

**点（Point）**是Qdrant操作的核心实体。一个点是由向量(Vector)+有效载荷(Payload)组成的记录（Record）。不一定只有一个Vector&Payload。

举一个简单的例子：
```python
// This is a simple point
{
    "id": 129,
    "vector": [0.1, 0.2, 0.3, 0.4],
    "payload": {"color": "red"},
}
```

可以基于向量相似度在一个集合中分组的点里进行搜索。

本节主要介绍如何创建&管理Point。

**任何点修改操作均为异步执行，分为两个步骤进行。**在第一阶段，该操作会被写入预写日志（Write-ahead-log）。

## Point IDs

Qdrant 支持同时使用**64-bit unsigned integers和UUID**作为点的标识符。

UUID字符串示例：
1. 简单格式：936DA01F9ABD4d9d80C702AF85C822A8
2. 带连字符的：550e8400-e29b-41d4-a716-446655440000
3. urn: urn:uuid:F9168C5E-CEB2-4faa-B6BF-329BF39FA1E4

这意味着在每次请求中，都可以使用 UUID 字符串替代数字 ID。示例：

```python
from qdrant_client import QdrantClient, models

client = QdrantClient(url="http://localhost:6333")

client.upsert(
    collection_name="{collection_name}",
    points=[
        models.PointStruct(
            id="5c56c793-69f3-4fbf-87e6-c4bf54c28c26",
            payload={
                "color": "red",
            },
            vector=[0.9, 0.1, 0.1],
        ),
    ],
)
```

## Vectors

Qdrant 中的每个点可以包含一个或多个向量。向量是 Qdrant 架构的核心组成部分。Qdrant 依靠不同类型的向量来实现各类数据探索和搜索功能。

支持的向量列表：
1. 稠密向量(Dense Vectors)：由大多数嵌入模型生成的常规向量。
2. 稀疏向量(Sparse Vectors)：没有固定的长度，但只有少数非零元素的向量。适用于精度标记匹配&协同过滤推荐。
3. 多向量：长度固定，但高度可变的数字矩阵（很少）

可以将多种类型的向量关联到同一个点。在 Qdrant 中，我们将这些向量称为**命名向量**。

## Upload Points

为优化性能，Qdrant 支持批量加载点数据。这意味着你可以在一次 API 调用中向服务加载多个点。批量处理可最大限度减少创建网络连接的开销。

Qdrant API 支持两种创建批量数据的方式：面向记录的方式和面向列的方式。在内部，这两种选项是等效的，提供它们仅为了方便使用。

批量创建点：
```python
client.upsert(
    collection_name="{collection_name}",
    points=models.Batch(
        ids=[1, 2, 3],
        payloads=[
            {"color": "red"},
            {"color": "green"},
            {"color": "blue"},
        ],
        vectors=[
            [0.9, 0.1, 0.1],
            [0.1, 0.9, 0.1],
            [0.1, 0.1, 0.9],
        ],
    ),
)   #ids&payloads&vectors一一对应
```

面向记录的等效形式(每一个点单独写，保持多个插入记录)：

```python
client.upsert(
    collection_name="{collection_name}",
    points=[
        models.PointStruct(
            id=1,
            payload={
                "color": "red",
            },
            vector=[0.9, 0.1, 0.1],
        ),
        models.PointStruct(
            id=2,
            payload={
                "color": "green",
            },
            vector=[0.1, 0.9, 0.1],
        ),
        models.PointStruct(
            id=3,
            payload={
                "color": "blue",
            },
            vector=[0.1, 0.1, 0.9],
        ),
    ],
)
```

## Python客户端额外优化

Python 客户端在加载点方面有额外功能，包括：
1. 并行化
2. 重试机制
3. 惰性批处理支持

批列存储形式：
```python
client.upload_collection(
    collection_name="{collection_name}",
    ids=[1, 2],
    payload=[
        {"color": "red"},
        {"color": "green"},
    ],
    vectors=[
        [0.9, 0.1, 0.1],
        [0.1, 0.9, 0.1],
    ],
    parallel=4,
    max_retries=3,
)
```

面向记录的形式：

```python
client.upload_points(
    collection_name="{collection_name}",
    points=[
        models.PointStruct(
            id=1,
            payload={
                "color": "red",
            },
            vector=[0.9, 0.1, 0.1],
        ),
        models.PointStruct(
            id=2,
            payload={
                "color": "green",
            },
            vector=[0.1, 0.9, 0.1],
        ),
    ],
    parallel=4,
    max_retries=3,
)
```

## 幂等性

Qdrant 中的所有 API（包括点加载）都是幂等的。这意味着连续多次执行同一方法等同于执行一次。
这种情况下，这意味着重新上传时，具有相同 ID 的点将被覆盖。
幂等性属性在你使用不提供“仅一次”保证的消息队列等工具时非常有用。即便使用这类系统，Qdrant 也能确保数据的一致性。

## 更新模式

默认情况下，upsert 操作会在点不存在时插入它，或在点已存在时更新它。要更改此行为，请使用 update_mode 参数：

1. upsert（默认）：若点不存在则插入，若已存在则更新。
2. insert_only：仅当点不存在时才插入该点。若存在具有相同 ID 的点，则忽略该操作。
3. update_only：仅在点已存在时更新该点。不存在的点不会被插入。

insert_only 模式：
```python
client.upsert(
    collection_name="{collection_name}",
    points=[
        models.PointStruct(
            id=1,
            vector=[0.9, 0.1, 0.1],
            payload={
                "color": "red",
            },
        ),
        models.PointStruct(
            id=2,
            vector=[0.1, 0.9, 0.1],
            payload={
                "color": "green",
            },
        ),
        models.PointStruct(
            id=3,
            vector=[0.1, 0.1, 0.9],
            payload={
                "color": "blue",
            },
        ),
    ],
    update_mode=models.UpdateMode.INSERT_ONLY
)
```

insert_only模式在**从一个嵌入模型迁移到另一个嵌入模型时特别有用**，此时需要解决常规更新与后台重新嵌入任务之间的冲突。

update_only模式适用于条件更新场景。由于对于不存在的数据点，upsert 操作默认执行插入操作，因此如果条件更新未显式指定update_mode，即便条件不满足，系统也会插入一个新数据点，这在大多数情况下并非预期行为。

## 命名向量

如果该集合是通过多个向量创建的，则可通过向量名称提供每个向量的数据(**也就是一个Point由多个向量组成，通过对向量命名对向量进行区分**)：
```python
client.upsert(
    collection_name="{collection_name}",
    points=[
        models.PointStruct(
            id=1,
            vector={
                "image": [0.9, 0.1, 0.1, 0.2],
                "text": [0.4, 0.7, 0.1, 0.8, 0.1, 0.1, 0.9, 0.2],
            },
        ),
        models.PointStruct(
            id=2,
            vector={
                "image": [0.2, 0.1, 0.3, 0.9],
                "text": [0.5, 0.2, 0.7, 0.4, 0.7, 0.2, 0.3, 0.9],
            },
        ),
    ],
)
```
命名向量为可选项。上传点时，部分向量可省略。例如，你可以上传一个仅包含image向量的点，以及另一个仅包含text向量的点。
上传具有现有 ID 的点时，会先删除该现有点，然后仅使用指定的向量插入新点。也就是说，整个点会被替换，任何未指定的向量都会被设为 null。

## 嵌入稀疏向量

**点可以包含稠密向量和稀疏向量。**

稀疏向量是一种大多数元素值均为零的数组。

可以利用这一特性实现优化表示，因此它们与密集向量的形态不同。

它们以(index, value)对的列表形式表示，其中index为整数，value为浮点数。index是向量中非零值的位置，values为非零元素的取值。

它们以(index, value)对的列表形式表示，其中index为整数，value为浮点数。index是向量中非零值的位置，values为非零元素的取值。
例如，以下向量：

[0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 2.0, 0.0, 0.0]

可以表示为：
[(6, 1.0), (7, 2.0)]

indices和values数组的长度必须相同，且indices必须唯一。**但是这个就可能会失去原有的向量的长度等信息**

如果indices未排序，Qdrant 会在内部对其进行排序，因此你不能依赖元素的顺序。

**稀疏向量必须命名，且上传方式与密集向量相同。**

```python
client.upsert(
    collection_name="{collection_name}",
    points=[
        models.PointStruct(
            id=1,
            vector={
                "text": models.SparseVector(
                    indices=[6, 7],
                    values=[1.0, 2.0],
                )
            },
        ),
        models.PointStruct(
            id=2,
            vector={
                "text": models.SparseVector(
                    indices=[1, 2, 3, 4, 5],
                    values=[0.1, 0.2, 0.3, 0.4, 0.5],
                )
            },
        ),
    ],
)
```

## 推理嵌入

Qdrant 无需显式提供向量，也可通过名为推理的过程生成向量。推理是指利用机器学习模型从文本、图像或其他数据类型中生成向量嵌入的过程。

比如：
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

## 修改点

要修改一个点，你可以调整其向量或有效载荷。有多种方法可以实现这一点。

### 更新向量

此方法会更新给定点上的指定向量。未指定的向量保持不变。所有给定的点必须存在。

**这里也可以使用上述讲的插入update模式进行插入，同样也可以实现更新的目的，不过是更新整个点的向量**

```python
client.update_vectors(
    collection_name="{collection_name}",
    points=[
        models.PointVectors(
            id=1,
            vector={
                "image": [0.1, 0.2, 0.3, 0.4],
            },
        ),
        models.PointVectors(
            id=2,
            vector={
                "text": [0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2],
            },
        ),
    ],
)
```

### 删除向量

此方法仅从给定的点中删除指定的向量。其他向量保持不变。点永远不会被删除。

```python
client.delete_vectors(
    collection_name="{collection_name}",
    points=[0, 3, 100],
    vectors=["text", "image"],
)
#通过指定ids&命名向量名称
```

### 更新负载(payload)

参考Payload章节

## 删除点

```python
client.delete(
    collection_name="{collection_name}",
    points_selector=models.PointIdsList(
        points=[0, 3, 100],
    ),
)
```

过滤后删除:
```python
client.delete(
    collection_name="{collection_name}",
    points_selector=models.FilterSelector(
        filter=models.Filter(
            must=[
                models.FieldCondition(
                    key="color",
                    match=models.MatchValue(value="red"),
                ),
            ],
        )
    ),
)
```

## 条件跟新

所有的更新操作均支持**基于过滤器**的可配置前置条件。

```python
from qdrant_client import QdrantClient, models

client = QdrantClient(url="http://localhost:6333")

client.upsert(
    collection_name="{collection_name}",
    points=[
        models.PointStruct(
            id=1,
            vector=[0.05, 0.61, 0.76, 0.74],
            payload={
                "city": "Berlin",
                "price": 1.99,
                "version": 3,
            },
        ),
    ],
    update_filter=models.Filter(
        must=[
            models.FieldCondition(
                key="version",
                match=models.MatchValue(value=2),
            ),
        ],
    ),
)
```

虽然条件化负载修改和删除覆盖了批量数据修改的场景，但条件化点插入和向量更新对于在分布式系统中实现乐观并发控制尤为有用。

## 检索点（查）

获取点相关的信息

```python
client.retrieve(
    collection_name="{collection_name}",
    ids=[0, 3, 100],
)
```

该方法包含 with_vectors 和 with_payload 两个额外参数。通过这些参数，你可以选择点云的部分内容作为结果。排除不必要的字段可以减少传输的数据量。

## 滚动点（遍历/筛选匹配）

有时可能需要在不知道 ID 的情况下获取所有存储的点，或者遍历与某个筛选条件匹配的点。

```python
client.scroll(
    collection_name="{collection_name}",
    scroll_filter=models.Filter(
        must=[
            models.FieldCondition(key="color", match=models.MatchValue(value="red")),
        ]
    ),
    limit=1,
    with_payload=True,
    with_vectors=False,
)
```

返回所有color为red的点。

### 按照payload对检索的点排序

使用scroll API 时，你可以按负载键对结果进行排序。例如，如果你的负载包含"timestamp"字段，你可以按时间顺序检索积分，如下例所示：
```python
client.scroll(
    collection_name="{collection_name}",
    limit=15,
    order_by="timestamp", # <-- this!
)
```

使用 order_by 参数时，分页功能(pagination)将被禁用。

## 统计点数

有时，无需执行实际搜索就能知道有多少个点符合筛选条件，这会很有用。

除此之外，例如，我们可以重点说明以下场景：
- 评估分页搜索的结果大小
- 确定分页的页面数量
- 调试时查询执行速度


```python
client.count(
    collection_name="{collection_name}",
    count_filter=models.Filter(
        must=[
            models.FieldCondition(key="color", match=models.MatchValue(value="red")),
        ]
    ),
    exact=True,
)
```

返回符合给定筛选条件的点数：
```python
{
  "count": 3811
}
```

## 批量更新

你可以批量执行多个点更新操作，包括插入、更新和删除点、向量以及有效载荷。

下面给出了具体的常见更新操作：


```python
client.batch_update_points(
    collection_name="{collection_name}",
    update_operations=[
        models.UpsertOperation(
            upsert=models.PointsList(
                points=[
                    models.PointStruct(
                        id=1,
                        vector=[1.0, 2.0, 3.0, 4.0],
                        payload={},
                    ),
                ]
            )
        ),
        models.UpdateVectorsOperation(
            update_vectors=models.UpdateVectors(
                points=[
                    models.PointVectors(
                        id=1,
                        vector=[1.0, 2.0, 3.0, 4.0],
                    )
                ]
            )
        ),
        models.DeleteVectorsOperation(
            delete_vectors=models.DeleteVectors(points=[1], vector=[""])
        ),
        models.OverwritePayloadOperation(
            overwrite_payload=models.SetPayload(
                payload={"test_payload": 1},
                points=[1],
            )
        ),
        models.SetPayloadOperation(
            set_payload=models.SetPayload(
                payload={
                    "test_payload_2": 2,
                    "test_payload_3": 3,
                },
                points=[1],
            )
        ),
        models.DeletePayloadOperation(
            delete_payload=models.DeletePayload(keys=["test_payload_2"], points=[1])
        ),
        models.ClearPayloadOperation(clear_payload=models.PointIdsList(points=[1])),
        models.DeleteOperation(delete=models.PointIdsList(points=[1])),
    ],
)
```

## 等待结果

如果调用 API 时传入了&wait=false参数，或者未显式指定该参数，客户端将收到数据接收确认(**默认，并非操作完成后的确认**)：


**Qdrant已经接收请求的响应**
```json
{
  "result": {
    "operation_id": 123,
    "status": "acknowledged"
  },
  "status": "ok",
  "time": 0.000206061
}
```
此响应并不代表数据已可检索。这采用了最终一致性的形式。**由于集合的更新在后台进行，实际完成处理可能需要一小段时间。**事实上，此类请求最终也可能失败。若需插入大量向量，我们也建议使用异步请求，以利用流水线机制。

如果你的应用程序逻辑要求保证向量在 API 响应后立即可供搜索，请使用标志 ?wait=true。在这种情况下，API 将在操作完成后才返回结果：

```python
{
  "result": {
    "operation_id": 0,
    "status": "completed"
  },
  "status": "ok",
  "time": 0.000206061
}
```