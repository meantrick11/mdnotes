# contextEditingMiddleware

当token限制时，通过清除较旧的工具输出来管理对话上下文，同时保留醉经的结果。这有助于大量工具调用的长对话同时把持上下文窗口的可控性，上下文编辑适用于以下场景：

- 长对话中包括大量工具并超出token限制
-  通过删除不相关的旧工具输出减少token成本
- 通过上下文仅保留最新的N个工具结果

## 参数配置

edits:list[ContextEdit] = [ClearToolUsesEdit()]

应用的ContextEdit的策略列表

token_count_method:string = "approximate"

token的计数方法："approximate"/"model"

trigger:numver=1000000
出发编辑的token数，当对话超过该token数的时候，较旧的工具输出将会被清除

clear_at_least:number=0
执行编辑操作时至少收回的token数量，如果设置为0，则按需尽可能多地回收

keep:number=3
必须保留地最近工具结果数量。这些结果永远不回被清除

clear_tool_inputs:boolean=False
是否再AI消息中清除发起地工具调用参数，当True时，工具调用参数将会被替换为空对象。

exclude_tools:list[string]=[]
需要排除地清理操作地工具名称列表，这些工具输出永远不会被清除

placeholder:string = "cleared"

为已清除地工具输出插入展位文本。此文本会替代原始工具消息内容

## 示例

```python
from langchain.agents import create_agent
from langchain.agents.middleware import ContextEditingMiddleware, ClearToolUsesEdit


agent = create_agent(
    model="gpt-5.5",
    tools=[search_tool, your_calculator_tool, database_tool],
    middleware=[
        ContextEditingMiddleware(
            edits=[
                ClearToolUsesEdit(
                    trigger=2000,
                    keep=3,
                    clear_tool_inputs=False,
                    exclude_tools=[],
                    placeholder="[cleared]",
                ),
            ],
        ),
    ],
)
```
