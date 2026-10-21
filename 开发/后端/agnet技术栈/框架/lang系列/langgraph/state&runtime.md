# LangGraph 的 State 与 Runtime：从代码层面彻底讲透

LangGraph 是 LangChain 团队推出的低级编排框架，用于构建有状态的多角色 Agent 应用。它的核心抽象非常精简：一个图由**节点**和**边**构成，节点通过读写一个**共享状态对象**来通信，而**Runtime** 则负责在节点执行时注入运行作用域内的上下文和工具。本文将把 State 和 Runtime 这两个概念从最小到最大、从表层到源码层彻底拆解，每个部分都附带可运行的代码示例。


## 第一部分：State——图的共享记忆

### 1.1 为什么 State 是核心

LangGraph 的设计哲学是：节点是纯函数，状态是可变的共享数据结构。每个节点接收当前状态作为输入，返回一个**局部更新字典**，而不是返回完整的新状态。运行时负责在每一步结束后把节点的返回值合并回状态中。

这个设计的关键在于：节点不需要知道状态的全貌，只需要声明“我要改什么”。未在返回字典中出现的字段保持不变。


### 1.2 State Schema 的定义方式

LangGraph 支持三种 Schema 定义方式，它们各有适用场景。

**方式一：TypedDict（推荐，也是最常用的）**

TypedDict 是 LangGraph 生态中默认的 State Schema 类型。它性能最优，且与节点“返回部分更新字典”的语义天然契合。

```python
from typing_extensions import TypedDict
from langgraph.graph import StateGraph

class AgentState(TypedDict):
    messages: list[str]
    user_id: str
    iteration: int
    status: str

graph = StateGraph(AgentState)
```

**方式二：Pydantic BaseModel**

当你需要运行时数据校验、类型强制转换或复杂嵌套模型时，使用 Pydantic。代价是每次状态更新都会触发验证，性能略低于 TypedDict。

```python
from pydantic import BaseModel, Field

class AgentState(BaseModel):
    messages: list[str] = Field(default_factory=list)
    user_id: str
    iteration: int = 0

graph = StateGraph(AgentState)
```

**方式三：dataclass**

dataclass 介于两者之间，提供结构化的字段定义但不做运行时校验。

```python
from dataclasses import dataclass, field

@dataclass
class AgentState:
    messages: list[str] = field(default_factory=list)
    user_id: str = ""
    iteration: int = 0
```


### 1.3 Annotated 与 Reducer：状态合并的核心机制

这是 LangGraph State 系统中最关键的设计。当你用 `Annotated[type, reducer]` 标注一个字段时，你是在告诉 LangGraph：**当多个来源向这个字段写入时，不要直接覆盖，而是用 reducer 函数来合并**。

```python
from typing import Annotated
import operator
from typing_extensions import TypedDict

class State(TypedDict):
    # 无 reducer：后写覆盖前写
    status: str

    # operator.add：追加到列表
    messages: Annotated[list[str], operator.add]

    # operator.add：整数累加
    total_cost: Annotated[int, operator.add]

    # 自定义 reducer：字典合并
    metadata: Annotated[dict, lambda old, new: {**old, **new}]
```

**Reducer 的调用约定**：Reducer 接收两个参数 `(current_value, new_value)`，返回合并后的结果。这个约定贯穿所有 reducer，无论是内置的 `operator.add`、LangGraph 提供的 `add_messages`，还是你自定义的函数。

#### 内置 Reducer：add_messages

对于对话类 Agent，LangGraph 提供了 `MessagesState` 和 `add_messages` reducer。`MessagesState` 本质上就是 `{messages: Annotated[list, add_messages]}`。

```python
from langgraph.graph.message import MessagesState, add_messages

# 方式一：直接继承 MessagesState
class ChatState(MessagesState):
    user_id: str
    current_plan: str | None

# 方式二：手动定义
class ChatState(TypedDict):
    messages: Annotated[list, add_messages]
    user_id: str
```

`add_messages` 不只是简单的列表追加。它支持**消息 ID 更新**：如果新消息的 ID 与已有消息相同，它会替换而非追加。还支持 `RemoveMessage` 来删除消息。

```python
from langchain_core.messages import HumanMessage, RemoveMessage

# add_messages 的智能合并行为
existing = [HumanMessage(content="hello", id="msg-1")]
new = [HumanMessage(content="hello (edited)", id="msg-1")]
# 结果：[HumanMessage(content="hello (edited)", id="msg-1")] —— 替换而非追加
```


### 1.4 状态更新的完整语义

理解状态更新需要区分三种情况：

**情况一：字段有 Reducer，单节点更新**

```python
class State(TypedDict):
    count: Annotated[int, operator.add]

def increment(state: State) -> dict:
    return {"count": 1}

# 假设当前 count = 5，执行后 count = 6
```

**情况二：字段无 Reducer，单节点更新**

```python
class State(TypedDict):
    status: str

def set_status(state: State) -> dict:
    return {"status": "done"}

# 无论之前 status 是什么，都变成 "done"
```

