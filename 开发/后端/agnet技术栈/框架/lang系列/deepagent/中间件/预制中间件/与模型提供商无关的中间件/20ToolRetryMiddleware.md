# ToolRetryMiddleware

**隶属@wrap_tool_call**
当工具调用失败的时候可配置指数退避尝试，该中间件适用以下场景：

- 处理外部api调用出现的故障问题
- 提高依赖网络的工具的可靠性(**尤其是可能因为网络波动等等引起的工具调用失败的问题**)
- 构建能够高可用的agent(只能够处理临时出现的错误)

## 参数配置


| 参数 | 说明 |
| ---- | ---- |
| max_retries | 数值，默认值："2"<br>初次调用之后的最大重试次数（默认情况下总共会尝试3次调用） |
| tools | list[BaseTool \| str]<br>可选，需要应用重试逻辑的工具/工具名称列表。为 None 时，对全部工具生效。 |
| retry_on | tuple[type[Exception], ...] \| callable，默认值："default_retry_on"<br>可以是一组需要触发重试的异常类型元组，或是一个可调用函数：接收异常对象，返回布尔值，True 代表需要重试。<br>LangChain >=1.3.16版本：默认策略会对可重试的模型错误、所有未归类异常执行重试；不再重试标记为不可重试的模型错误。 |
| on_failure | string \| callable，默认值："continue"<br>全部重试耗尽后的处理行为，可选值：<br>- 'continue'（默认）：返回携带错误详情的ToolMessage，交由大模型处理本次失败<br>- 'error'：重新抛出异常，终止Agent执行<br>- 自定义可调用函数：入参为异常对象，返回字符串作为ToolMessage的内容<br>已废弃值：'return_message'（改用continue）、'raise'（改用error） |
| backoff_factor | number，默认值："2.0"<br>指数退避倍率。每次重试等待时长 = 初始延迟 × (退避因子 ** 重试序号)，单位秒。设为0.0代表固定延迟。 |
| initial_delay | number，默认值："1.0"<br>第一次重试前的初始等待延迟（单位：秒） |
| max_delay | number，默认值："60.0"<br>两次重试之间的最大延迟时长（限制指数退避的持续增长） |
| jitter | boolean，默认值："true"<br>是否给延迟时间增加随机抖动（±25%），用来避免惊群效应 |


## 示例

```python
from langchain.agents import create_agent
from langchain.agents.middleware import ToolRetryMiddleware


agent = create_agent(
    model="gpt-5.5",
    tools=[search_tool, database_tool, api_tool],
    middleware=[
        ToolRetryMiddleware(
            max_retries=3,
            backoff_factor=2.0,
            initial_delay=1.0,
            max_delay=60.0,
            jitter=True,
            tools=["api_tool"],
            retry_on=(ConnectionError, TimeoutError),
            on_failure="continue",
        ),
    ],
)
```