# ModelRetryMiddleware
**隶属@wrap_model_call**
通过可配置的指数退避机制自动重试失败的模型调用，模型重试适用于以下场景：
- 处理模型API调用的暂时性故障
- 提升依赖网络的模型请求的可靠性
- 构建能够处理临时错误的高可用agent

简单示例：
```python
from langchain.agents import create_agent
from langchain.agents.middleware import ModelRetryMiddleware

agent = create_agent(
    model="gpt-5.5",
    tools=[search_tool, database_tool],
    middleware=[
        ModelRetryMiddleware(
            max_retries=3,
            backoff_factor=2.0,
            initial_delay=1.0,
        ),
    ],
)
```

## 配置选项

**参数和ToolRetryMiddleware几乎没什么变化**

max_retries
number default:"2"
初次调用后的最大重试次数，默认总共尝试3次

retry_on
tuple[type[Exception], ...] | callable default:"default_retry_on"
重试的异常类型元组，或判断是否重试的函数；langchain>=1.3.16默认重试可重试模型错误、未分类异常，不重试不可重试模型错误

on_failure
string | callable default:"continue"
重试耗尽后的行为：
'continue'：返回带错误的AIMessage，让agent优雅处理
'error'：重新抛出异常，停止agent
自定义函数：接收异常，返回AIMessage内容

backoff_factor
number default:"2.0"
指数退避乘数，每次等待时间 = initial_delay * (backoff_factor ** 重试次数)，设为0.0为固定延迟

initial_delay
number default:"1.0"
第一次重试前的初始延迟（秒）

max_delay
number default:"60.0"
重试间最大延迟（秒），限制指数退避增长

jitter
boolean default:"true"
是否添加±25%随机抖动，避免惊群效应


## 示例

```python
from langchain.agents import create_agent
from langchain.agents.middleware import ModelRetryMiddleware


# Basic usage with default settings (2 retries, exponential backoff)
agent = create_agent(
    model="gpt-5.5",
    tools=[search_tool],
    middleware=[ModelRetryMiddleware()],
)

# Custom exception filtering
class TimeoutError(Exception):
    """Custom exception for timeout errors."""
    pass

class ConnectionError(Exception):
    """Custom exception for connection errors."""
    pass

# Retry specific exceptions only
retry = ModelRetryMiddleware(
    max_retries=4,
    retry_on=(TimeoutError, ConnectionError),
    backoff_factor=1.5,
)


def should_retry(error: Exception) -> bool:
    # Only retry on rate limit errors
    if isinstance(error, TimeoutError):
        return True
    # Or check for specific HTTP status codes
    if hasattr(error, "status_code"):
        return error.status_code in (429, 503)
    return False

retry_with_filter = ModelRetryMiddleware(
    max_retries=3,
    retry_on=should_retry,
)

# Return error message instead of raising
retry_continue = ModelRetryMiddleware(
    max_retries=4,
    on_failure="continue",  # Return AIMessage with error instead of raising
)

# Custom error message formatting
def format_error(error: Exception) -> str:
    return f"Model call failed: {error}. Please try again later."

retry_with_formatter = ModelRetryMiddleware(
    max_retries=4,
    on_failure=format_error,
)

# Constant backoff (no exponential growth)
constant_backoff = ModelRetryMiddleware(
    max_retries=5,
    backoff_factor=0.0,  # No exponential growth
    initial_delay=2.0,  # Always wait 2 seconds
)

# Raise exception on failure
strict_retry = ModelRetryMiddleware(
    max_retries=2,
    on_failure="error",  # Re-raise exception instead of returning message
)
```

**重点：这个返回的字符串不是交给别的程序 / 别的模型，是塞进对话历史，期望 LLM 读到这条错误。但前提是 LLM 后续能正常调用。所以照道理说会出现模型一直尝试重试的情况如果不回退或者其他处理逻辑的话**