**情况三：多个节点在同一步骤更新同一个无 Reducer 字段——会报错**

这是最容易出错的场景。如果两个节点在同一个超级步骤中并行执行，且都更新了同一个没有 reducer 的字段，LangGraph 会抛出 `InvalidUpdateError`。

```python
class State(TypedDict):
    status: str  # 无 reducer

def node_a(state: State) -> dict:
    return {"status": "A"}

def node_b(state: State) -> dict:
    return {"status": "B"}

# 如果 node_a 和 node_b 并行执行：
# InvalidUpdateError: At key 'status': Can receive only one value per step.
# Use an Annotated key to handle multiple values.
```

**解决方案**：为该字段添加 reducer，或者确保两个节点不在同一步骤执行。


### 1.5 Channel：State 的底层实现

State Schema 是开发者看到的抽象，但 LangGraph 运行时并不直接操作 Python 字典。底层通过 **Channel** 来管理每个状态字段的读写。理解 Channel 是理解 LangGraph 状态合并行为的关键。

LangGraph 有三种基本 Channel 类型：

| Channel 类型 | 行为 | 对应 State 字段 |
|---|---|---|
| `LastValue` | 存储最后一个值，多于一个值时报错 | 无 reducer 的字段 |
| `Topic` | 可配置的 PubSub 主题，累积多个值 | 有 reducer 的字段（如 `operator.add`） |
| `BinaryOperatorAggregate` | 用二元操作符折叠值，持久存储 | 有 reducer 的字段（如 `operator.add` 用于标量） |

当你定义 `Annotated[int, operator.add]` 时，LangGraph 实际上创建了一个 `BinaryOperatorAggregate` channel，其二元操作符就是 `operator.add`。当你定义无标注的 `str` 字段时，创建的是 `LastValue` channel——它只接受一个值，收到多个值就报错。


### 1.6 输入 Schema 和输出 Schema

默认情况下，图的输入和输出使用同一个 State Schema。但你可以分别指定，以限制输入或输出暴露的字段。

```python
class FullState(TypedDict):
    user_input: str
    messages: Annotated[list, add_messages]
    internal_trace: list[str]  # 不想暴露给调用方
    final_answer: str

class InputSchema(TypedDict):
    user_input: str

class OutputSchema(TypedDict):
    final_answer: str

graph = StateGraph(
    state_schema=FullState,
    input_schema=InputSchema,
    output_schema=OutputSchema,
)
```

这样调用 `graph.invoke({"user_input": "..."})` 时不需要提供 `messages` 和 `internal_trace`，返回结果也只包含 `final_answer`。注意：`input_schema` 和 `output_schema` 是 state_schema 的子集，内部运行时始终使用完整的 `FullState`。


### 1.7 持久化：Checkpointer 如何保存 State

State 的默认生命周期是单次 `invoke` 调用。要让状态跨多次调用持久化（例如多轮对话），需要配置 **Checkpointer**。

```python
from langgraph.checkpoint.memory import InMemorySaver
from langgraph.graph import StateGraph

builder = StateGraph(MyState)
# ... 添加节点和边 ...
checkpointer = InMemorySaver()
graph = builder.compile(checkpointer=checkpointer)

# 使用 thread_id 标识会话
config = {"configurable": {"thread_id": "conversation-1"}}
graph.invoke({"messages": [{"role": "user", "content": "hi"}]}, config)
# 同一 thread 的下一次调用会恢复之前的状态
graph.invoke({"messages": [{"role": "user", "content": "what did I say?"}]}, config)
```

**生产环境**使用 PostgresSaver 或 MongoDB Saver 将状态持久化到数据库。

Checkpointer 在每个超级步骤结束后保存一个快照。这意味着它记录了状态随时间变化的完整历史，支持时间旅行调试和从任意检查点恢复执行。


## 第二部分：Runtime——运行作用域的注入体系

### 2.1 Runtime 的本质

如果说 State 是“节点之间传递的数据”，那么 Runtime 就是“节点执行时需要的环境”。它包含不可变的运行上下文、长期记忆存储、流写入器等工具。

在 LangGraph v0.6 之前，运行作用域的数据通过 `config['configurable']` 传递，访问方式是嵌套字典的 `.get()` 链，缺乏类型安全。v0.6 引入了 Runtime 对象和 Context API，把这些工具统一到一个注入参数中。


### 2.2 Runtime 的完整定义

Runtime 是一个 dataclass，泛型参数 `ContextT` 指定上下文类型：

```python
@dataclass
class Runtime(Generic[ContextT]):
    context: ContextT                          # 静态上下文（不可变）
    store: BaseStore | None                    # 长期记忆存储
    stream_writer: StreamWriter                # 自定义流写入器
    heartbeat: ...                             # 心跳
    previous: Any | None                       # 函数式 API 的上一个返回值
    execution_info: ExecutionInfo | None       # 执行身份信息
    server_info: ServerRuntime | None          # 服务端元数据
```

