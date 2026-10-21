# ToolErrorMiddleware

**隶属@wrap_tool_call**
ToolErrorMiddleware这个中间件能够捕捉工具在执行过程中引发的异常，并将其转换为模型可以查看/回复的ToolMessage，而不是直接中断Agent。可以尝试在以下场景中使用ToolMessage：

- 期望模型根据工具反馈而尝试修正参数重试失败的工具调用
- 呈现经过受控/清洗后的错误信息，而非原始的异常详情
- 防止意外的工具异常导致agent崩溃


```python
from langchain.agents import create_agent
from langchain.agents.middleware import ToolErrorMiddleware


def on_error(exc: Exception, request: ToolCallRequest) -> str | None:
    if isinstance(exc, ValueError):
        return f"`{request.tool_call['name']}` failed with {type(exc).__name__}."
    # propagate everything else


agent = create_agent(
    model="gpt-5.5",
    tools=[your_tools],
    middleware=[ToolErrorMiddleware(on_error)],
)

```

## 参数配置

1. on_error
Callable[[Exception, ToolCallRequest], str | list[ContentBlock] | None]

工具执行引发的每个异常都会调用同步处理程序。返回内容（str或内容块列表）以将异常转换为ToolMessage(status="error")。返回None或省略返回语句以让异常继续传播。用于同步路径，且除非指定了aon_error，否则也用于异步路径。

2. aon_error
Callable[[Exception, ToolCallRequest], Awaitable[str | list[ContentBlock] | None]]

可选的异步处理程序，用于异步执行路径。未提供时回退到on_error。

3. tools 
list[BaseTool | str]

要应用错误处理的工具或工具名称的可选列表。如果None，则应用于所有工具。

**三个参数全部是可选参数，只需要至少提供 on_error 或者 aon_error 其中一个就行；tools 是用来限定作用范围的，不填默认作用于全部工具。**

### 使用全示例

```python
from langchain.agents import create_agent
from langchain.agents.middleware import ToolErrorMiddleware, ToolRetryMiddleware


def on_error(exc: Exception, request: ToolCallRequest) -> str | None:
    # Surface ValueError to the model so it can correct the input
    if isinstance(exc, ValueError):
        return f"`{request.tool_call['name']}` failed: {type(exc).__name__}. Fix the input and retry."
    # Let all other exceptions propagate (halts the run)
    return None


# Async-only usage
async def aon_error(exc: Exception, request: ToolCallRequest) -> str | None:
    if isinstance(exc, ConnectionError):
        return f"Tool `{request.tool_call['name']}` encountered a connection error."
    return None


agent = create_agent(
    model="gpt-5.5",
    tools=[search_tool, database_tool],
    middleware=[
        # Place retry inner so exceptions reach ToolErrorMiddleware after retries are exhausted
        ToolErrorMiddleware(on_error=on_error, tools=["search_tool"]),
        ToolRetryMiddleware(max_retries=3, on_failure="error"),
    ],
)

# Async-only: pass aon_error alone (do not pass on_error)
async_agent = create_agent(
    model="gpt-5.5",
    tools=[api_tool],
    middleware=[ToolErrorMiddleware(aon_error=aon_error)],
)
```