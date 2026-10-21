# streaming

深度智能体基于 LangGraph 的流式基础设施构建，对智能体子流提供一流支持。当深度智能体将工作委托给子智能体时，你可以独立地流式传输每个子智能体的更新——实时跟踪进度、大语言模型令牌以及工具调用情况。

1. 流式传输subagent的进度
2. streaming模型的token
3. 工具的流式输出
4. 自定义流式更新(从subagent节点内部发出用户定义的信号)

**开启 subgraphs=True 后：updates 事件会同时包含主图节点和子图（子代理）节点的更新。每个事件通过 chunk["ns"] 来区分来源。**

## 启用子图流式传输

```python
from deepagents import create_deep_agent

agent = create_deep_agent(
    model="openai:gpt-5.5",
    system_prompt="You are a helpful research assistant",
    subagents=[
        {
            "name": "researcher",
            "description": "Researches a topic in depth",
            "system_prompt": "You are a thorough researcher.",
        },
    ],
)

for chunk in agent.stream(
    {"messages": [{"role": "user", "content": "Research quantum computing advances"}]},
    stream_mode="updates",
    subgraphs=True,
    version="v2",
):
    if chunk["type"] == "updates":
        if chunk["ns"]:
            # Subagent event - namespace identifies the source
            print(f"[subagent: {chunk['ns']}]")
        else:
            # Main agent event
            print("[main agent]")
        print(chunk["data"])

```

### 命名空间
普通 LangGraph 流式输出只关注主图的事件；Deep Agents 在 subgraphs=True 时，会把子代理的执行过程也暴露出来，每个事件都带有一个 ns（命名空间）字段：

命名空间	来源
() 空元组	主代理（main agent）
("tools:abc123",)	主代理通过 task 工具调用生成的子代理
("tools:abc123", "model_request:def456")	子代理内部的模型请求节点
因此，Deep Agents 流式输出的不同点可以总结为：

1. 子代理生命周期可见：可以实时看到子代理何时启动、运行、完成。

2. LLM token 可区分来源：主代理和每个子代理的 token 可以分开消费。

3. 工具调用可追踪：子代理内部的工具调用和结果也能流式输出。

4. 支持自定义事件：子代理工具内可通过 get_stream_writer() 发出自定义进度。

此外，推荐使用 v2 流式格式（version="v2"），每个 chunk 都是统一的 StreamPart 字典（包含 type、ns、data），不需要再解嵌套元组。

## Subagent 进度
使用 stream_mode="updates" 跟踪每个步骤完成时子代理的进度。

```python
from deepagents import create_deep_agent

agent = create_deep_agent(
    model="openai:gpt-5.5",
    system_prompt=(
        "You are a project coordinator with no research knowledge. "
        "For every user request, you must call the task() tool with "
        "subagent_type set to researcher. Never answer research questions yourself. "
        "Keep your final response to one sentence."
    ),
    subagents=[
        {
            "name": "researcher",
            "description": "Researches topics thoroughly",
            "system_prompt": (
                "You are a thorough researcher. Research the given topic "
                "and provide a concise summary in 2-3 sentences."
            ),
        },
    ],
)

for chunk in agent.stream(
    {"messages": [{"role": "user", "content": "Write a short summary about AI safety"}]},
    stream_mode="updates",
    subgraphs=True,
    version="v2",
):
    if chunk["type"] == "updates":
        # Main agent updates (empty namespace)
        if not chunk["ns"]:
            for node_name, data in chunk["data"].items():
                if node_name == "tools":
                    # Subagent results returned to main agent
                    for msg in data.get("messages", []):
                        if msg.type == "tool":
                            print(f"\nSubagent complete: {msg.name}")
                            print(f"  Result: {str(msg.content)[:200]}...")
                else:
                    print(f"[main agent] step: {node_name}")

        # Subagent updates (non-empty namespace)
        else:
            for node_name, data in chunk["data"].items():
                print(f"  [{chunk['ns'][0]}] step: {node_name}")
```
## LLM tokens

