# 异步子代理
异步子代理允许主管代理启动后台任务，启动后会立刻返回，这样主管就可以在子代理并发执行任务的同时，继续和用户交互。主管可以随时查看任务进度、发送后续指令或者取消任务。

该能力建立在**子智能体**之上；普通子智能体是以同步方式运行，会阻塞主管，直到任务执行完毕。适合使用异步子代理的场景：任务执行时间长、可以并行执行，或者需要在任务运行途中进行干预控制。

异步子代理可以和任意实现了**智能体协议（Agent Protocol）**的服务器通信。你可以使用 LangSmith 部署服务，也可以自托管任何兼容智能体协议的服务器。每一个子代理都独立于主管运行；主管通过 SDK 来控制它们：启动、查询、更新、取消。

## 何时使用异步子代理
| 维度 | 同步子代理 | 异步子代理 |
| ---- | ---- | ---- |
| 执行模型 | 主管代理会一直阻塞，直到子代理完成 | 立即返回作业 ID；监督者继续执行 |
| 并发 | 并行但阻塞 | 并行且非阻塞 |
| 任务中更新 | 不可实现 | 通过 `update_async_task` 发送后续指令 |
| 取消 | 不可行 | 通过 `cancel_async_task` 取消正在运行的任务 |
| 状态性 | 无状态 — 调用之间无持久状态 | 有状态——在多次交互过程中，在其专属线程上维持状态 |
| 最适合 | 智能体需等待结果后再继续的任务 | 在聊天中以交互方式管理的长期、复杂任务 |

## 配置异步子代理
将异步子代理定义为`AsyncSubAgent`规范列表，每个规范都指向一台代理协议服务器：
```python
from deepagents import AsyncSubAgent, create_deep_agent

async_subagents = [
    AsyncSubAgent(
        name="researcher",
        description="信息搜集与综合的调研智能体",
        graph_id="researcher",
        # 不填url → ASGI传输（同服务协同部署）
    ),
    AsyncSubAgent(
        name="coder",
        description="用于代码生成和代码审查的编码智能体",
        graph_id="coder",
        # url="https://coder-deployment.langsmith.dev"  # 可选：远程HTTP传输
    ),
]

agent = create_deep_agent(
    model="google_genai:gemini-3.6-flash",
    subagents=async_subagents,
)
```

| 字段 | 类型 | 描述 |
| ---- | ---- | ---- |
| `name` | `str` | 必填。唯一标识。主管启动任务时使用该名称。 |
| `description` | `str` | 必填。描述该子代理能做什么。主管依靠它判断应该把任务委派给谁。 |
| `graph_id` | `str` | 必填。Agent Protocol服务器上的图ID（助手ID）。基于LangGraph的部署，该ID必须匹配`langgraph.json`里注册的图。 |
| `url` | `str` | 可选。如果省略，使用ASGI传输（进程内）。如果填写，则通过HTTP访问远程Agent Protocol服务器。 |
| `headers` | `dict[str, str]` | 可选。发往远程服务器请求的附加请求头，用于自托管Agent Protocol服务器的自定义鉴权。 |

对于基于LangGraph的协同部署，所有图都注册到同一个`langgraph.json`：
```json
{
  "graphs": {
    "supervisor": "./src/supervisor.py:graph",
    "researcher": "./src/researcher.py:graph",
    "coder": "./src/coder.py:graph"
  }
}
```

## 使用异步子代理工具


当你在 `create_deep_agent` 中配置了 `AsyncSubAgent` 后，`AsyncSubAgentMiddleware` 会被自动包含在中间件栈中，并为监督智能体提供五个专用工具。这些工具完全独立于同步子智能体所使用的 `task` 工具：

| 工具 | 用途 | 返回值 |
| :--- | :--- | :--- |
| **`start_async_task`** | 启动一个新的后台任务 | **立即返回任务 ID** |
| **`check_async_task`** | 获取任务的当前状态和结果 | 状态 + 结果（如果已完成） |
| **`update_async_task`** | 向运行中的任务发送新指令 | 确认信息 + 更新后的状态 |
| **`cancel_async_task`** | 停止运行中的任务 | 确认信息 |
| **`list_async_tasks`** | 列出所有受追踪的任务及其实时状态 | 所有任务的摘要 |

监督智能体的 LLM 会像调用任何其他工具一样调用这些工具。`AsyncSubAgentMiddleware` 会在后台自动处理线程创建、运行管理和状态持久化。

### 🎯 主智能体如何选择异步子智能体？

主智能体通过 **`AsyncSubAgent` 规格（Spec）** 来选择异步子智能体，而不是通过 `task` 工具的 `subagent_type` 参数。

