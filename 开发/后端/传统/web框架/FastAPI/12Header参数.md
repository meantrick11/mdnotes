# Header 参数

使用Header()类获取请求头中的其他参数。


## 变量名对应请求头名

当变量名对应请求头中的名时，可以直接通过Header()获取对应的数据：
```Python
from typing import Annotated

from fastapi import FastAPI, Header

app = FastAPI()


@app.get("/items/")
async def read_items(user_agent: Annotated[str | None, Header()] = None):
    return {"User-Agent": user_agent}
```

这里Fastapi直接获取请求头中的User-Agent参数，Fastapi会将请求头中的大写全转为小写，以及-转为_进行匹配。

## 使用Header(alias="?")进行匹配

当执行函数中的变量名和请求体中的变量名不一样时，可以使用Header()中的alias参数进行获取：

```python
from typing import Annotated

from fastapi import FastAPI, Header

app = FastAPI()


@app.get("/items/")
async def read_items(user_agent: Annotated[str | None, Header()] = None):
    return {"User-Agent": user_agent}from typing import Annotated

from fastapi import FastAPI, Header

app = FastAPI()


@app.get("/items/")
async def read_items(auth: Annotated[str | None, Header(alias="Authorization")] = None):
    return {"User-Agent": user_agent}
```

## 自动转换

Header 比 Path、Query 和 Cookie 提供了更多功能。

大部分标准请求头用连字符分隔，即减号（-）。

但是 user-agent 这样的变量在 Python 中是无效的。

因此，默认情况下，Header 把参数名中的字符由下划线（_）改为连字符（-）来提取并存档请求头 。

同时，HTTP 的请求头不区分大小写，可以使用 Python 标准样式（即 snake_case）进行声明。

因此，可以像在 Python 代码中一样使用 user_agent ，无需把首字母大写为 User_Agent 等形式。
