# 请求体-更新数据


## 使用PUT更新替换数据

更新数据可以使用HTTP PUT操作。

如果需要进行python类型到JSON的转换，那么需要使用jsonable_encoder()进行数据间的转换。

```python
from fastapi import FastAPI
from  fastapi.encoders  import  jsonable_encoder 
from pydantic import BaseModel

app = FastAPI()


class  Item(BaseModel): 
    name: str | None = None
    description: str | None = None
    price: float | None = None
    tax: float = 10.5
    tags: list[str] = []


items = {
    "foo": {"name": "Foo", "price": 50.2},
    "bar": {"name": "Bar", "description": "The bartenders", "price": 62, "tax": 20.2},
    "baz": {"name": "Baz", "description": None, "price": 50.2, "tax": 10.5, "tags": []},
}


@app.get("/items/{item_id}", response_model=Item)
async def read_item(item_id: str):
    return items[item_id]


@app.put("/items/{item_id}", response_model=Item)
async def update_item(item_id: str, item: Item):
    update_item_encoded = jsonable_encoder(item)
    items[item_id] = update_item_encoded
    return update_item_encoded
```

上述示例中使用PUT()方法更新了items实例中的参数。

### 关于替换的警告

如果新的替换数据项缺失原来的数据项，数据项之间会进行继承下的更新。
比如，用 PUT 把数据项 bar 更新为以下请求体时：

```json
{
    "name": "Barz",
    "price": 3,
    "description": None,
}
```
因为其中未包含已存储的属性 "tax": 20.2，输入模型会取 "tax": 10.5 的默认值。

因此，保存的数据会带有这个“新的” tax 值 10.5。


## 使用PATCH进行部分更新

也可以使用 HTTP PATCH 操作对数据进行部分更新。

也就是说，你只需发送想要更新的数据，其余数据保持不变。

>PATCH 没有 PUT 知名，也没那么常用。

>很多团队甚至只用 PUT 实现部分更新。

>你可以随意选择如何使用它们，FastAPI 不做任何限制。

>但本指南会大致展示它们的预期用法。