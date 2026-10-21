# ModelCallLimitMiddleware

**这个中间件既需要线程限制，也需要单轮会话限制，所以不是单一的wrap_model_call制造的**

before_model 钩子：在每次模型调用前执行。它负责检查当前的计数是否已达到配置的 thread_limit 或 run_limit，如果超限则根据配置中止执行。

after_model 钩子：在每次模型调用后执行，将 run_model_call_count 和 thread_model_call_count 各加 1。

run_limit 只需在单次运行内有效，所以计数可以保存在内存中。但 thread_limit 需要跨多次 invoke 调用累计，这就要求状态必须被持久化。

限制模型调用次数以防止无限循环或者过高成本。主要使用场景如下：


- 防止失控的agent发起过多的API调用
- 再生产环境中实施成本控制
- 在特定的调用预算内测试agent行为



## 参数设置

thread_limit num

单个线程/会话的最大模型调用次数，默认无限制


run_limit num

单个对话/message级的模型最大调用次数，默认无限制

exit_behavior string default:"end"

达到限制时的行为，默认为"end"


## 示例

```python
from langchain.agents import create_agent
from langchain.agents.middleware import ModelCallLimitMiddleware
from langgraph.checkpoint.memory import InMemorySaver

agent = create_agent(
    model="gpt-5.5",
    checkpointer=InMemorySaver(),  # Required for thread limiting
    tools=[],
    middleware=[
        ModelCallLimitMiddleware(
            thread_limit=10,
            run_limit=5,
            exit_behavior="end",
        ),
    ],
)
```