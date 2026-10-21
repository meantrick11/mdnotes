# HumanIntheLoop()

这里需要强调的是HumantIntheLoop中间件是动态中断**tool节点内部的调用interrupt**，而langgraph的compile中断则是静态的固定的节点级别中断，每次运行到email_agent节点前都必须中断，然而对于中间件的上层封装是通过动态的节点内部的interrupt进行实现的回路阻断。

适用场景：

- 需要人工审批的高风险操作（例如数据库写入、金融交易）。
- 需要人工监督的合规工作流
- 需要人实时反馈监督agent执行的长任务对话

1. 静态断点：compile(interrupt_before=[...])
```python
graph = builder.compile(
    checkpointer=checkpointer,
    interrupt_before=["email_agent"],
)
```
特点：

- 编译时决定：你在编译图时列出节点名。

- 节点级：只能控制到“哪个节点前/后停”。

- 无条件：只要执行到 email_agent 前，就一定会停。

- 不能根据本次参数跳过：哪怕这次 email_agent 只是调用 read_email，它也会停。

- 恢复：通常发送 Command(resume=...) 或 None，继续执行。

所以静态断点适合：我明确希望整个节点执行前都暂停一下。

2. 动态中断：节点内部调用 interrupt()
```python
def email_agent_node(state):
    tool_call = decide_tool(state)

    if tool_call["name"] == "send_email":
        decision = interrupt({"tool_call": tool_call})  # 运行时主动暂停
        if decision["type"] == "approve":
            execute(tool_call)
    else:
        execute(tool_call)
```
特点：

- 运行时决定：执行到 interrupt() 这一行时才决定是否暂停。

- 代码级/工具级：可以精确到某个工具调用、某条分支、某个参数。

- 有条件：可以用 if 判断，条件不满足就不调用 interrupt()，直接继续。

- 可多次中断：一个节点里可以多次调用 interrupt()。

- 恢复：发送 Command(resume=...)，interrupt() 会返回你传入的恢复值，代码继续往下走。

HumanInTheLoopMiddleware(interrupt_on={"send_email": True}) 本质上就是帮你写了类似这样的动态逻辑：

```python
if tool_call["name"] in interrupt_on:
    if when is None or when(tool_call):
        decision = interrupt(...)
```
它不是靠 compile(interrupt_before=["email_agent"])，而是在 Agent 运行到工具调用时，中间件检查：

- 这次调用的工具是不是 send_email？

- 参数是否满足 when 条件？

- 如果满足，就调用 interrupt() 暂停。

所以：

- 调用 read_email → 不中断。

- 调用 send_email → 中断。

- 甚至可以通过 when 配置成：只有发给外部邮箱才中断，发给内部同事不中断。

3. 核心区别对比

| 维度 | 静态断点 `interrupt_before/after` | 动态中断 `interrupt_on` / `interrupt()` |
| ---- | ---- | ---- |
| 决定时机 | 编译图时 | 运行时 |
| 决定者 | 图配置 | 节点代码/中间件 |
| 粒度 | 节点级 | 工具级、代码级 |
| 是否无条件 | 是，走到就停 | 否，条件满足才停 |
| 能否根据参数跳过 | 不能 | 能 |
| 典型用途 | 整个节点执行前暂停 | 只对敏感工具、敏感参数暂停 |
| 恢复后行为 | 从断点处继续 | 节点从头重放，`interrupt()` 返回恢复值 |

4. 底层关系
它们底层都依赖 LangGraph 的中断恢复机制，都需要 checkpointer。


**后者的动态的最重要的组件就是when来实现动态+按需中断的核心开关**


## 参数配置

interrupt_on dict[str, bool|InterruptOnConfig] 必选参数，工具名称到审批配置的映射，不在此映射内的工具默认自动放行
description_prefix str 可选参数，动作请求描述的前缀，默认值："Tool execution requires approval"

allowed_decisions list[string] 可选参数，允许的人工操作选项，可选值：approve、edit、reject、respond
description string|callable 可选参数，静态描述文本或者返回描述的回调函数
when callable 可选参数，断言函数，接收ToolCallRequest，返回True则中断等待人工审批


## 示例

```python
from langchain.agents import create_agent
from langchain.agents.middleware import HumanInTheLoopMiddleware
from langgraph.checkpoint.memory import InMemorySaver


def your_read_email_tool(email_id: str) -> str:
    """Mock function to read an email by its ID."""
    return f"Email content for ID: {email_id}"

def your_send_email_tool(recipient: str, subject: str, body: str) -> str:
    """Mock function to send an email."""
    return f"Email sent to {recipient} with subject '{subject}'"

agent = create_agent(
    model="gpt-5.5",
    tools=[your_read_email_tool, your_send_email_tool],
    checkpointer=InMemorySaver(),
    middleware=[
        HumanInTheLoopMiddleware(
            interrupt_on={
                "your_send_email_tool": {
                    "allowed_decisions": ["approve", "edit", "reject"],
                },
                "your_read_email_tool": False,
            }
        ),
    ],
)
```