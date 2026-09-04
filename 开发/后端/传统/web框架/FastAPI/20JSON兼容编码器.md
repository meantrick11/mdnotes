# JSON兼容编码器
在某些情况下，您可能需要将数据类型（如Pydantic模型）转换为与JSON兼容的数据类型（如dict、list等）。

比如，如果您需要将其存储在数据库中。

对于这种要求， FastAPI提供了jsonable_encoder()函数。


**jsonable_encoder()能够将任何Python数据->转换成直接被json.dumps()序列化的兼容类型**
## 使用jsonable_encoder

对于只接收json数据格式，那么你需要使用jsonable_encoder()将Pydantic模型转为JSON的兼容版本

```python
from datetime import datetime

from fastapi import FastAPI
from fastapi.encoders import jsonable_encoder
from pydantic import BaseModel

fake_db = {}


class Item(BaseModel):
    title: str
    timestamp: datetime
    description: str | None = None


app = FastAPI()


@app.put("/items/{id}")
def update_item(id: str, item: Item):
    json_compatible_item_data = jsonable_encoder(item)
    fake_db[id] = json_compatible_item_data
```


在这个例子中，它将Pydantic模型转换为dict，并将datetime转换为str。

调用它的结果后就可以使用Python标准编码中的json.dumps()。

这个操作不会返回一个包含JSON格式（作为字符串）数据的庞大的str。它将返回一个Python标准数据结构（例如dict），其值和子值都与JSON兼容。