> **注意**：Runtime **不包含** `config`。要访问 RunnableConfig，需要单独作为节点参数声明。

### 2.3 三种上下文类型的对比

LangGraph 的上下文体系可以按两个维度划分：**可变性**（静态 vs 动态）和**生命周期**（单次运行 vs 跨会话）。

| 上下文类型 | 可变性 | 生命周期 | 访问方式 |
|---|---|---|---|
| 静态运行时上下文 | 不可变 | 单次运行 | `runtime.context` |
| 动态运行时上下文（State） | 可变 | 单次运行 | 节点函数的 `state` 参数 |
| 跨会话上下文（Store） | 可变 | 跨对话 | `runtime.store` |

**静态上下文**适合放那些在运行期间不会改变的数据：用户 ID、数据库连接、API 密钥、模型选择等。

**动态上下文（State）** 是运行期间演变的数据：对话历史、中间结果、工具调用的观察结果。

**Store** 是跨线程的长期记忆：用户偏好、积累的知识、应该跨对话保留的事实。Store 的 API 是键值对形式，按命名空间组织。


### 2.4 节点函数签名与 Runtime 注入

LangGraph 的节点函数支持多种签名，Runtime 通过类型注解自动注入。

**签名一：仅 State**

```python
def simple_node(state: State) -> dict:
    return {"count": state["count"] + 1}
```

**签名二：State + Runtime**

```python
from langgraph.runtime import Runtime

@dataclass
class Context:
    user_id: str
    db_connection: str

def context_node(state: State, runtime: Runtime[Context]) -> dict:
    user_id = runtime.context.user_id       # 类型安全访问
    db = runtime.context.db_connection
    return {"message": f"User {user_id}"}
```

**签名三：State + Runtime + Config**

```python
from langchain_core.runnables import RunnableConfig

def full_node(state: State, runtime: Runtime[Context], config: RunnableConfig) -> dict:
    thread_id = config["configurable"]["thread_id"]
    return {"message": f"Thread {thread_id}"}
```

**签名四：Async 节点**

```python
async def async_node(state: State, runtime: Runtime[Context]) -> dict:
    data = await fetch_from_api(runtime.context.user_id)
    return {"data": data}
```

**签名五：Runnable 作为节点**

```python
from langchain_core.runnables import RunnableLambda

chain = prompt | model
builder.add_node("llm", chain)
```


### 2.5 静态上下文的完整代码示例

以下是一个完整示例，展示了 Context 的定义、传递和在节点中的使用：

```python
from dataclasses import dataclass
from typing_extensions import TypedDict
from langgraph.graph import StateGraph, START, END
from langgraph.runtime import Runtime

# 1. 定义 State Schema
class State(TypedDict):
    query: str
    answer: str

# 2. 定义 Context Schema
@dataclass
class Context:
    user_id: str
    db_connection: str
    model_name: str

# 3. 定义节点
def process_query(state: State, runtime: Runtime[Context]) -> dict:
    user_id = runtime.context.user_id
    model = runtime.context.model_name
    # 使用上下文中的连接和模型
    result = call_model(model, state["query"], user_id)
    return {"answer": result}

# 4. 构建图
builder = StateGraph(State, context_schema=Context)
builder.add_node("process", process_query)
builder.add_edge(START, "process")
builder.add_edge("process", END)
graph = builder.compile()

# 5. 调用时传递 Context
result = graph.invoke(
    {"query": "What's my account balance?"},
    context=Context(
        user_id="user-123",
        db_connection="postgresql://...",
        model_name="claude-sonnet-4-6",
    ),
)
```

**迁移说明**：v0.5 及之前的写法是 `StateGraph(state_schema=State, config_schema=Config)`，节点函数接收 `config: RunnableConfig` 并通过 `config["configurable"]["user_id"]` 访问。v0.6 之后 `config_schema` 被弃用，将在 v2.0.0 移除。


### 2.6 在工具中使用 Runtime

在 `create_agent` 创建的工具中，Runtime 通过 `ToolRuntime` 注入。这让你可以在工具内部访问用户上下文和 Store。

```python
from dataclasses import dataclass
from langchain.tools import tool, ToolRuntime

@dataclass
class Context:
    user_id: str

@tool
def fetch_preferences(runtime: ToolRuntime[Context]) -> str:
    """Fetch the user's preferences from long-term memory."""
    user_id = runtime.context.user_id
    if runtime.store:
        memory = runtime.store.get(("preferences", user_id), "settings")
        if memory:
            return memory.value["preferences"]
    return "No preferences found."

agent = create_agent(
    model="claude-sonnet-4-6",
    tools=[fetch_preferences],
    context_schema=Context,
)
```

**Store 的命名空间设计**在这里很关键。`runtime.store.get((“preferences”, user_id), “settings”)` 中，第一个参数是命名空间（支持动态构造），第二个参数是键。这样你可以按用户隔离数据，同时跨线程持久化。


### 2.7 Runtime 中的执行信息

在 LangGraph Server 环境中，Runtime 还包含执行身份信息：

