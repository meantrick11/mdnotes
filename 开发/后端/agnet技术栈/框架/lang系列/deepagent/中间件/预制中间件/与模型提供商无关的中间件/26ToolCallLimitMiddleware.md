# ToolCallLimitMiddleware

通过限制工具调用的数量（无论是所有工具还是特定工具）来控制agent的执行，主要适用于一下场景：

- 避免对成本过高的外部API进行过度调用
- 限制网络搜索+数据库查询
- 对特定工具的使用实施速率限制
- 防止agent失控循环

## 参数配置

tool_name string

需要限制的特定工具的名称。如果未提供，则**适用于全局所有工具**

thread_limit num

单个线程中的最大工具调用次数限制，相同线程ID的在多次调用中保持不变。需要使用checkpoint进行维护线程状态，**跨语句对话需要记录总次数**


run_limit num

单词调用(message级别)的最大工具调用次数。每次用户新的message都会重置，None表示无运行限制。

exit_behavior string default:"continue"

'continue'</b>（默认）- 阻止工具调用并附带错误信息，让其他工具和模型继续执行。模型根据错误信息决定何时结束。

'error' - 引发ToolCallLimitExceededError异常，立即停止执行

'end' - 立即停止执行，并附带ToolMessage和关于超出工具调用次数的AI消息。仅在限制单个工具时生效；若其他工具存在待处理调用，则抛出NotImplementedError。

## 示例

**注意：如果多个工具，不能同时制定，需要分别构建中间件，然后传入**

```python
from langchain.agents import create_agent
from langchain.agents.middleware import ToolCallLimitMiddleware


global_limiter = ToolCallLimitMiddleware(thread_limit=20, run_limit=10)
search_limiter = ToolCallLimitMiddleware(tool_name="search", thread_limit=5, run_limit=3)
database_limiter = ToolCallLimitMiddleware(tool_name="query_database", thread_limit=10)
strict_limiter = ToolCallLimitMiddleware(tool_name="scrape_webpage", run_limit=2, exit_behavior="error")

agent = create_agent(
    model="gpt-5.5",
    tools=[search_tool, database_tool, scraper_tool],
    middleware=[global_limiter, search_limiter, database_limiter, strict_limiter],
)
```