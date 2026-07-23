# Cookie 参数

使用Cookie()类定义获取请求头中的Cookie，Cookie是特殊的请求头，所以不同于普通的Header，其他的请求头头需要使用Header()来获取。

## 使用Cookie()

```python 
from typing import Annotated

from fastapi import Cookie,FastAPI

app = FastAPI()

@app.get("/item/")
async def read_item(ads_id:Annotated[str|None,Cookie()]=None):
    return {"ads_id":ads_id}

```

以上代码获取请求头中的Cookie赋值给ads_id