```python
@tool
def context_aware_tool(runtime: ToolRuntime) -> str:
    info = runtime.execution_info
    print(f"Thread: {info.thread_id}, Run: {info.run_id}")

    server = runtime.server_info
    print(f"Assistant: {server.assistant_id}, User: {server.user}")
    return "ok"
```

`execution_info` 包含 thread ID、run ID 和重试次数，对于调试和审计非常有用。


### 2.8 Runtime 与 Store 的协同：跨线程记忆

Store 和 Checkpointer 是两个不同的持久化机制。Checkpointer 保存每个 thread 的 State 快照，Store 保存跨 thread 的键值数据。

```python
from langgraph.store.memory import InMemoryStore

store = InMemoryStore()
graph = builder.compile(checkpointer=checkpointer, store=store)

def save_preference(state: State, runtime: Runtime[Context]) -> dict:
    user_id = runtime.context.user_id
    # 写入长期记忆
    runtime.store.put(
        ("preferences", user_id),
        "theme",
        {"value": state["selected_theme"]},
    )
    return {}
```


## 第三部分：State 与 Runtime 的协同

### 3.1 完整的综合示例

以下示例展示了 State（短期记忆）和 Runtime Context（静态依赖）以及 Store（长期记忆）在一个多轮对话 Agent 中的协同工作：

```python
from dataclasses import dataclass, field
from typing_extensions import TypedDict, Annotated
import operator
from langgraph.graph import StateGraph, START, END
from langgraph.graph.message import add_messages
from langgraph.runtime import Runtime
from langgraph.checkpoint.memory import InMemorySaver
from langgraph.store.memory import InMemoryStore

# State：短期记忆
class ChatState(TypedDict):
    messages: Annotated[list, add_messages]
    turn_count: Annotated[int, operator.add]
    user_profile: dict

# Context：静态依赖
@dataclass
class Context:
    user_id: str
    model: str = "claude-sonnet-4-6"

# 节点：结合 State 和 Runtime
def chat_node(state: ChatState, runtime: Runtime[Context]) -> dict:
    user_id = runtime.context.user_id
    model = runtime.context.model

    # 从 Store 读取长期记忆
    profile = {}
    if runtime.store:
        mem = runtime.store.get(("profile", user_id), "info")
        if mem:
            profile = mem.value

    # 调用 LLM（伪代码）
    response = call_llm(model, state["messages"], profile)

    # 写入 Store
    if runtime.store:
        runtime.store.put(
            ("profile", user_id),
            "info",
            {**profile, "last_seen": "now"},
        )

    return {
        "messages": [response],
        "turn_count": 1,
        "user_profile": profile,
    }

# 构建图
builder = StateGraph(ChatState, context_schema=Context)
builder.add_node("chat", chat_node)
builder.add_edge(START, "chat")
builder.add_edge("chat", END)

graph = builder.compile(
    checkpointer=InMemorySaver(),
    store=InMemoryStore(),
)

# 调用
config = {"configurable": {"thread_id": "thread-1"}}
result = graph.invoke(
    {"messages": [{"role": "user", "content": "Hello"}]},
    config=config,
    context=Context(user_id="user-123"),
)
```


### 3.2 子图的 State 与 Checkpointer 作用域

当使用子图时，Checkpointer 的配置有重要的规则：**只给父图配置 Checkpointer，子图不要单独配置**。如果子图编译时带自己的 Checkpointer，会创建独立的检查点命名空间，导致文档体积膨胀和恢复时的状态持久化问题。

子图的 Checkpointer 行为有三种模式：

| 子图 checkpointer 设置 | 持久化行为 | 适用场景 |
|---|---|---|
| `checkpointer=False` | 不持久化，即使父图有 Checkpointer | 纯计算子图 |
| 不设置（默认） | 继承父图 Checkpointer 用于 interrupt，但每次调用状态重置 | 需要 HITL 但不需要跨调用记忆 |
| 设置 Checkpointer | 独立持久化 | 需要跨调用记忆的子图 |


## 总结

LangGraph 的 State 和 Runtime 构成了一个分层的上下文体系：

**State** 是节点之间的通信媒介。它通过 Schema（TypedDict/Pydantic/dataclass）定义结构，通过 Reducer（`Annotated[type, reducer]`）控制合并语义，底层由 Channel（`LastValue`/`Topic`/`BinaryOperatorAggregate`）实现读写行为。State 的默认生命周期是单次运行，通过 Checkpointer 可以扩展为跨调用持久化。

**Runtime** 是节点执行时的环境容器。它通过 `Runtime[Context]` 泛型注入到节点函数中，提供对静态上下文（`context`）、长期记忆（`store`）、流写入（`stream_writer`）和执行元数据（`execution_info`）的访问。v0.6 的 Context API 将原来分散在 `config['configurable']` 中的配置统一到了类型安全的 Context 对象中。

两者的边界清晰：**运行期间会变的数据放 State，运行期间不变的数据放 Context，跨会话持久的数据放 Store**。理解了这条边界，就理解了 LangGraph 上下文管理的全部精髓。