#### 配置方式
你在创建智能体时，通过 `subagents` 参数传入一个 `AsyncSubAgent` 对象的列表。每个规格定义了子智能体的名称、描述、以及它所指向的远程图（graph）ID。

```python
from deepagents import AsyncSubAgent, create_deep_agent

async_subagents = [
    AsyncSubAgent(
        name="researcher",  # 唯一标识符，主智能体用它来启动任务
        description="Research agent for information gathering and synthesis",  # 主智能体据此决定何时委派
        graph_id="researcher",  # Agent Protocol 服务器上的图 ID
        # 省略 url 则使用 ASGI 传输（进程内），设置 url 则使用 HTTP 传输（远程）
    ),
    AsyncSubAgent(
        name="coder",
        description="Coding agent for code generation and review",
        graph_id="coder",
        url="https://coder-deployment.langsmith.dev",  # 远程部署
    ),
]

agent = create_deep_agent(
    model="google_genai:gemini-3.6-flash",
    subagents=async_subagents,  # 混合传入同步和异步规格
)
```

#### 路由机制
`create_deep_agent` 会根据 `subagents` 列表中每个规格的**类型**，将其路由到**不同的中间件**：`AsyncSubAgent` 被路由到 `AsyncSubAgentMiddleware`，而同步的 `SubAgent` 则被路由到 `SubAgentMiddleware`。因此，主智能体获得的工具集取决于它被配置了哪些类型的子智能体。

当主智能体决定委派任务时，它会根据 `AsyncSubAgent` 的 `description` 字段来判断哪个子智能体最合适，然后调用 `start_async_task`，并在调用中指定目标子智能体的 `name`。

### ⚡ 异步子智能体的激活机制

异步子智能体的激活遵循一个清晰的“启动 → 后台执行 → 按需检查”的生命周期。

#### 启动（Launch）
主智能体调用 **`start_async_task`** 工具，并提供任务描述。该工具会在服务器上创建一个**新线程（thread）**，以任务描述作为输入开始运行，并**立即返回线程 ID 作为任务 ID**。主智能体获得任务 ID 后，**不会轮询等待其完成**，而是可以继续与用户交互或处理其他工作。

#### 后台执行
子智能体在 Agent Protocol 服务器上**独立运行**，与监督智能体完全解耦。它拥有自己的状态和线程，可以维护跨交互的上下文。

#### 状态管理与检查
任务元数据（任务 ID、智能体名称、线程 ID、运行 ID、状态、时间戳）被存储在一个**专用的状态通道 `async_tasks`** 中，与消息历史记录分开。这一点至关重要，因为 Deep Agents 会在上下文窗口填满时压缩消息历史，如果任务 ID 仅存在于工具消息中，它们会在压缩期间丢失。

主智能体可以在任何时候调用 **`check_async_task`** 来查询进度，或调用 **`list_async_tasks`** 来查看所有受追踪任务的实时状态。

#### 中途干预
异步子智能体支持**有状态**的交互。主智能体可以通过 **`update_async_task`** 向运行中的任务发送新指令，子智能体会带着完整的对话历史和新指令重新启动，而**任务 ID 保持不变**。也可以通过 **`cancel_async_task`** 随时取消运行中的任务。

### 了解生命周期
典型交互流程：
1. **启动**：在服务器新建线程，使用任务描述作为输入开始执行，返回线程ID作为任务ID。主管把ID告知用户，**不会轮询等待任务完成**。
2. **查询**：获取当前运行状态。如果执行成功，读取线程状态，提取子代理最终输出；如果还在运行，则告知用户任务仍在进行。
3. **更新**：在同一个线程上创建新的运行，采用中断多任务策略。之前的运行被中断；子代理带着完整对话历史+新指令重新开始。**任务ID保持不变**。
4. **取消**：调用服务器`runs.cancel()`，将任务标记为`"cancelled"`。
5. **列表查询**：遍历所有跟踪任务。未结束任务会并行从服务器拉取实时状态；终态（成功、错误、取消）直接从缓存返回。

## 理解状态管理
任务元数据保存在主管图里一个独立状态通道（`async_tasks`），和消息历史分开。这点很关键：深度智能体会在上下文窗口占满时压缩消息历史。如果任务ID只保存在工具消息中，压缩的时候就会丢失。独立通道保证即使经过多轮总结，主管依旧可以用`list_async_tasks`找回所有任务。

每条被跟踪任务会记录：任务ID、代理名称、线程ID、运行ID、状态、时间戳（创建时间、上次查询时间、上次更新时间）。

## 选择传输方式
### ASGI传输（协同部署）
子代理配置不写`url`字段时，LangGraph SDK 使用 ASGI 传输。SDK调用走进程内函数调用，而不是HTTP网络请求。基于LangGraph的部署，要求所有图注册在同一个`langgraph.json`。

