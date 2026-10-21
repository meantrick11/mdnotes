# RubricMiddleware

有些任务对“完成”有明确的定义，而智能体无法在第一次尝试时可靠地达成这一状态。RubricMiddleware 允许你将完成的标准声明为评分规则，并让智能体进行自我评估和迭代，直到满足评分规则或达到最大迭代次数上限。

**这个中间件使用不多，不过多介绍**
## 示例

```python
from deepagents import RubricMiddleware, create_deep_agent
from langgraph.checkpoint.memory import InMemorySaver

agent = create_deep_agent(
    model="openai:gpt-5.5",
    middleware=[
        RubricMiddleware(
            model="anthropic:claude-haiku-4-5",
            max_iterations=3,
        ),
    ],
    checkpointer=InMemorySaver(),
)

```