节点返回的 dict 只影响 State。  
Runtime 不是 State 的一部分，也不参与 State 合并。  
它是框架递给节点的“当前运行环境句柄”。

---

## 一、先用一个类比理解 Runtime

你可以把 LangGraph 的一次执行想象成一个 Web 请求。

| LangGraph 概念 | Web 类比 |
|---|---|
| State | 请求体 + 会话状态 + 本次请求产生的数据 |
| 节点 | Controller / Service 方法 |
| Runtime | Request 对象 + 依赖注入容器 |
| Runtime.context | 当前用户、数据库连接、API Key、租户信息 |
| Runtime.store | 用户数据库 / 长期记忆库 |
| Runtime.stream_writer | SSE / WebSocket 输出通道 |
| Runtime.execution_info | 当前 request_id、thread_id、run_id |
| Checkpointer | Session 存储 |
| Store | 跨会话的用户数据库 |

在 Web 框架里，Controller 不会把数据库连接、当前用户、请求 ID 塞进请求体里传来传去。  
它们通过依赖注入进入方法参数。

LangGraph 的 Runtime 就是类似的东西：

- State 是“图内部流动的数据”。
- Runtime 是“节点执行时的依赖注入环境”。
- 节点返回的是“对 State 的补丁”。
- 节点通过 Runtime 访问“外部依赖和框架能力”。

---

## 二、State 和 Runtime 的本质区别

先给一张对比表：

| 维度 | State | Runtime |
|---|---|---|
| 本质 | 图内共享数据 | 运行环境 / 依赖容器 |
| 生命周期 | 单次运行，可 checkpoint 跨调用 | 单次运行，context 不持久化 |
| 谁创建 | 调用方输入 + 节点更新合并 | 框架在 invoke/stream 时创建 |
| 谁修改 | 节点返回 partial dict | 节点读 context；写 store/stream |
| 合并机制 | reducer / channel | 无 reducer，不参与图状态合并 |
| 序列化 | 需要可序列化（checkpoint） | 通常不可序列化，不应 checkpoint |
| 作用域 | 当前 thread | 当前 run；store 跨 thread |
| 典型内容 | messages、counter、中间结果 | user_id、db、api_key、store、stream_writer、execution_info |
| 节点间传递 | 是 | 否。context 对所有节点相同，store 是外部通道 |
| 路由 | 可用于条件边 | 不用于图路由 |

一句话：

> **State 是数据平面，Runtime 是控制/依赖平面。**

节点返回的是“对数据平面的补丁”。  
Runtime 是“执行时从控制平面注入的环境”。

---

## 三、Runtime 到底包含什么？

LangGraph 的 Runtime 大致可以理解为这样一个 dataclass：

```python
from dataclasses import dataclass
from typing import Generic, TypeVar, Any

ContextT = TypeVar("ContextT")

@dataclass
class Runtime(Generic[ContextT]):
    context: ContextT                  # 静态上下文，调用方传入
    store: BaseStore | None            # 长期记忆存储
    stream_writer: StreamWriter        # 自定义流写入器
    heartbeat: Heartbeat               # 心跳
    previous: Any | None               # 函数式 API 的上一个返回值
    execution_info: ExecutionInfo | None
    server_info: ServerRuntime | None
```

注意两个点：

1. **Runtime 不包含 `config`**。  
   如果你要访问 `RunnableConfig`，需要单独声明参数。
2. **普通节点的 Runtime 不包含 `state`**。  
   State 是单独参数传入的。  
   只有在工具里，`ToolRuntime` 才会包含一个只读的 state 快照。

下面逐字段解释。

---

### 3.1 `runtime.context`：静态业务上下文

这是最常用、也是你最需要理解的字段。

`context` 是调用方在 `invoke` 时传入的：

```python
graph.invoke(
    {"query": "你好"},
    context=Context(user_id="u1", db=db, api_key="sk-..."),
)
```

它在本次运行期间是**静态的、不可变的**。  
所有节点看到的是同一个 `context`。

适合放：

- 用户 ID
- 租户 ID
- 数据库连接
- API Key
- 模型选择
- 权限信息
- 请求级配置

不适合放：

- 对话消息
- 中间结果
- 计数器
- 需要被 reducer 合并的数据

代码示例：

```python
from dataclasses import dataclass
from typing_extensions import TypedDict
from langgraph.graph import StateGraph, START, END
from langgraph.runtime import Runtime

class State(TypedDict):
    query: str
    answer: str

@dataclass
class Context:
    user_id: str
    db_connection: str
    model_name: str

def process_query(state: State, runtime: Runtime[Context]) -> dict:
    user_id = runtime.context.user_id
    model = runtime.context.model_name
    db = runtime.context.db_connection

    result = call_model(model, state["query"], user_id)
    return {"answer": result}

builder = StateGraph(State, context_schema=Context)
builder.add_node("process", process_query)
builder.add_edge(START, "process")
builder.add_edge("process", END)
graph = builder.compile()

result = graph.invoke(
    {"query": "我的余额是多少？"},
    context=Context(
        user_id="user-123",
        db_connection="postgresql://...",
        model_name="claude-sonnet-4-6",
    ),
)
```