使用 stream_mode="messages" 来流式传输来自主智能体和子智能体的单个token。
```python
current_source = ""

for chunk in agent.stream(
    {"messages": [{"role": "user", "content": "Research quantum computing advances"}]},
    stream_mode="messages",
    subgraphs=True,
    version="v2",
):
    if chunk["type"] == "messages":
        token, metadata = chunk["data"]

        # Check if this event came from a subagent (namespace contains "tools:")
        is_subagent = any(s.startswith("tools:") for s in chunk["ns"])

        if is_subagent:
            # Token from a subagent
            subagent_ns = next(s for s in chunk["ns"] if s.startswith("tools:"))
            if subagent_ns != current_source:
                print(f"\n\n--- [subagent: {subagent_ns}] ---")
                current_source = subagent_ns
            if token.content:
                print(token.content, end="", flush=True)
        else:
            # Token from the main agent
            if "main" != current_source:
                print("\n\n--- [main agent] ---")
                current_source = "main"
            if token.content:
                print(token.content, end="", flush=True)

print()

```

## 工具调用

当子智能体使用工具时，你可以流式传输工具调用事件，以显示每个子智能体正在执行的操作。工具调用块会以messages流模式显示。

```python
from langchain.messages import AIMessageChunk, ToolMessage

for chunk in agent.stream(
    {"messages": [{"role": "user", "content": "Research recent quantum computing advances"}]},
    stream_mode="messages",
    subgraphs=True,
    version="v2",
):
    if chunk["type"] == "messages":
        token, metadata = chunk["data"]

        # Identify source: "main" or the subagent namespace segment
        is_subagent = any(s.startswith("tools:") for s in chunk["ns"])
        source = next((s for s in chunk["ns"] if s.startswith("tools:")), "main") if is_subagent else "main"

        # Tool call chunks (streaming tool invocations)
        if isinstance(token, AIMessageChunk) and token.tool_call_chunks:
            for tc in token.tool_call_chunks:
                if tc.get("name"):
                    print(f"\n[{source}] Tool call: {tc['name']}")
                # Args stream in chunks - write them incrementally
                if tc.get("args"):
                    print(tc["args"], end="", flush=True)

        # Tool results
        if isinstance(token, ToolMessage):
            print(f"\n[{source}] Tool result [{token.name}]: {str(token.content)[:150]}")

        # Regular AI content (skip tool call messages)
        if (
            isinstance(token, AIMessageChunk)
            and token.content
            and not token.tool_call_chunks
        ):
            print(token.content, end="", flush=True)

print()
```

## 自定义updates

在你的子代理工具中使用get_stream_writer来发出自定义进度事件：

```python
import time
from langchain.tools import tool
from langgraph.config import get_stream_writer
from deepagents import create_deep_agent


@tool
def analyze_data(topic: str) -> str:
    """Run a data analysis on a given topic.

    This tool performs the actual analysis and emits progress updates.
    You MUST call this tool for any analysis request.
    """
    writer = get_stream_writer()

    writer({"status": "starting", "topic": topic, "progress": 0})
    time.sleep(0.5)

    writer({"status": "analyzing", "progress": 50})
    time.sleep(0.5)

    writer({"status": "complete", "progress": 100})
    return (
        f'Analysis of "{topic}": Customer sentiment is 85% positive, '
        "driven by product quality and support response times."
    )


agent = create_deep_agent(
    model="openai:gpt-5.5",
    system_prompt=(
        "You are a coordinator. For any analysis request, you MUST delegate "
        "to the analyst subagent using the task tool. Never try to answer directly. "
        "After receiving the result, summarize it in one sentence."
    ),
    subagents=[
        {
            "name": "analyst",
            "description": "Performs data analysis with real-time progress tracking",
            "system_prompt": (
                "You are a data analyst. You MUST call the analyze_data tool "
                "for every analysis request. Do not use any other tools. "
                "After the analysis completes, report the result."
            ),
            "tools": [analyze_data],
        },
    ],
)

custom_event_count = 0
for chunk in agent.stream(
    {"messages": [{"role": "user", "content": "Analyze customer satisfaction trends"}]},
    stream_mode="custom",
    subgraphs=True,
    version="v2",
):
    if chunk["type"] == "custom":
        custom_event_count += 1
        is_subagent = any(s.startswith("tools:") for s in chunk["ns"])
        if is_subagent:
            subagent_ns = next(s for s in chunk["ns"] if s.startswith("tools:"))
            print(f"[{subagent_ns}]", chunk["data"])
        else:
            print("[main]", chunk["data"])
```



