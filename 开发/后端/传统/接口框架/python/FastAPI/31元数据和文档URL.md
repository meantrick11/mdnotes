# 元数据和文档URL

## API元数据


| 参数 | 类型 | 描述 |
| ---- | ---- | ---- |
| `title` | `str` | API 的标题。 |
| `summary` | `str` | API 的简短摘要。自 OpenAPI 3.1.0、FastAPI 0.99.0 起可用。 |
| `description` | `str` | API 的简短描述。可以使用 Markdown。 |
| `version` | `str` | API 的版本。这是你自己的应用程序的版本，而不是 OpenAPI 的版本。例如 `2.5.0`。 |
| `terms_of_service` | `str` | API 服务条款的 URL。如果提供，则必须是 URL。 |
| `contact` | `dict` | 公开的 API 的联系信息。它可以包含多个字段。 |
| `license_info` | `dict` | 公开的 API 的许可证信息。它可以包含多个字段。 |


```python
from fastapi import FastAPI

description = """
ChimichangApp API helps you do awesome stuff. 🚀

## Items

You can **read items**.

## Users

You will be able to:

* **Create users** (_not implemented_).
* **Read users** (_not implemented_).
"""

app = FastAPI(
    title="ChimichangApp",
    description=description,
    summary="Deadpool's favorite app. Nuff said.",
    version="0.0.1",
    terms_of_service="http://example.com/terms/",
    contact={
        "name": "Deadpoolio the Amazing",
        "url": "http://x-force.example.com/contact/",
        "email": "dp@x-force.example.com",
    },
    license_info={
        "name": "Apache 2.0",
        "url": "https://www.apache.org/licenses/LICENSE-2.0.html",
    },
)


@app.get("/items/")
async def read_items():
    return [{"name": "Katana"}]
```

## 许可证标识符(license_info参数的子字段)

自 OpenAPI 3.1.0 和 FastAPI 0.99.0 起，你还可以在 license_info 中使用 identifier 而不是 url。

例如：
```python
from fastapi import FastAPI

description = """
ChimichangApp API helps you do awesome stuff. 🚀

## Items

You can **read items**.

## Users

You will be able to:

* **Create users** (_not implemented_).
* **Read users** (_not implemented_).
"""

app = FastAPI(
    title="ChimichangApp",
    description=description,
    summary="Deadpool's favorite app. Nuff said.",
    version="0.0.1",
    terms_of_service="http://example.com/terms/",
    contact={
        "name": "Deadpoolio the Amazing",
        "url": "http://x-force.example.com/contact/",
        "email": "dp@x-force.example.com",
    },
    license_info={
        "name": "Apache 2.0",
        "identifier": "Apache-2.0",
    },
)


@app.get("/items/")
async def read_items():
    return [{"name": "Katana"}]
```
对比上面两处代码的license_info字段不一样。也就是使用identifier来代替url字段。

## 标签元数据(openapi_tags参数)

```python
from fastapi import FastAPI

### 定义app的应用标签细类
tags_metadata = [
    {
        "name": "users",
        "description": "Operations with users. The **login** logic is also here.",
    },
    {
        "name": "items",
        "description": "Manage items. So _fancy_ they have their own docs.",
        "externalDocs": {
            "description": "Items external docs",
            "url": "https://fastapi.tiangolo.com/",
        },
    },
]

### 实例化
app = FastAPI(openapi_tags=tags_metadata)

### 标签中字段给每个路由使用
@app.get("/users/", tags=["users"])
async def get_users():
    return [{"name": "Harry"}, {"name": "Ron"}]


@app.get("/items/", tags=["items"])
async def get_items():
    return [{"name": "wand"}, {"name": "flying broom"}]

```
注意你可以在描述内使用 Markdown，例如「login」会显示为粗体（login）以及「fancy」会显示为斜体（fancy）。

## 使用标签

如上图的路由阶段的标签的使用

## OpenAPI URL

默认情况下，OpenAPI 模式服务于 /openapi.json。

但是你可以通过参数 openapi_url 对其进行配置。

例如，将其设置为服务于 /api/v1/openapi.json：

```python
from fastapi import FastAPI

app = FastAPI(openapi_url="/api/v1/openapi.json")


@app.get("/items/")
async def read_items():
    return [{"name": "Foo"}]
```
如果你想完全禁用 OpenAPI 模式，可以将其设置为 openapi_url=None，这样也会禁用使用它的文档用户界面。

## 文档url
你可以配置两个文档用户界面，包括：

Swagger UI：服务于 /docs。
可以使用参数 docs_url 设置它的 URL。
可以通过设置 docs_url=None 禁用它。
ReDoc：服务于 /redoc。
可以使用参数 redoc_url 设置它的 URL。
可以通过设置 redoc_url=None 禁用它。
例如，设置 Swagger UI 服务于 /documentation 并禁用 ReDoc：

```python
from fastapi import FastAPI

app = FastAPI(docs_url="/documentation", redoc_url=None)


@app.get("/items/")
async def read_items():
    return [{"name": "Foo"}]
```