这里：

- `state["query"]` 来自 State。
- `runtime.context.user_id` 来自 Runtime。
- 节点返回 `{"answer": result}`，这是对 State 的补丁。
- `context` 不会被写进 State，也不会被 checkpoint。

---

### 3.2 `runtime.store`：跨 thread 长期记忆

`store` 是跨对话、跨 thread 的长期记忆。  
它和 Checkpointer 不是一回事。

- Checkpointer 保存的是当前 thread 的 State 快照。
- Store 保存的是跨 thread 的键值数据。

Store 的 API 通常是：

```python
runtime.store.put(namespace, key, value)
runtime.store.get(namespace, key)
runtime.store.search(namespace_prefix)
runtime.store.delete(namespace, key)
```

命名空间是元组，适合按用户隔离：

```python
runtime.store.put(("preferences", user_id), "theme", {"value": "dark"})
```

代码示例：

```python
from langgraph.store.memory import InMemoryStore

store = InMemoryStore()
graph = builder.compile(checkpointer=checkpointer, store=store)

def save_preference(state: State, runtime: Runtime[Context]) -> dict:
    user_id = runtime.context.user_id
    runtime.store.put(
        ("preferences", user_id),
        "theme",
        {"value": state["selected_theme"]},
    )
    return {}
```

注意：

- `store.put` 是立即副作用，不经过 State。
- 它不会自动出现在 State 里。
- 如果你要把 Store 里的数据放进图内，需要显式返回 State 更新。

---

### 3.3 `runtime.stream_writer`：自定义流输出

节点可以通过 `stream_writer` 向外部发自定义事件：

```python
def node(state: State, runtime: Runtime[Context]) -> dict:
    runtime.stream_writer({"progress": 50, "msg": "处理中"})
    return {"done": True}
```

客户端用 `stream_mode="custom"` 消费：

```python
for chunk in graph.stream(..., stream_mode="custom"):
    print(chunk)
```

它适合：

- 进度条
- 中间状态提示
- 调试事件
- 前端实时反馈

不适合：

- 需要被 reducer 合并的数据
- 需要持久化的业务状态

---

### 3.4 `runtime.execution_info`：执行身份信息

在 LangGraph Server 环境中，`execution_info` 包含：

- `thread_id`
- `run_id`
- 重试次数等

适合：

- 日志
- 审计
- 幂等键
- 调试

代码示例：

```python
def node(state: State, runtime: Runtime[Context]) -> dict:
    info = runtime.execution_info
    print(f"thread_id={info.thread_id}, run_id={info.run_id}")
    return {}
```

它不应该放 State，因为它是框架生成的，每次运行都不同，也不需要图内传递。

---

### 3.5 `runtime.server_info`：服务端元信息

在 LangGraph Server 中，`server_info` 可能包含：

- `assistant_id`
- `user`
- `graph_id`

适合服务端审计和权限判断。

---

### 3.6 `runtime.heartbeat` 和 `runtime.previous`

- `heartbeat`：长任务心跳，普通场景少用。
- `previous`：函数式 API 中上一个任务的返回值，普通 StateGraph 节点通常为 `None`。

---

### 3.7 `config` 不在 Runtime 里

这是一个容易困惑的点。

LangGraph v0.6 之后：

- 业务依赖放 `runtime.context`。
- 框架配置仍然通过 `RunnableConfig` 单独传入。

代码示例：

```python
from langchain_core.runnables import RunnableConfig

def node(
    state: State,
    runtime: Runtime[Context],
    config: RunnableConfig,
) -> dict:
    thread_id = config["configurable"]["thread_id"]
    user_id = runtime.context.user_id
    return {"msg": f"thread={thread_id}, user={user_id}"}
```

分工：

| 内容 | 放哪 |
|---|---|
| thread_id、callbacks、recursion_limit、tags、metadata | `config` |
| user_id、db、api_key、tenant | `runtime.context` |
| 长期记忆 | `runtime.store` |
| 流事件 | `runtime.stream_writer` |
| 执行身份 | `runtime.execution_info` |

---

## 四、Runtime 是怎么注入的？

LangGraph 节点函数支持多种签名。

### 4.1 只声明 State

```python
def node(state: State) -> dict:
    return {"count": state["count"] + 1}
```

### 4.2 声明 State + Runtime

```python
def node(state: State, runtime: Runtime[Context]) -> dict:
    user_id = runtime.context.user_id
    return {"msg": f"user={user_id}"}
```

### 4.3 声明 State + Runtime + Config

```python
def node(
    state: State,
    runtime: Runtime[Context],
    config: RunnableConfig,
) -> dict:
    thread_id = config["configurable"]["thread_id"]
    user_id = runtime.context.user_id
    return {"msg": f"thread={thread_id}, user={user_id}"}
```

