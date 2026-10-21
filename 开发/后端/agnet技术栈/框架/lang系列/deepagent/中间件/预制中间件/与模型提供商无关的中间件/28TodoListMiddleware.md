# TodoListMiddleware

如果长任务，通常需要采用Plan-execute模式，也就是在完整任务开始之前需要列一个待办任务清单，然后再按照任务执行，执行过程中可以是React模式，但是它的任务已经被计划好了。也就是将一个长任务拆分为多个分步骤完成的短任务。

**有助于复杂的多步骤任务，常规的React模式则是明确的短任务，不需要特别计划**


尤其是解决**复杂多步任务里，如果计划只存在于上下文或思维链中，很容易被长上下文淹没，出现遗忘、重复、提前结束、目标漂移**

1. 注入计划工具
2. 系统提示词追加计划工具调用提示词
3. 自动维护状态

**不过这个计划事件维护都是通过agent自身更新事件清单，而不是

## 参数配置

system_prompt:string

用于指导待办事项使用的自定义系统提示。**如果未指定，则使用内置提示。**

tool_description:string

为write_todos工具自定义的描述。如果未指定，则使用内置描述。


## 示例

```python
from langchain.agents import create_agent
from langchain.agents.middleware import TodoListMiddleware

agent = create_agent(
    model="gpt-5.5",
    tools=[read_file, write_file, run_tests],
    middleware=[TodoListMiddleware()],
)
```





