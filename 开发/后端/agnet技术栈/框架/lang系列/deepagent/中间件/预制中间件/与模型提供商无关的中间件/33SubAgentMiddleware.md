# SubAgentMiddleware

可以通过中间件创建子agent来隔离上下文，既保持主agent智能体的上下文窗口整洁，又能深入处理某个单一任务

**通过注入一个task工具的形式，然后主agent传入子agent名+描述**然后调用sub_agent

**就算不设定Deep agent也会自动注入一个general_agent作为子agent，可以覆盖掉**

子智能体通过**名称、描述、系统提示词和工具**来定义。你还可以为子智能体指定**自定义的模型，或添加额外的中间件**。当你希望为子智能体添加一个可与主智能体共享的额外状态键时，这种设置会特别有用。

当然还可以定义与主agent是独立还是关联的定义。

除了任何用户定义的子智能体外，主智能体始终可以访问一个general-purpose子智能体。该子智能体拥有与主智能体相同的指令，以及它可使用的所有工具。general-purpose子智能体的主要用途是实现上下文隔离——主智能体可以将复杂任务委托给该子智能体，然后获得简洁的回复，而不会受到中间工具调用的冗余信息影响。

## 示例

```python
from langchain.tools import tool
from langchain.agents import create_agent
from deepagents.middleware.subagents import SubAgentMiddleware


@tool
def get_weather(city: str) -> str:
    """Get the weather in a city."""
    return f"The weather in {city} is sunny."

agent = create_agent(
    model="claude-sonnet-4-6",
    middleware=[
        SubAgentMiddleware(
            default_model="claude-sonnet-4-6",
            default_tools=[],
            subagents=[
                {
                    "name": "weather",
                    "description": "This subagent can get weather in cities.",
                    "system_prompt": "Use the get_weather tool to get the weather in a city.",
                    "tools": [get_weather],
                    "model": "gpt-5.5",
                    "middleware": [],
                }
            ],
        )
    ],
)
```