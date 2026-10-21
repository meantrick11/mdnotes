# 流式传输 JSON Lines

## 流式传输

流式传输”数据意味着你的应用会在整段数据全部准备好之前，就开始把每个数据项发送给客户端。

也就是说，它会先发送第一个数据项，客户端会接收并开始处理它，而此时你的应用可能还在生成下一个数据项。

## JSON Lines

在这些场景中，常见的做法是发送 “JSON Lines”，这是一种每行发送一个 JSON 对象的格式。

响应的内容类型是 application/jsonl（而不是 application/json），响应体类似这样：
```json

{"name": "Plumbus", "description": "A multi-purpose household device."}
{"name": "Portal Gun", "description": "A portal opening device."}
{"name": "Meeseeks Box", "description": "A box that summons a Meeseeks."}
```
它与 JSON 数组（相当于 Python 的 list）非常相似，但不是用 [] 包裹、并在各项之间使用 , 分隔，而是每行一个 JSON 对象，彼此以换行符分隔。

## 使用场景

你可以用它来从 AI LLM 服务、日志或遥测中流式传输数据，或其他可以用 JSON 项目来结构化的数据。

## 使用FastAPI流式传输JSON Lines

```python
from collections.abc import AsyncIterable, Iterable

from fastapi import FastAPI
from pydantic import BaseModel

app = FastAPI()


class Item(BaseModel):
    name: str
    description: str | None


items = [
    Item(name="Plumbus", description="A multi-purpose household device."),
    Item(name="Portal Gun", description="A portal opening device."),
    Item(name="Meeseeks Box", description="A box that summons a Meeseeks."),
]


@app.get("/items/stream")
async def stream_items() -> AsyncIterable[Item]:
    for item in items:
        yield item

# Code below omitted 👇
```

这里的声明的模型类的Item，实例化之后，底层就是使用字典保存的，所以可以直接返回，但是这里没有声明是否是JSON还是JSONLines，最好还是以下代码实现：
```python
from fastapi import FastAPI
from fastapi.responses import StreamingResponse
from pydantic import BaseModel
import json

app = FastAPI()

class Item(BaseModel):
    name: str
    description: str | None

items = [
    Item(name="Plumbus", description="A multi-purpose household device."),
    Item(name="Portal Gun", description="A portal opening device."),
]

@app.get("/items/stream")
async def stream_items():
    # 生成器：每一条 JSON + 换行
    async def generate():
        for item in items:
            # 序列化为 JSON 字符串
            yield json.dumps(item.model_dump()) + "\n"  # ✅ 关键：加换行

    # 返回 StreamingResponse，声明 JSON Lines 格式
    return StreamingResponse(
        generate(),
        media_type="application/x-ndjson"  # ✅ JSON Lines 标准媒体类型
    )
```


FastAPI在没有声明的前提下，会自动将return内的数据转为JSON格式返回前端，如果操作函数使用yield，FastAPI默认返回JSON Lines的格式。

## 非异步的路径操作函数¶
你也可以使用常规的 def 函数（不带 async），并以同样的方式使用 yield。

FastAPI 会确保其正确运行，不会阻塞事件循环。

因为这个函数不是异步的，合适的返回类型是 Iterable[Item]：

```python
# Code above omitted 👆

@app.get("/items/stream-no-async")
def stream_items_no_async() -> Iterable[Item]:
    for item in items:
        yield item

# Code below omitted 👇
```

## 无返回类型
你也可以省略返回类型。此时 FastAPI 会使用 jsonable_encoder 将数据转换为可序列化为 JSON 的形式，然后以 JSON Lines 发送。
```python
# Code above omitted 👆

@app.get("/items/stream-no-annotation")
async def stream_items_no_annotation():
    for item in items:
        yield item

# Code below omitted 👇`
```

