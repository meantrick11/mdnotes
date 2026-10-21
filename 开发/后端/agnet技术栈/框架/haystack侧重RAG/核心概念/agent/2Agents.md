# Agents

本页面介绍如何在Haystack中创建一个Ai Agent，该Agent能够利用各类Haystack组件检索信息、生成回复并执行操作。

**深刻记住：所有都是组件，通过pipelien相连，通过数据类传入，DAG**

## 什么是AI Agent？

AI Agent是一种能够实现以下功能的系统：
    - 理解用户输入(text\image\video...)
    - 检索信息(文档或者结构化的数据)
    - 生成智能回复
    - 执行操作（调用API、获取实时数据、执行函数等等）

**这里写一下读者对Agent的理解；**
Agent本意为代理，如果有任何能够帮你做一些事情的都可以叫做代理，比如一个自动化程序，他能每天早起叫你起床，他就可以是一个代理，那么回到如今较火的模型相关的Agent，Anthropic非常明确地阐述了Agent地定义：分为工作流(提示词链、并行、编排、监督者等等模式)+纯LLM驱动的Agent，普通的LLM只能根据输入，吐出token预测，仿佛能够回答问题(表现出一种智能)，比如纯聊天LLM，但是完全做不到帮你做一些事情比如写代码、做ppt等等内容，如果要让它有这个能力，就得给他上下文，比如在它的输入提示词中添加工具的定义："你现在有一个get_weather的工具，入参是首字母大写的英文城市名city，用来获取某一个城市的当前的天气状况。"，当用户输入“查询一下上海的天气”的时候，那么模型根据输入返回
```json
{
  "tool_calls": [
    {
      "type": "function",
      "function": {
        "name": "get_weather",
        "arguments": "{\"city\":\"Shanghai\"}"
      }
    }
  ]
}
```
接着又有其他代码逻辑，检测到get_weather这个函数名，就拿着对应的参数调用具体的函数，并将api调用结果作为新的输入返回给LLM，最后LLM再输出回答"今天上海天气晴，东北风三级...注意防晒"，这就是典型的function call&React Agent范式。


AI智能体是利用大语言模型（LLMs）进行决策并解决复杂任务的自主系统。它们借助工具、记忆和推理能力与所处环境交互。AI智能体并非只是聊天机器人——它会主动规划、选择合适的工具并执行任务以达成目标。与传统软件不同，它能适应新信息，并根据需要优化自身流程。

1. 以大语言模型为大脑：智能体的核心是一个大语言模型，它能够理解上下文、处理自然语言，并作为核心智能系统发挥作用。

2. 交互工具：智能体连接外部工具、API 和数据库，以收集信息并采取行动。

3. 情境记忆：短期记忆有助于追踪对话，而长期记忆则为未来的互动储存知识。

4. 推理与规划：智能体拆解复杂问题，制定分步行动计划，并根据新数据和反馈进行调整。

**AI智能体从定义其角色和目标的提示词开始。它会决定何时使用工具、收集数据，并通过推理和行动的循环来优化自身的处理方式。**例如，客服智能体借助数据库解答咨询——若无法找到答案，它会调取实时数据、进行总结并给出回复。代码辅助助手则能理解项目需求、提供解决方案建议并编写代码。


## Agent组件

Haystack 内置了 智能体 组件，用于管理完整的工具调用循环——它调用大语言模型、调用工具、更新状态，并持续执行直到满足停止条件。核心功能包括：

- 状态管理：在工具之间共享类型化数据，跨迭代累积结果，并使用state_schema将其显示在结果字典中。**类似lang系列中的Agent组件内部State全局共享，不过Agent在haystack中仍然是某一个组件**


- Streaming:流式输出,通过streaming_callback逐令牌输出内容。

- HumanInTheLoop:人工介入,在执行前拦截工具调用以进行人工审核。

- Multi-Agent-Systems:多智能体系统：将Agent包装为AgentTool，以构建协调器/专家架构。

- MCP:MCP 服务器暴露：使用Hayhooks将你的智能体暴露为 MCP 服务器，使其可从任何 MCP 兼容客户端（如 Claude Desktop 或 Cursor）调用。

- Multimodel inputs:在 ImageContent 内容部分中使用 ImageContent 与文本一同传入图像，或从工具返回 ImageContent 以进行动态图像分析。需要支持视觉功能的模型，例如 gpt-5 或 gemini-2.5-flash。

### State

State是Haystack内置的用于在工具之间共享数据并跨多次工具调用累积结果的机制。你需要在Agent上定义一个state_schema，在那里声明的所有键都会与messages和last_message一起返回到智能体的结果字典中。


### Tools

Haystack 提供了多种创建和管理工具的方式：

- Tool 类 / @tool 装饰器 – 从 Python 函数定义一个工具。@tool 装饰器会自动使用函数的名称和文档字符串；Tool 类则可完全控制名称、描述和模式。

- ComponentTool – 将任意 Haystack 组件封装为可调用工具。

- PipelineTool – 将完整的 Haystack 管道包装为可调用工具。

- AgentTool – 将一个 Agent 包装为可调用工具，以便另一个 Agent 可以委托给它。

- MCPTool / MCPToolset – 连接到模型上下文协议（MCP）服务器以加载外部工具。

- Toolset – 将多个工具组合成一个单元，以传递给智能体或生成器。

- SearchableToolset – 支持针对大型工具目录进行基于关键词的工具发现，使大语言模型在每一步仅查看相关工具。

### 示例

使用Agent组件创建一个工具调用智能体。本示例需要将OPENAI_API_KEY和SERPERDEV_API_KEY设置为环境变量：

1. 定义环境变量+安装haystack包
```shell
export OPENAI_API_KEY=<your-openai-key>
export SERPERDEV_API_KEY=<your-serperdev-key>

pip install serperdev-haystack
```

2. 构建完整agent

```python
from haystack.components.agents import Agent
from haystack.components.generators.chat import OpenAIChatGenerator
from haystack.components.generators.utils import print_streaming_chunk
from haystack_integrations.components.websearch.serperdev import SerperDevWebSearch
from haystack.dataclasses import ChatMessage
from haystack.tools import ComponentTool

# Wrap the web search component as a tool
web_tool = ComponentTool(
    component=SerperDevWebSearch(top_k=3),
    name="web_search",
    description="Search the web for current information like weather, news, or facts.",
)

tool_calling_agent = Agent(
    chat_generator=OpenAIChatGenerator(model="gpt-5.4-nano"),
    system_prompt=(
        "You're a helpful agent. When asked about current information like weather, news, or facts, "
        "use the web_search tool to find the information and then summarize the findings."
    ),
    tools=[web_tool],
    streaming_callback=print_streaming_chunk,
)

result = tool_calling_agent.run(
    messages=[ChatMessage.from_user("How is the weather in Berlin?")],
)
print(result["last_message"].text)
```