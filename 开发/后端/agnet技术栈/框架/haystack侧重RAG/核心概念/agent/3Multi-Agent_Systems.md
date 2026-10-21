# Multi-Agent Systems

多智能体系统允许你将多个Agent实例组合成更大的架构，在该架构中，协调器智能体会向专业智能体分配任务。每个专业智能体凭借自身的工具和系统提示词专注于特定任务——协调器则负责规划和分配工作，无需了解每项任务的具体执行方式。

将智能体作为工具生成适用于以下情况：
- 单个任务范围过广，单个Agent无法可靠处理**单个Agent无法满足需求**
- 你希望将不同的能力隔离为专注且可复用的智能体**专精**
- 为了做出更优决策并减少令牌消耗，你需要保持协调器的上下文简洁。**省上下文窗口**

## 将Agent转为Tool

AgentTool封装了一个专业智能体，以便协调器可以调用它。协调器的模型将需要委派的任务作为单条用户消息发送出去，并以文本形式接收专业智能体的最终回复，因此你无需描述该智能体的接口，也无需解析其结果字典。

```python
from haystack.components.agents import Agent
from haystack.components.generators.chat import OpenAIResponsesChatGenerator
from haystack.components.generators.utils import print_streaming_chunk
from haystack.dataclasses import ChatMessage
from haystack.tools import AgentTool, ComponentTool
from haystack_integrations.components.websearch.serperdev import SerperDevWebSearch
from haystack.utils import Secret


research_agent = Agent(
    chat_generator=OpenAIResponsesChatGenerator(model="gpt-5.4-nano"),
    tools=[
        ComponentTool(
            component=SerperDevWebSearch(
                api_key=Secret.from_env_var("SERPERDEV_API_KEY"),
                top_k=3,
            ),
            name="web_search",
            description="Search the web for current information on any topic",
        ),
    ],
    system_prompt="You are a research specialist. Search the web to find information.",
)


research_specialist = AgentTool(
    agent=research_agent,
    name="research_specialist",
    description="A specialist that researches topics on the web",
)


coordinator = Agent(
    chat_generator=OpenAIResponsesChatGenerator(model="gpt-5.4-nano"),
    tools=[research_specialist],
    system_prompt="You are a coordinator. Delegate research tasks to the research specialist.",
    streaming_callback=print_streaming_chunk,
)

result = coordinator.run(
    messages=[
        ChatMessage.from_user("What are the latest developments in Haystack AI?"),
    ],
)

```

## AgentTool的替代方案

**于Agent是一个 Haystack 组件，你也可以用ComponentTool对其进行封装。**不过这会将智能体完整的组件接口暴露给协调器，包括tools、generation_kwargs和hook_context等参数，且协调器会接收到完整的结果字典。默认情况下，AgentTool仅暴露一个输入项messages，用于承载待委派的任务，同时还会为专业人员的每个必填提示变量添加一个参数。它仅返回专业人员最终回复的文本内容。

**你也可以使用@tool装饰器创建一个类似的窄接口。**当你需要自定义输入参数、想要转换委托任务，或者需要对专家的响应进行后处理时，这种方式非常有用。其代价是序列化问题：被装饰的工具会序列化为函数的导入路径，因此专家的配置会保存在你的 Python 模块中，而非上述的 YAML 文件里。

## Coordinator&Specialist Pattern

Coordinator&Specialist Pattern模式清晰地划分了职责：协调者负责规划和授权，而每位专业人员拥有一套专注的工具集和一个针对性的系统提示词。

**就是编排者模式，可以看一下**[Ahthropic-Engineering-Agent-model](https://www.anthropic.com/engineering/building-effective-agents),里面详细阐述了5中工作流+LLM驱动的Agent，其中从路由->监督者模式，都可以是Multi-Agent,这里的haystack讲的很浅


这也是一种上下文工程的形式：刻意控制每个智能体所能看到的内容。专业智能体会积累自身的工具调用轨迹，而协调器仅需最终答案。AgentTool 只展示专业智能体的最终回复，从而让协调器的上下文保持简洁。

当涉及多个主题时，协调器可在单次回复中多次调用同一个专业工具。一次大语言模型回复产生的所有工具调用都会通过线程池并发执行。可通过tool_concurrency_limit初始化参数控制并发级别（默认值为4）。

*以下代码示例展示了将一个search_agent转为AgentTool当作参数传递给coordinator：*

```python
from typing import Annotated
from haystack.components.agents import Agent
from haystack.components.converters import HTMLToDocument
from haystack.components.fetchers.link_content import LinkContentFetcher
from haystack.components.generators.chat import OpenAIResponsesChatGenerator
from haystack.components.generators.utils import print_streaming_chunk
from haystack_integrations.components.websearch.serperdev import SerperDevWebSearch
from haystack.dataclasses import ChatMessage
from haystack.tools import AgentTool, ComponentTool, tool
from haystack.utils import Secret


search_tool = ComponentTool(
    component=SerperDevWebSearch(
        api_key=Secret.from_env_var("SERPERDEV_API_KEY"),
        top_k=3,
    ),
    name="web_search",
    description="Search the web for current information on any topic",
)


@tool
def fetch_page(url: Annotated[str, "The URL of the web page to fetch"]) -> str:
    """Fetch the content of a web page given its URL."""
    try:
        streams = LinkContentFetcher().run(urls=[url])["streams"]
        if not streams:
            return "No content found."
        documents = HTMLToDocument().run(sources=streams)["documents"]
        return documents[0].content if documents else "No content extracted."
    except Exception as e:
        return f"Failed to fetch page: {e}"


research_agent = Agent(
    chat_generator=OpenAIResponsesChatGenerator(model="gpt-5.4-nano"),
    tools=[search_tool, fetch_page],
    system_prompt=(
        "You are a research specialist. Search the web to find relevant pages, "
        "then fetch their full content for detailed information. "
        "Return a concise summary of your findings in 3-5 sentences."
    ),
)


research_specialist = AgentTool(
    agent=research_agent,
    name="research_specialist",
    description="Research a topic on the web and report a summary of the findings",
)


coordinator = Agent(
    chat_generator=OpenAIResponsesChatGenerator(model="gpt-5.4-nano"),
    tools=[research_specialist],
    system_prompt=(
        "You are a coordinator. Delegate research tasks to the research specialist. "
        "For questions covering multiple topics, research each one independently. "
        "Keep your final answer concise."
    ),
    streaming_callback=print_streaming_chunk,
    tool_concurrency_limit=4,  # run up to 4 specialist calls in parallel
)

result = coordinator.run(
    messages=[
        ChatMessage.from_user(
            "What are the latest developments in the Haystack framework, "
            "and what is the current state of the Model Context Protocol?",
        ),
    ],
)
```