### 4.4 异步节点

```python
async def node(state: State, runtime: Runtime[Context]) -> dict:
    data = await fetch(runtime.context.user_id)
    return {"data": data}
```

框架大致做的事情类似：

```python
def invoke(input, config=None, context=None):
    state = load_or_init_state(input)
    runtime = Runtime(
        context=context,
        store=store,
        stream_writer=StreamWriter(...),
        execution_info=ExecutionInfo(...),
        server_info=...,
    )

    for step in supersteps:
        updates = []
        for node in step.nodes:
            result = node(state, runtime, config)
            updates.append(result)
        state = apply_reducers(state, updates)
        save_checkpoint(state)

    return state
```

重点：

- Runtime 由框架创建。
- Context 由调用方传入。
- State 由输入和 Checkpointer 恢复。
- 节点返回值被 reducer 合并。
- Runtime 不参与合并。

---

## 五、为什么不能把所有东西都塞进 State？

因为 State 和 Runtime 的生命周期、职责完全不同。

### 5.1 序列化问题

State 会被 Checkpointer 序列化保存。  
如果你把数据库连接放 State：

```python
class BadState(TypedDict):
    db: Connection  # 不可序列化
    user_id: str
```

Checkpointer 会失败，或者产生安全风险。

正确做法：

```python
@dataclass
class Context:
    db: Connection
    user_id: str
```

### 5.2 并发合并问题

State 字段可能被多个并行节点更新。  
如果没有 reducer，会报错：

```python
class State(TypedDict):
    status: str  # 无 reducer

def node_a(state): return {"status": "A"}
def node_b(state): return {"status": "B"}
```

如果 `node_a` 和 `node_b` 并行执行：

```text
InvalidUpdateError: At key 'status': Can receive only one value per step.
```

Runtime 不解决这个问题。  
State 冲突要靠 reducer 或执行顺序解决。

### 5.3 安全与隔离问题

多用户并发时，如果把用户信息放全局变量，会串数据。  
Runtime.context 是每次 invoke 独立传入的：

```python
async def run(user_id):
    return await graph.ainvoke(
        {"messages": []},
        context=Context(user_id=user_id),
        config={"configurable": {"thread_id": user_id}},
    )

await asyncio.gather(run("A"), run("B"))
```

节点里：

```python
def node(state, runtime: Runtime[Context]):
    user_id = runtime.context.user_id  # A 和 B 各自独立
    return {}
```

### 5.4 职责分离问题

State 应该只关心：

- 图内数据流
- 节点间传递
- reducer 合并
- 路由判断
- checkpoint 持久化

Runtime 应该关心：

- 外部依赖
- 长期记忆
- 流输出
- 执行身份
- 服务端信息

混在一起会让图变得不可测试、不可持久化、不可并发。

---

## 六、State、Context、Store、Config、ExecutionInfo 的边界

| 对象 | 生命周期 | 是否可变 | 是否持久化 | 典型用途 |
|---|---|---|---|---|
| State | 当前 thread | 可变，节点返回补丁 | Checkpointer | 消息、中间结果、计数器 |
| Context | 当前 run | 静态，视为只读 | 否 | 用户、租户、DB、API Key |
| Store | 跨 thread | 可写 | Store 后端 | 用户偏好、长期记忆 |
| Config | 当前 run | 只读 | 否 | thread_id、callbacks、recursion_limit |
| ExecutionInfo | 当前 run | 只读 | 否 | run_id、thread_id、重试次数 |
| StreamWriter | 当前 run | 可写 | 否 | 进度、自定义事件 |

决策树：

1. 需要被节点返回并合并吗？是 -> State。
2. 本次运行所有节点都一样，且调用方提供吗？是 -> Context。
3. 要跨对话记住吗？是 -> Store。
4. 是框架执行元信息吗？是 -> ExecutionInfo / Config。
5. 是流式事件吗？是 -> StreamWriter。

---

## 七、完整综合示例

下面示例同时使用 State、Context、Store、StreamWriter、Config。