ASGI传输消除网络延迟，不需要额外鉴权配置。子代理仍然跑在独立线程，拥有独立状态。**推荐作为默认方案**。

### HTTP传输（远程）
增加`url`字段就切换为HTTP传输。SDK通过网络调用远程Agent Protocol服务器。
```python
from deepagents import AsyncSubAgent

AsyncSubAgent(
    name="researcher",
    description="调研智能体",
    graph_id="researcher",
    url="https://my-research-deployment.langsmith.dev",
)
```
LangGraph部署的身份认证由LangGraph SDK读取环境变量`LANGSMITH_API_KEY`（或`LANGGRAPH_API_KEY`）处理。自托管Agent Protocol服务器可能使用别的鉴权机制。

当子代理需要独立扩缩容、资源规格不一样，或者由其他团队维护时，选用HTTP传输。

## 选择部署拓扑
### 单节点部署
全部智能体协同部署在同一台服务器，使用ASGI传输。基于LangGraph的部署，所有图写进同一个`langgraph.json`。推荐新手从这个方案起步：只维护一台服务，智能体之间没有网络延迟。

### 分离部署
主管部署在一台服务器，子代理放在其他服务器，走HTTP传输。适合子代理需要不同算力规格、独立扩缩容的场景。

### 混合部署
一部分子代理通过ASGI协同部署，另一部分子代理通过HTTP远程部署。
```python
from deepagents import AsyncSubAgent

async_subagents = [
    AsyncSubAgent(
        name="researcher",
        description="调研智能体",
        graph_id="researcher",
        # 不写url → ASGI协同部署
    ),
    AsyncSubAgent(
        name="coder",
        description="编码智能体",
        graph_id="coder",
        url="https://coder-deployment.langsmith.dev",
        # 设置url → HTTP远程
    ),
]
```

## 最佳实践
### 本地开发时调整工作线程池大小
本地使用`langgraph dev`启动时，调大工作池容量，支持并发子代理。每一个正在执行的任务占用一个worker槽位。如果主管同时跑3个子代理任务，一共至少需要4个槽位（主管1个 + 3个子代理）。资源不足会导致新任务排队。
```bash
langgraph dev --n-jobs-per-worker 10
```

### 撰写清晰的子代理描述
主管根据描述判断启动哪个子代理。描述要具体，面向动作。

✅ 好示例：
```python
from deepagents import AsyncSubAgent

AsyncSubAgent(
    name="researcher",
    description="使用网页搜索进行深度调研。适合需要多次搜索并整合信息的问题。",
    graph_id="researcher",
)
```

❌ 差示例：
```python
from deepagents import AsyncSubAgent

AsyncSubAgent(
    name="helper",
    description="帮忙做各种事",
    graph_id="helper",
)
```

### 使用线程ID追踪链路
LangGraph部署中，每一次异步子代理运行都是标准LangGraph运行记录，可以在LangSmith完整查看。主管的追踪链路里会记录启动、查询、更新、取消、列表这些工具调用。每个子代理运行单独一条追踪记录，通过线程ID关联。可以使用线程ID（任务ID）把主管编排链路和子代理执行链路关联起来。

## 故障排查
### 主管启动任务之后立刻循环轮询查询
**问题**：主管刚启动任务，就反复调用check，把异步执行变成阻塞调用。
**解决方案**：中间件会注入系统提示词防止该行为。如果还是出现轮询，可以在主管系统提示词中加强约束：
```python
from deepagents import create_deep_agent

agent = create_deep_agent(
    model="google_genai:gemini-3.6-flash",
    system_prompt="""……你的指令……

启动异步子代理之后，**务必把控制权交还给用户。
永远不要在启动后立刻调用 check_async_task。""",
    subagents=async_subagents,
)
```

### 主管汇报过时的任务状态
**问题**：主管直接拿对话历史里保存的旧状态回答用户，而不发起新的check调用。
**解决方案**：中间件提示词会告诉模型：对话历史中的任务状态一定是过时的。如果问题依旧，增加显式要求：汇报状态前，必须调用`check`或`list`。

### 任务ID查找失败
**问题**：主管截断、改写任务ID，导致check或cancel调用失败。
**解决方案**：中间件提示词要求模型使用完整任务ID。如果持续截断，一般是模型本身的限制；可更换模型，或者在系统提示词加上：**永远输出完整task_id，不要截断、简写ID**。

### 子代理任务进入队列，迟迟不运行
**问题**：启动子代理卡住，很久才开始执行。
**解决方案**：大概率工作线程池耗尽。用`--n-jobs-per-worker`调大线程池。参考【本地开发时调整工作线程池大小】章节。
