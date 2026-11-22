
Qdrant 的一个重要特性是能够同时**存储额外信息以及向量**。在 Qdrant 术语中，这些信息称为 `payload`。
Qdrant 允许你存储任何可以用 JSON 表示的信息。

示例：
```python
{
    "name": "jacket",
    "colors": ["red", "blue"],
    "count": 10,
    "price": 11.99,
    "locations": [
        {
            "lon": 52.5200, 
            "lat": 13.4050
        }
    ],
    "reviews": [
        {
            "user": "alice",
            "score": 4
        },
        {
            "user": "bob",
            "score": 5
        }
    ]
}
```

## Payload Types

除了存储载荷外，Qdrant 还允许基于某些类型的值进行搜索。此功能作为搜索过程中的额外筛选器实现，使您能够在语义相似度的基础上加入自定义逻辑。**JSON，主要是键值对**

在筛选过程中，Qdrant 会检查与过滤条件类型相匹配的数值上的条件。如果存储值类型不符合过滤条件——将视为不满足。

然而，数组（同类型的多个值）的处理方式略有不同。当我们对数组应用滤波器时，如果数组内至少有一个值满足条件，滤波器就会成功。

支持的搜索类型：
### Integer

`integer` - 64位整数，范围为`-9223372036854775808`到`9223372036854775807`。

### Float
`float` - 64位浮点数。

```python
{
    "price": 11.99,
    "ratings": [9.1, 9.2, 9.4]
}
```
### Bool
Bool值
```python
{
    "is_delivered": true,
    "responses": [false, false, true, false]
}
```

### Keyword
字符串值

```python
{
    "name": "Alice",
    "friends": [
        "bob",
        "eva",
        "jack"
    ]
}
```
### Geo
地理坐标（lon表示经度、lat表示纬度）

```python
{
    "location": {
        "lon": 52.5200,
        "lat": 13.4050
    },
    "cities": [
        {
            "lon": 51.5072,
            "lat": 0.1276
        },
        {
            "lon": 40.7128,
            "lat": 74.0060
        }
    ]
}
```
### Datetime
`datetime` - 日期和时间，格式为 RFC 3339
```python
{
    "created_at": "2023-02-08T10:49:00Z",
    "updated_at": [
        "2023-02-08T13:52:00Z",
        "2023-02-21T21:23:00Z"
    ]
}
```

支持以下格式：
- `"2023-02-08T11:49:00+01:00"`（RFC 3339，含时区）
- `"2023-02-08T10:49:00"`（不加时区，假设为UTC）
- `"2023-02-08T10:49"`（不含时区和秒）
- `"2023-02-08"`（仅假设为午夜）
### UUID
除了基本的 `keyword` 类型外，Qdrant 还支持 `uuid` 类型用于存储 UUID 值。功能上，它的工作原理和`keyword`一样，内部存储解析后的UUID值。

```python
{
    "uuid": "550e8400-e29b-41d4-a716-446655440000",
    "uuids": [
        "550e8400-e29b-41d4-a716-446655440000",
        "550e8400-e29b-41d4-a716-446655440001"
    ]
}
```

UUID 的字符串表示（例如 `550e8400-e29b-41d4-a716-446655440000`）占用 36 字节。但如果使用数值表示，则只有128位（16字节）。

## 带有payload的点的创建

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
            },
        ),
        models.PointStruct(
            id=2,
            vector=[0.19, 0.81, 0.75, 0.11],
            payload={
                "city": ["Berlin", "London"],
                "price": 1.99,
            },
        ),
        models.PointStruct(
            id=3,
            vector=[0.36, 0.55, 0.47, 0.94],
            payload={
                "city": ["Berlin", "Moscow"],
                "price": [1.99, 2.99],
            },
        ),
    ],
)
```

## 更新payload
在 Qdrant 中更新有效载荷提供了灵活的方法来管理向量元数据。set payload 方法更新特定字段，同时保持其他字段不变，而 overwrite 方法则替换整个负载。开发者还可以使用清晰的负载移除所有元数据，或删除字段以移除特定键而不影响其余部分。这些选项为适应动态数据集提供了精准控制。

###  设置payload

基于点的设置payload:

```python
client.set_payload(
    collection_name="{collection_name}",
    payload={
        "property1": "string",
        "property2": "string",
    },
    points=[0, 3, 10],
)
```

可以使用Filter进行对点的筛选：
```python
client.set_payload(
    collection_name="{collection_name}",
    payload={
        "property1": "string",
        "property2": "string",
    },
    points=models.Filter(
        must=[
            models.FieldCondition(
                key="color",
                match=models.MatchValue(value="red"),
            ),
        ],
    ),
)
```

### 完全覆盖

用给定的有效载荷完全替换任何现有有效载荷。

```python
client.overwrite_payload(
    collection_name="{collection_name}",
    payload={
        "property1": "string",
        "property2": "string",
    },
    points=[0, 3, 10],
)
```

### 清除payload
此方法会移除指定点上的所有有效载荷密钥
```python
client.clear_payload(
    collection_name="{collection_name}",
    points_selector=[0, 3, 100],
)
```

### 据key删除payload
从点中删除特定key的payload：
```python
client.delete_payload(
    collection_name="{collection_name}",
    keys=["color", "price"],
    points=[0, 3, 100],
)
```

**同样可以使用过滤器选点**


## Payload索引

为了通过筛选更高效地搜索，Qdrant 允许您通过指定负载字段的名称和类型来创建索引。

**索引是collection层面的点的属性，当然每个点可以有自己的payloadkey，如果没有建立统一的索引，Qdrant也可以检索，不过有些缓慢(枚举)，建立所有点层面（Collecitons结构），检索起来更高效，所以有必要为比较重要&统一的字段key创建索引,但这并不要求所有的Point都有该字段（有了跟容易/快速检索到）**

实际上，我们建议在那些可能限制结果最严重的字段上创建索引。例如，为对象 ID 使用索引要高效得多，因为每条记录都是唯一的，而按颜色索引只有少数可能值。

```python
client.create_payload_index(
    collection_name="{collection_name}",
    field_name="name_of_the_field_to_index",
    field_schema=models.PayloadSchemaType.KEYWORD,
)
```

### Facet counts

切面是一种特殊的计数技术，可用于多种用途：
- 了解负载密钥存在哪些唯一值。
- 了解包含每个唯一值的点数。
- 了解匹配特定值时滤波器的限制有多大。
具体来说，它是字段中数值的计数聚合，类似于 SQL 中带有 `COUNT(*)` 命令的 `GROUP BY` 。