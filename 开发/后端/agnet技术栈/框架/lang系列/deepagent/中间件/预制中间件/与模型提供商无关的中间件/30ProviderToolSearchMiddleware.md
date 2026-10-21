# ProviderToolSearchMiddleware

将选定的工具延迟到模型提供商的服务端工具搜索中执行，这样模型就能按需发现这些工具，而不是预先接收所有工具框架。提供商工具搜索适用于以下场景：
- 使用大量工具时减少上下文膨胀
- 仅展示相关工具以提升工具选择的准确性


## 参数配置

searchable_tools:list[str|BaseTool]

用于延迟提供者工具搜索的工具，按名称、实例提供。只有模型使用搜索之后，才会将这些**延迟工具**显示给模型。
使用extras={"defer_loading": True}构建的工具会被延迟，不受此选项影响；如果省略searchable_tools，则仅延迟那些预先标记的工具。


## 完整实例

1. 可以通过在创建工具的时候就指定为延迟工具
2. 在中间件中列出为延迟工具


```python
from langchain.agents import create_agent
from langchain.agents.middleware import ProviderToolSearchMiddleware
from langchain.tools import tool


# Marked `defer_loading` at construction, so it's deferred on its own —
# no need to list it in `searchable_tools`.
@tool(extras={"defer_loading": True})
def send_email(to: str) -> str:
    """Send an email."""
    return "sent"


agent = create_agent(
    model="anthropic:claude-opus-4-8",
    tools=[send_email],
    middleware=[ProviderToolSearchMiddleware()],
)
```