```python
from dataclasses import dataclass
from typing_extensions import TypedDict, Annotated
import operator

from langgraph.graph import StateGraph, START, END
from langgraph.graph.message import add_messages
from langgraph.runtime import Runtime
from langgraph.checkpoint.memory import InMemorySaver
from langgraph.store.memory import InMemoryStore
from langchain_core.runnables import RunnableConfig

class ChatState(TypedDict):
    messages: Annotated[list, add_messages]
    turn_count: Annotated[int, operator.add]
    user_profile: dict

@dataclass
class Context:
    user_id: str
    model: str = "claude-sonnet-4-6"

def chat_node(
    state: ChatState,
    runtime: Runtime[Context],
    config: RunnableConfig,
) -> dict:
    user_id = runtime.context.user_id
    model = runtime.context.model
    thread_id = config["configurable"]["thread_id"]

    # 发自定义流事件
    runtime.stream_writer({"progress": 10, "msg": "开始处理"})

    # 从 Store 读长期记忆
    profile = {}
    if runtime.store:
        mem = runtime.store.get(("profile", user_id), "info")
        if mem:
            profile = mem.value

    # 调用 LLM（伪代码）
    response = call_llm(model, state["messages"], profile)

    # 写 Store
    if runtime.store:
        runtime.store.put(
            ("profile", user_id),
            "info",
            {**profile, "last_thread": thread_id},
        )

    # 返回 State 补丁
    return {
        "messages": [response],
        "turn_count": 1,
        "user_profile": profile,
    }

builder = StateGraph(ChatState, context_schema=Context)
builder.add_node("chat", chat_node)
builder.add_edge(START, "chat")
builder.add_edge("chat", END)

graph = builder.compile(
    checkpointer=InMemorySaver(),
    store=InMemoryStore(),
)

config = {"configurable": {"thread_id": "thread-1"}}
result = graph.invoke(
    {"messages": [{"role": "user", "content": "你好"}]},
    config=config,
    context=Context(user_id="user-123"),
)
```

这个例子里：

- `messages`、`turn_count`、`user_profile` 是 State。
- `user_id`、`model` 是 Context。
- `profile` 是 Store 长期记忆。
- `thread_id` 是 Config。
- `stream_writer` 发进度事件。
- 节点返回 dict，只更新 State。

---

## 八、工具中的 Runtime：ToolRuntime

在 `create_agent` 创建的工具里，Runtime 通过 `ToolRuntime` 注入。

```python
from dataclasses import dataclass
from langchain.tools import tool, ToolRuntime

@dataclass
class Context:
    user_id: str

@tool
def save_note(note: str, runtime: ToolRuntime[Context]) -> str:
    """Save a note for the user."""
    user_id = runtime.context.user_id

    if runtime.store:
        runtime.store.put(
            ("notes", user_id),
            "note",
            {"text": note},
        )

    return "saved"
```

`ToolRuntime` 和普通 Runtime 的区别：

| 普通节点 Runtime | 工具 ToolRuntime |
|---|---|
| 不包含 state | 包含只读 state 快照 |
| State 单独参数 | 没有 State 参数 |
| 可访问 context/store/stream_writer | 可访问 context/store/stream_writer/config/tool_call_id |
| 用于节点 | 用于工具 |

工具中不要直接修改 state。  
如果要更新 State，通常返回 `Command` 或让 Agent 处理。

---

## 九、常见误区

### 误区 1：把 Runtime 当 State 用

错误：

```python
def node(state, runtime):
    runtime.context.counter += 1  # 不要改 context
    return {}
```

正确：

```python
class State(TypedDict):
    counter: Annotated[int, operator.add]

def node(state, runtime):
    return {"counter": 1}
```

### 误区 2：把数据库连接放 State

错误：

```python
class State(TypedDict):
    db: Connection
```

正确：

```python
@dataclass
class Context:
    db: Connection
```

### 误区 3：用 Runtime 在节点间传业务数据

错误：

```python
def node_a(state, runtime):
    runtime.context.temp_data = "abc"
    return {}

def node_b(state, runtime):
    print(runtime.context.temp_data)  # 不可靠
```

正确：

```python
class State(TypedDict):
    temp_data: str

def node_a(state, runtime):
    return {"temp_data": "abc"}

def node_b(state, runtime):
    print(state["temp_data"])
```

### 误区 4：忘记传 context

```python
graph.invoke({"query": "hi"})  # 如果节点需要 Runtime[Context]，会缺 context
```

正确：

```python
graph.invoke(
    {"query": "hi"},
    context=Context(user_id="u1"),
)
```

### 误区 5：在工具中找不到 runtime

需要声明：

```python
from langchain.tools import ToolRuntime

@tool
def my_tool(runtime: ToolRuntime[Context]) -> str:
    ...
```

---

## 十、最终总结

你的理解：

> 数据通过 State 传递，每个节点返回对 State 的更改请求。

这是对的。

现在补上 Runtime：

> Runtime 是节点执行时框架注入的“环境容器”。  
> 它不参与 State 数据流，不参与 reducer 合并，不会被 checkpoint。  
> 它提供节点执行时需要的外部依赖和框架能力：  
> `context`、`store`、`stream_writer`、`execution_info`、`server_info` 等。

所以完整公式是：

```python
节点 = (State, Runtime, Config) -> Partial<State>

State' = apply_reducers(State, 节点返回值)

Runtime = {
    context,        # 静态业务依赖，调用方传入
    store,          # 跨 thread 长期记忆
    stream_writer,  # 自定义流输出
    execution_info, # 当前 run 身份
    server_info,    # 服务端信息
}
```

一句话记住：

> **State 管图内数据，Runtime 管执行环境。**  
> **节点返回的是对 State 的补丁，Runtime 是节点执行时用来访问外部世界和框架能力的接口。**

这样理解，Runtime 就不再神秘了。它不是另一个 State，而是节点执行时的“依赖注入容器 + 工具箱 + 当前运行上下文”。