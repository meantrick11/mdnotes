# 自定义 Deep Agents
了解如何通过系统提示词、工具、子代理等来自定义 Deep Agents

围绕你的业务目标构建执行框架。`create_deep_agent` 为你提供一套可投入生产的基础能力：对接你的数据、调整代理行为、并添加业务场景所需的各类能力。

支持模型服务商：Google、OpenAI、Anthropic、OpenRouter、Fireworks、Baseten、Ollama

```python
from deepagents import create_deep_agent

agent = create_deep_agent(
  model="google_genai:gemini-3.6-flash",
  system_prompt="You are a helpful assistant.",
  tools=[search, fetch_url],
  memory=["./AGENTS.md"],
  skills=["./skills/"],
)
```

```python
from deepagents import create_deep_agent

agent = create_deep_agent(
 model="openai:gpt-5.5",
 system_prompt="You are a helpful assistant.",
 tools=[search, fetch_url],
 memory=["./AGENTS.md"],
 skills=["./skills/"],
)
```

```python
from deepagents import create_deep_agent

agent = create_deep_agent(
 model="anthropic:claude-sonnet-4-6",
 system_prompt="You are a helpful assistant.",
 tools=[search, fetch_url],
 memory=["./AGENTS.md"],
 skills=["./skills/"],
)
```

```python
from deepagents import create_deep_agent

agent = create_deep_agent(
 model="openrouter:z-ai/glm-5.2",
 system_prompt="You are a helpful assistant.",
 tools=[search, fetch_url],
 memory=["./AGENTS.md"],
 skills=["./skills/"],
)
```

```python
from deepagents import create_deep_agent

agent = create_deep_agent(
 model="fireworks:accounts/fireworks/models/glm-5p2",
 system_prompt="You are a helpful assistant.",
 tools=[search, fetch_url],
 memory=["./AGENTS.md"],
 skills=["./skills/"],
)
```

```python
from deepagents import create_deep_agent

agent = create_deep_agent(
 model="baseten:zai-org/GLM-5.2",
 system_prompt="You are a helpful assistant.",
 tools=[search, fetch_url],
 memory=["./AGENTS.md"],
 skills=["./skills/"],
)
```

```python
from deepagents import create_deep_agent

agent = create_deep_agent(
 model="ollama:north-mini-code-1.0",
 system_prompt="You are a helpful assistant.",
 tools=[search, fetch_url],
 memory=["./AGENTS.md"],
 skills=["./skills/"],
)
```

| 参数 | 作用 |
|---|---|
| `model=` | 指定要使用的模型 |
| `system_prompt=` | 给代理的自定义指令 |
| `tools=` | 代理可以调用的领域工具 |
| `memory=` | 启动时加载的 AGENTS.md 文件 |
| `skills=` | 按需加载知识的技能目录 |
| `backend=` | 文件系统后端（默认为 StateBackend） |
| `permissions=` | 文件系统的路径级访问控制 |
| `subagents=` | 用于任务委派的自定义子代理 |
| `middleware=` | 合并到默认调用栈的额外中间件；若实例的 `.name` 与默认中间件匹配，则就地替换；不匹配的实例会追加到核心中间件末尾、配置集、提示缓存与内存模块之前 |
| `interrupt_on=` | 在工具调用前暂停，等待人工审批 |
| `response_format=` | 结构化输出的 Schema |
| `state_schema=` | 自定义图状态 Schema |
| `context_schema=` | 单次运行的运行时上下文 Schema（用户ID、API密钥、功能开关等） |
| profiles | 可复用的每套模型默认配置集合 |

## 完整函数签名
```python
create_deep_agent(
  model: str | BaseChatModel | None = None,
  tools: Sequence[BaseTool | Callable | dict[str, Any]] | None = None,
  *,
  system_prompt: str | SystemMessage | None = None,
  middleware: Sequence[AgentMiddleware[StateT_co, ContextT]] = (),
  subagents: Sequence[SubAgent | CompiledSubAgent | AsyncSubAgent] | None = None,
  skills: list[str] | None = None,
  memory: list[str] | None = None,
  permissions: list[FilesystemPermission] | None = None,
  backend: BackendProtocol | None = None,
  interrupt_on: dict[str, bool | InterruptOnConfig] | None = None,
  response_format: ResponseFormat[ResponseT] | type[ResponseT] | dict[str, Any] | None = None,
  state_schema: type[DeepAgentState] | None = None,
  context_schema: type[ContextT] | None = None,
  checkpointer: Checkpointer | None = None,
  store: BaseStore | None = None,
  debug: bool = False,
  name: str | None = None,
  cache: BaseCache | None = None
) -> CompiledStateGraph[AgentState[ResponseT], ContextT, InputAgentState, OutputAgentState[ResponseT]]
```

完整参数列表请查阅 `create_deep_agent` API 参考文档。
如果你希望从零搭建一套完全自定义的执行框架，请参阅**配置执行框架**文档，或者跟随**从零构建 Deep Agent**分步指南学习。

当你添加工具、子代理、后端组件时，可以使用 LangSmith 追踪各个组件的协同行为。按照**可观测性快速入门**完成配置；上线部署请参考**投入生产环境**文档。我们同时建议启用 LangSmith Engine，它可以监控追踪记录、检测问题并给出修复建议。

## 模型
传入 `provider:model` 格式的 `model` 字符串，也可以直接传入已经初始化完成的模型实例。查看**支持的模型**获取全部服务商，查看**推荐模型**获取经过测试的模型建议。

使用 `服务商:模型名` 格式（例如 `openai:gpt‑5.5`），可以快速切换模型。

支持服务商：
- OpenAI
- Anthropic
- Azure
- Google Gemini
- AWS Bedrock
- HuggingFace
- 其他

👉 阅读 OpenAI 聊天模型集成文档
```bash
pip install -U "langchain[openai]"
```

示例：使用默认参数、`init_chat_model`、模型类实例三种方式
```python
import os
from deepagents import create_deep_agent

os.environ["OPENAI_API_KEY"] = "sk-..."

agent = create_deep_agent(model="openai:gpt-5.5")
# 内部会调用 init_chat_model，使用该模型的默认参数
# 如需自定义模型参数，请直接使用 init_chat_model
```

```python
import os
from langchain.chat_models import init_chat_model
from deepagents import create_deep_agent

os.environ["OPENAI_API_KEY"] = "sk-..."

model = init_chat_model(model="openai:gpt-5.5")
agent = create_deep_agent(model=model)
```

```python
import os
from langchain_openai import ChatOpenAI
from deepagents import create_deep_agent

os.environ["OPENAI_API_KEY"] = "sk-..."

model = ChatOpenAI(model="gpt-5.5")
agent = create_deep_agent(model=model)
```

👉 阅读 Anthropic 聊天模型集成文档
```bash
pip install -U "langchain[anthropic]"
```

```python
import os
from deepagents import create_deep_agent

os.environ["ANTHROPIC_API_KEY"] = "sk-..."

agent = create_deep_agent(model="anthropic:claude-sonnet-4-6")
# 内部调用 init_chat_model，使用模型默认参数
# 如需自定义参数，直接调用 init_chat_model
```

```python
import os
from langchain.chat_models import init_chat_model
from deepagents import create_deep_agent

os.environ["ANTHROPIC_API_KEY"] = "sk-..."

model = init_chat_model(model="claude-sonnet-4-6")
agent = create_deep_agent(model=model)
```

```python
import os
from langchain_anthropic import ChatAnthropic
from deepagents import create_deep_agent

os.environ["ANTHROPIC_API_KEY"] = "sk-..."

model = ChatAnthropic(model="claude-sonnet-4-6")
agent = create_deep_agent(model=model)
```

👉 阅读 Azure 聊天模型集成文档
```bash
pip install -U "langchain[openai]"
```

```python
import os
from deepagents import create_deep_agent

os.environ["AZURE_OPENAI_API_KEY"] = "..."
os.environ["AZURE_OPENAI_ENDPOINT"] = "..."
os.environ["OPENAI_API_VERSION"] = "2025-03-01-preview"

agent = create_deep_agent(model="azure_openai:gpt-5.5")
# 内部调用 init_chat_model，使用默认参数
# 自定义参数请直接使用 init_chat_model
```

```python
import os
from langchain.chat_models import init_chat_model
from deepagents import create_deep_agent

os.environ["AZURE_OPENAI_API_KEY"] = "..."
os.environ["AZURE_OPENAI_ENDPOINT"] = "..."
os.environ["OPENAI_API_VERSION"] = "2025-03-01-preview"

model = init_chat_model(
 model="azure_openai:gpt-5.5",
 azure_deployment=os.environ["AZURE_OPENAI_DEPLOYMENT_NAME"],
)
agent = create_deep_agent(model=model)
```

```python
import os
from langchain_openai import AzureChatOpenAI
from deepagents import create_deep_agent

os.environ["AZURE_OPENAI_API_KEY"] = "..."
os.environ["AZURE_OPENAI_ENDPOINT"] = "..."
os.environ["OPENAI_API_VERSION"] = "2025-03-01-preview"

model = AzureChatOpenAI(
 model="gpt-5.5",
 azure_deployment=os.environ["AZURE_OPENAI_DEPLOYMENT_NAME"],
)
agent = create_deep_agent(model=model)
```

👉 阅读 Google GenAI 聊天模型集成文档
```bash
pip install -U "langchain[google-genai]"
```

```python
import os
from deepagents import create_deep_agent

os.environ["GOOGLE_API_KEY"] = "..."

agent = create_deep_agent(model="google_genai:gemini-3.6-flash")
# 内部调用 init_chat_model，使用默认参数
# 自定义参数请直接调用 init_chat_model
```

```python
import os
from langchain.chat_models import init_chat_model
from deepagents import create_deep_agent

os.environ["GOOGLE_API_KEY"] = "..."

model = init_chat_model(model="google_genai:gemini-3.6-flash")
agent = create_deep_agent(model=model)
```

```python
import os
from langchain_google_genai import ChatGoogleGenerativeAI
from deepagents import create_deep_agent

os.environ["GOOGLE_API_KEY"] = "..."

model = ChatGoogleGenerativeAI(model="gemini-3.6-flash")
agent = create_deep_agent(model=model)
```

👉 阅读 AWS Bedrock 聊天模型集成文档
```bash
pip install -U "langchain[aws]"
```

```python
from deepagents import create_deep_agent

# 凭据配置步骤参考官方文档
# https://docs.aws.amazon.com/bedrock/latest/userguide/getting-started.html

agent = create_deep_agent(
 model="anthropic.claude-sonnet-4-6",
 model_provider="bedrock_converse",
)
# 内部调用 init_chat_model，使用默认参数
# 自定义参数直接调用 init_chat_model
```

```python
from langchain.chat_models import init_chat_model
from deepagents import create_deep_agent

# 凭据配置参考官方文档
# https://docs.aws.amazon.com/bedrock/latest/userguide/getting-started.html

model = init_chat_model(
 model="anthropic.claude-sonnet-4-6",
 model_provider="bedrock_converse",
)
agent = create_deep_agent(model=model)
```

```python
from langchain_aws import ChatBedrock
from deepagents import create_deep_agent

# 凭据配置参考官方文档
# https://docs.aws.amazon.com/bedrock/latest/userguide/getting-started.html

model = ChatBedrock(model="anthropic.claude-sonnet-4-6")
agent = create_deep_agent(model=model)
```

👉 阅读 HuggingFace 聊天模型集成文档
```bash
pip install -U "langchain[huggingface]"
```

```python
import os
from deepagents import create_deep_agent

os.environ["HUGGINGFACEHUB_API_TOKEN"] = "hf_..."

agent = create_deep_agent(
 model="microsoft/Phi-3-mini-4k-instruct",
 model_provider="huggingface",
 temperature=0.7,
 max_tokens=1024,
)
# 内部调用 init_chat_model，使用默认参数
# 自定义参数直接调用 init_chat_model
```

```python
import os
from langchain.chat_models import init_chat_model
from deepagents import create_deep_agent

os.environ["HUGGINGFACEHUB_API_TOKEN"] = "hf_..."

model = init_chat_model(
 model="microsoft/Phi-3-mini-4k-instruct",
 model_provider="huggingface",
 temperature=0.7,
 max_tokens=1024,
)
agent = create_deep_agent(model=model)
```

```python
import os
from langchain_huggingface import ChatHuggingFace, HuggingFaceEndpoint
from deepagents import create_deep_agent

os.environ["HUGGINGFACEHUB_API_TOKEN"] = "hf_..."

llm = HuggingFaceEndpoint(
 repo_id="microsoft/Phi-3-mini-4k-instruct",
 temperature=0.7,
 max_length=1024,
)
model = ChatHuggingFace(llm=llm)
agent = create_deep_agent(model=model)
```

你可以传入任意支持的模型字符串，也可以传入已经初始化好的模型实例：

| 模型字符串写法 | init_chat_model 写法 | 模型类实例写法 |
|---|---|---|
```python
from deepagents import create_deep_agent
agent = create_deep_agent(model="provider:model-name")
```
```python
from deepagents import create_deep_agent
from langchain.chat_models import init_chat_model
model = init_chat_model("provider:model-name")
agent = create_deep_agent(model=model)
```
```python
from langchain_<provider> import Chat<Provider>
from deepagents import create_deep_agent
model = Chat<Provider>(model="model-name")
agent = create_deep_agent(model=model)
```

聊天模型会自动对瞬时API故障做指数退避重试。关于默认配置、限制、`max_retries` / `timeout`调参示例，请查阅 LangChain Models 页面。

## 工具
除了用于文件管理、生成子代理的内置工具，你还可以提供自定义工具：

```python
import os
from typing import Literal
from tavily import TavilyClient
from deepagents import create_deep_agent

tavily_client = TavilyClient(api_key=os.environ["TAVILY_API_KEY"])

def internet_search(
 query: str,
 max_results: int = 5,
 topic: Literal["general", "news", "finance"] = "general",
 include_raw_content: bool = False,
):
 """执行网页搜索"""
 return tavily_client.search(
 query,
 max_results=max_results,
 include_raw_content=include_raw_content,
 topic=topic,
 )

agent = create_deep_agent(
 model="openai:gpt-5.5",
 tools=[internet_search],
)
```

### MCP 工具
Deep Agents 完整支持**模型上下文协议（Model Context Protocol, MCP）**工具。你可以从任意MCP服务加载工具（数据库、API、文件系统等），直接传给 `create_deep_agent`。

安装 `langchain‑mcp‑adapters` 连接MCP服务端：
```bash
pip install langchain-mcp-adapters
```

```python
import asyncio
from langchain_mcp_adapters.client import MultiServerMCPClient
from deepagents import create_deep_agent

async def main():
 async with MultiServerMCPClient(
 {
 "my_server": {
 "transport": "http",
 "url": "http://localhost:8000/mcp",
 }
 }
 ) as client:
 tools = await client.get_tools()

 agent = create_deep_agent(
 model="openai:gpt-5.5",
 tools=tools,
 )

 result = await agent.ainvoke(
 {"messages": [{"role": "user", "content": "Use the MCP server to help me."}]},
 config={"configurable": {"thread_id": "1"}},
 )

asyncio.run(main())
```

完整配置项（包括stdio服务、OAuth鉴权、工具过滤、有状态会话）请查阅 MCP 完整指南。

## 系统提示词
通过 `system_prompt=` 参数，为代理提供自定义指令：

```python
from deepagents import create_deep_agent

research_instructions = """\
你是一名专业研究员。你的工作是完成全面调研，然后输出一份格式精良的报告。
"""

agent = create_deep_agent(
 model="openai:gpt-5.5",
 system_prompt=research_instructions,
)
```

除字符串之外，主代理也可以接收带有结构化内容块的 `SystemMessage` 对象；Deep Agents 会完整保留这些内容块（子代理字典形式配置只能使用字符串）。

### 子代理提示词
声明式配置的子代理，会先基于自身模型解析配置集的覆盖规则，再将配置集的 `base_system_prompt` / `system_prompt_suffix` 应用到子代理的系统提示词。
- 如果配置集只设置了 `system_prompt_suffix`（Anthropic、OpenAI内置配置集的常见用法），该后缀会**追加**到你编写的提示词之后。
- 如果配置集设置了 `base_system_prompt`，会直接**替换**原有提示词。

### 通用子代理提示词
自动生成的通用子代理，基础提示词解析优先级：
`general_purpose_subagent.system_prompt`（如果设置）→ `HarnessProfile.base_system_prompt`（如果设置）→ SDK内置通用子代理默认提示词，之后再叠加配置集后缀。
当两项覆盖字段同时设置时，**通用子代理专属配置优先生效**，避免你的配置被静默丢弃。

```python
from deepagents import (
 GeneralPurposeSubagentProfile,
 HarnessProfile,
 register_harness_profile,
)

register_harness_profile(
 "anthropic",
 HarnessProfile(
 base_system_prompt="你是ACME的业务编排主代理。", # 主代理
 general_purpose_subagent=GeneralPurposeSubagentProfile(
 system_prompt="你是调研子代理，需要标注引用来源。", # 通用子代理
 ),
 system_prompt_suffix="必须逐步思考。",
 ),
)
```

| 角色 | 最终系统提示词 |
|---|---|
| 主代理 | `"你是ACME的业务编排主代理。" + 后缀` |
| 通用子代理 | `"你是调研子代理，需要标注引用来源。" + 后缀` |

## 中间件
Deep Agents 支持各类中间件：内置中间件、LangChain预构建中间件、服务商专属中间件，以及你自行编写的自定义中间件。
将中间件传入 `create_deep_agent` 的 `middleware` 参数，每个实例会合并进**默认调用栈**：
- 如果实例的 `.name` 和栈中已有中间件匹配，会就地替换原有实例；
- 不匹配的中间件会插入到 `PatchToolCallsMiddleware` 之后。

详见**覆盖默认中间件实例**章节。

### 主代理默认调用栈（执行顺序从前到后）
1. **SkillsMiddleware**：传入`skills`参数才启用。在文件系统中间件**之前**注入，保证文件工具执行前就可以获取技能元数据。
2. **FilesystemMiddleware**：处理读写文件、目录导航等文件系统操作。传入`permissions`时，权限校验在此中间件执行，拦截代理调用的全部工具。
3. **SubAgentMiddleware**：生成并协调子代理，把任务委派给专用代理。
4. **SummarizationMiddleware**：会话变长时压缩消息历史，避免超出上下文窗口（底层调用`create_summarization_middleware`）。
5. **PatchToolCallsMiddleware**：代理中断恢复、工具调用参数异常时，修复消息历史中残缺的工具调用。在Anthropic提示缓存以及后续栈逻辑**之前**运行。
6. **AsyncSubAgentMiddleware**：配置异步子代理时才启用。
7. **你传入的middleware参数**：你传入的自定义中间件，合并位置在Patch之后。名称匹配默认中间件会就地替换，否则追加到此位置。
8. **框架配置集扩展中间件**：解析后的模型配置集所携带的服务商专属中间件。
9. **排除工具过滤**：当配置集声明要排除某些工具，该中间件会把对应工具从代理可用列表移除。
10. **提示缓存**（`AnthropicPromptCachingMiddleware`、`BedrockPromptCachingMiddleware`）：始终注册，运行在Patch与自定义中间件之后，保证缓存前缀与实际发给模型的内容完全一致；对不支持缓存的模型自动空跑（`unsupported_model_behavior="ignore"`），即Anthropic中间件只作用于Anthropic模型，Bedrock中间件只作用于支持缓存的Bedrock模型。
11. **MemoryMiddleware**：传入`memory`参数才启用。放置在配置集扩展、提示缓存之后，避免内存内容更新导致缓存前缀失效；`create_deep_agent`源码注释中专门强调该顺序考量。
12. **HumanInTheLoopMiddleware**：传入`interrupt_on`才启用，在指定工具调用时暂停等待人工审批。

### 同步子代理默认调用栈
内置通用子代理，以及每个声明式同步`SubAgent`图，都由`create_deep_agent`内部构建调用栈。整体结构与主代理大体一致（文件系统、摘要、Patch、配置集扩展、Anthropic/Bedrock缓存、可选权限校验），但有两处区别：
1. 在子代理内部，**Skills 在 PatchToolCallsMiddleware 之后执行**（主代理中Skills运行在文件系统中间件之前）。
2. 子代理图内部**没有 SubAgentMiddleware**；只有父代理对外暴露`task`工具。

当声明式子代理配置了`interrupt_on`，该配置会转发给子代理的`create_agent`，完成指定工具调用的人工介入逻辑。

### 预构建中间件
LangChain提供更多开箱即用的预构建中间件，可实现重试、降级、PII检测等能力，查阅预构建中间件文档。
deepagents库同时提供`create_summarization_tool_middleware`，允许代理在合适时机（例如任务间隙）执行摘要，而不是固定按token阈值触发。详见**摘要处理**文档。

### 服务商专属中间件
针对特定大模型服务商做优化的中间件，请查阅**中间件集成**文档。

### 自定义中间件
你可以添加额外中间件来扩展能力、注入工具、实现自定义钩子：

```python
from langchain.agents.middleware import wrap_tool_call
from langchain.tools import tool
from deepagents import create_deep_agent

@tool
def get_weather(city: str) -> str:
 """获取城市天气"""
 return f"The weather in {city} is sunny."

call_count = [0] # 使用list，支持嵌套函数修改

@wrap_tool_call
def log_tool_calls(request, handler):
 """拦截并打印每一次工具调用，演示横切逻辑"""
 call_count[0] += 1
 tool_name = request.name if hasattr(request, "name") else str(request)

 print(f"[中间件] 工具调用 #{call_count[0]}: {tool_name}")
 print(f"[中间件] 参数: {request.args if hasattr(request, 'args') else 'N/A'}")

 # 执行工具调用
 result = handler(request)

 # 打印调用结果
 print(f"[中间件] 工具调用 #{call_count[0]} 执行完成")

 return result

agent = create_deep_agent(
 model="openai:gpt-5.5",
 tools=[get_weather],
 middleware=[log_tool_calls],
)
```

> ⚠️ 禁止在初始化后直接修改实例属性
> 如果你需要在多次钩子调用之间保存状态（计数器、累计数据），**请使用图状态**。图状态按线程隔离，在并发场景下是安全的。

✅ 正确示例：
```python
from langchain.agents.middleware import AgentMiddleware

class CustomMiddleware(AgentMiddleware):
 def __init__(self):
  pass

 def before_agent(self, state, runtime):
 return {"x": state.get("x", 0) + 1} # 更新图状态
```

❌ 错误示例，不要这样写：
```python
class CustomMiddlewareBad(AgentMiddleware):
 def __init__(self):
 self.x = 1

 def before_agent(self, state, runtime):
 self.x += 1 # 直接修改实例属性，会引发竞态条件
```

直接修改实例成员（例如在`before_agent`修改`self.x`）会产生隐蔽bug与竞态，因为子代理、并行工具、多线程请求会并发执行。
如果你必须使用可变对象，要充分考虑子代理、并行工具、多代理同时运行的场景。
关于扩展自定义状态字段，请查阅【自定义中间件 - 自定义状态Schema】。

### 覆盖默认中间件实例
> 通过匹配`.name`覆盖默认中间件，要求 `deepagents >= 0.7`

传入一个中间件实例，如果它的`.name`与默认调用栈中的某个中间件完全匹配，会**就地替换该默认中间件**，而不是新增一份副本。
传入的中间件如果name不匹配任何默认项，不会做替换，会追加到核心中间件之后、配置集、提示缓存、内存模块之前。查看【主代理默认调用栈】了解完整顺序。

```python
from deepagents import create_deep_agent
from deepagents.backends import StateBackend
from deepagents.middleware import SummarizationMiddleware

backend = StateBackend()
model = "openai:gpt-5.5"

custom_summarization = SummarizationMiddleware(
 model=model,
 backend=backend,
 summary_prompt="在此填写自定义摘要提示词。",
)

agent = create_deep_agent(
 model=model,
 middleware=[custom_summarization], # 替换默认的SummarizationMiddleware
)
```

> 覆盖是**完全替换实例**，而不是合并配置。
> 你传入的替换实例必须完整配置所有需要的参数。
> 这点对`FilesystemMiddleware`尤其重要：如果你覆盖它，你必须手动给实例传入`backend`（以及`permissions`，如需要），它不会继承`create_deep_agent`顶层传入的`backend=`与`permissions=`。
> 如需限制文件系统可用工具，可以给自定义`FilesystemMiddleware`设置`tools`白名单；参考文档【限制文件系统工具】。

自动添加的通用子代理，会继承主代理对默认中间件的覆盖，但不会继承主代理专属的中间件。
通过`subagents=`声明的子代理**不会继承主代理的中间件自定义配置**。需要在子代理自身的`middleware`字段传入覆盖配置；子代理内部会按照自己的默认调用栈做同样的name匹配逻辑。

#### 使用示例
##### 调整摘要触发时机
覆盖`SummarizationMiddleware`，自定义`trigger`触发阈值、`keep`保留消息数量，可以提前或延后压缩会话历史，控制每次压缩后保留多少条原始消息。

```python
from deepagents import create_deep_agent
from deepagents.backends import StateBackend
from deepagents.middleware import SummarizationMiddleware

backend = StateBackend()
model = "anthropic:claude-sonnet-4-6"

agent = create_deep_agent(
 model=model,
 middleware=[
 SummarizationMiddleware(
 model=model,
 backend=backend,
 trigger=("tokens", 100000), # token超过100000触发摘要
 keep=("messages", 20), # 完整保留最近20条消息
 ),
 ],
)
```

`trigger`同样支持`("fraction", ...)`，表示模型上下文窗口占比阈值；多个阈值列表是或的关系。完整选项参考`SummarizationMiddleware`API文档。

##### 修改提示缓存TTL
覆盖`AnthropicPromptCachingMiddleware`，延长缓存生命周期（默认5分钟TTL），适合对话间隔较长的代理场景。查阅【提示缓存】了解默认行为。

```python
from deepagents import create_deep_agent
from langchain_anthropic.middleware import AnthropicPromptCachingMiddleware

agent = create_deep_agent(
 model="anthropic:claude-sonnet-4-6",
 middleware=[
 AnthropicPromptCachingMiddleware(ttl="1h"), # 将默认5分钟替换为1小时TTL
 ],
)
```

##### 限制文件系统可用工具
> `FilesystemMiddleware`的tools白名单需要`deepagents >=0.7`

覆盖`FilesystemMiddleware`，设置`tools`白名单，只向模型暴露部分文件工具，而不是全套默认工具。
下面示例实现只读代理：禁用写文件、编辑、删除、执行命令。

```python
from deepagents import create_deep_agent
from deepagents.backends import StateBackend
from deepagents.middleware import FilesystemMiddleware

backend = StateBackend()

# 只读代理：write_file、edit_file、delete、execute不会暴露给模型
agent = create_deep_agent(
 model="anthropic:claude-sonnet-4-6",
 backend=backend,
 middleware=[
 FilesystemMiddleware(backend=backend, tools=["read_file", "ls", "glob", "grep"]),
 ],
)
```

更多内容参考【限制文件系统工具】。

### 解释器（Interpreters）
使用解释器中间件可以添加`eval`工具，在隔离的QuickJS运行时执行JavaScript代码。适合代理需要动态组装工具、批量处理任务、代码异常捕获、结构化数据转换，又不希望启用完整shell环境的场景。

```python
from deepagents import create_deep_agent
from langchain_quickjs import CodeInterpreterMiddleware

agent = create_deep_agent(
 model="openai:gpt-5.5",
 middleware=[CodeInterpreterMiddleware()],
)
```

关于环境部署、工具编程调用、子代理编排、资源限制，请查阅【解释器】文档。

## 子代理 Subagents
使用子代理把复杂任务拆解隔离，避免主代理上下文膨胀：

```python
import os
from typing import Literal

from deepagents import create_deep_agent
from tavily import TavilyClient

tavily_client = TavilyClient(api_key=os.environ["TAVILY_API_KEY"])

def internet_search(
 query: str,
 max_results: int = 5,
 topic: Literal["general", "news", "finance"] = "general",
 include_raw_content: bool = False,
):
 """执行网页搜索"""
 return tavily_client.search(
 query,
 max_results=max_results,
 include_raw_content=include_raw_content,
 topic=topic,
 )

research_subagent = {
 "name": "research-agent",
 "description": "用于深度调研问题",
 "system_prompt": "你是一名优秀的研究员",
 "tools": [internet_search],
 "model": "openai:gpt-5.5", # 可选，不填则继承主代理模型
}
subagents = [research_subagent]

agent = create_deep_agent(
 model="google_genai:gemini-3.6-flash",
 subagents=subagents,
)
```

更多内容请查阅【子代理】文档。

## 后端 Backends
Deep Agent工具可以使用虚拟文件系统存储、读取、修改文件。默认使用`StateBackend`。
如果你使用`skills`或者`memory`，必须在创建代理之前，将对应的技能/内存文件预先放入后端。

后端实现列表：
- StateBackend
- FilesystemBackend
- LocalShellBackend
- StoreBackend
- ContextHubBackend
- CompositeBackend

**StateBackend**：线程作用域的文件系统后端，存储在LangGraph状态中。同一条对话线程内，跨轮次文件可以持久化（依赖checkpointer）；不同线程之间数据互不共享。

```python
from deepagents import create_deep_agent
from deepagents.backends import StateBackend

# 默认就是StateBackend
agent = create_deep_agent(model="openai:gpt-5.5")

# 底层等价写法
agent2 = create_deep_agent(
 model="openai:gpt-5.5",
 backend=StateBackend(),
)
```

**FilesystemBackend**：访问本机真实文件系统。
> 该后端授予代理直接读写本地文件权限，请谨慎使用，仅在合适环境启用。查阅`FilesystemBackend`文档。

```python
from deepagents import create_deep_agent
from deepagents.backends import FilesystemBackend

agent = create_deep_agent(
 model="openai:gpt-5.5",
 backend=FilesystemBackend(root_dir=".", virtual_mode=True),
)
```

> 最佳实践：用`CompositeBackend`包装`FilesystemBackend`，避免代理内部数据（工具返回结果、会话历史）与项目业务文件混写在磁盘上，参考推荐模式。

**LocalShellBackend**：文件系统能力 + 在宿主机执行shell命令的`execute`工具。
> ⚠️该后端授予代理读写文件以及无限制宿主机shell执行权限，请极度谨慎，仅在受控环境启用。查阅`LocalShellBackend`文档。

```python
from deepagents import create_deep_agent
from deepagents.backends import LocalShellBackend

agent = create_deep_agent(
 model="openai:gpt-5.5",
 backend=LocalShellBackend(root_dir=".", virtual_mode=True, env={"PATH": "/usr/bin:/bin"}),
)
```

**StoreBackend**：跨线程持久存储的文件系统。

```python
from deepagents import create_deep_agent
from deepagents.backends import StoreBackend
from langgraph.store.memory import InMemoryStore

agent = create_deep_agent(
 model="openai:gpt-5.5",
 backend=StoreBackend(
 namespace=lambda rt: (rt.server_info.user.identity,),
 ),
 store=InMemoryStore(), # 本地开发使用；部署到LangSmith平台时省略
)
```

部署到LangSmith Deployment时，请省略`store`参数，平台会自动提供存储。
`namespace`参数控制数据隔离。多用户部署务必配置namespace工厂，实现按用户/租户数据隔离。

**ContextHubBackend**：将文件持久化存储在LangSmith Hub仓库。

```python
from deepagents import create_deep_agent
from deepagents.backends import ContextHubBackend

agent = create_deep_agent(
 model="openai:gpt-5.5",
 backend=ContextHubBackend("my-agent"),
)
```

查阅`ContextHubBackend`文档获取详情。

**CompositeBackend**：灵活的复合后端，可以将文件系统不同路径路由到不同后端实例。

```python
from deepagents import create_deep_agent
from deepagents.backends import CompositeBackend, StateBackend, StoreBackend
from langgraph.store.memory import InMemoryStore

agent = create_deep_agent(
 model="openai:gpt-5.5",
 backend=CompositeBackend(
 default=StateBackend(),
 routes={
 "/memories/": StoreBackend(namespace=lambda _rt: ("memories",)),
 },
 ),
 store=InMemoryStore(), # store传给create_deep_agent，而不是backend
)
```

更多内容查阅【后端 Backends】文档。

### 沙箱 Sandboxes
沙箱是一类特殊后端，代理代码运行在隔离环境，自带独立文件系统，提供`execute`工具执行shell命令。
当你希望代理写文件、安装依赖、运行命令，但又不想改动本机环境，就使用沙箱后端。
使用时把沙箱后端传给`create_deep_agent`的`backend`参数。

支持沙箱服务商：LangSmith、Daytona、E2B、Modal、Runloop、Vercel

```bash
pip install "langsmith[sandbox]"
# uv add "langsmith[sandbox]"
```

```python
from deepagents import create_deep_agent
from deepagents.backends import LangSmithSandbox
from langchain_anthropic import ChatAnthropic
from langsmith.sandbox import SandboxClient

client = SandboxClient()
ls_sandbox = client.create_sandbox()
backend = LangSmithSandbox(sandbox=ls_sandbox)

agent = create_deep_agent(
 model=ChatAnthropic(model="claude-sonnet-4-6"),
 system_prompt="你是拥有沙箱权限的Python编码助手。",
 backend=backend,
)
try:
 result = agent.invoke(
 {
 "messages": [
 {
 "role": "user",
 "content": "创建一个小型Python包并运行pytest",
 }
 ]
 }
 )
finally:
 client.delete_sandbox(ls_sandbox.name)
```

其余沙箱（Daytona/E2B/Modal/Runloop/Vercel）的安装包与示例代码请查阅原文【沙箱】章节。更多细节查看【沙箱 Sandboxes】文档。

## 人工介入 Human‑in‑the‑loop
部分工具操作比较敏感，需要人工审批之后才允许执行。你可以为每个工具单独配置审批规则。

> ⚠️ **必须配置 checkpointer 才能启用人工介入**

```python
from langchain.tools import tool
from deepagents import create_deep_agent
from langgraph.checkpoint.memory import MemorySaver

@tool
def remove_file(path: str) -> str:
 """从文件系统删除文件"""
 return f"Deleted {path}"

@tool
def fetch_file(path: str) -> str:
 """读取文件内容"""
 return f"Contents of {path}"

@tool
def notify_email(to: str, subject: str, body: str) -> str:
 """发送邮件"""
 return f"Sent email to {to}"

# checkpointer 是人工介入功能必需组件
checkpointer = MemorySaver()

agent = create_deep_agent(
 model="openai:gpt-5.5",
 tools=[remove_file, fetch_file, notify_email],
 interrupt_on={
 "remove_file": True, # 默认：批准、编辑参数、拒绝、回复消息
 "fetch_file": False, # 不拦截
 "notify_email": {"allowed_decisions": ["approve", "reject"]}, # 仅允许批准/拒绝，不允许编辑参数
 },
 checkpointer=checkpointer, # 必填！
)
```

你既可以在代理、子代理层面配置工具调用中断，也可以在工具内部触发中断。查阅【人工介入】文档。

## Skills 技能
使用技能为Deep Agent赋予新能力与专业知识。
- **Tools工具**偏向底层能力，例如原生文件操作；
- **Skills技能**包含完成任务的详细指引、参考资料、模板等资产。

技能文件不会在代理启动时全部加载；代理判断当前用户问题需要该技能时才会读取，这种按需加载可以降低启动时token消耗、减少上下文负担。
查看【Deep Agents示例技能】获取样例。

给Deep Agent添加技能，直接把目录路径传给`create_deep_agent`的`skills`参数。
支持后端：StateBackend、StoreBackend、FilesystemBackend。

> StateBackend示例，向虚拟文件系统注入技能文件：
```python
from urllib.request import urlopen
from deepagents import create_deep_agent
from deepagents.backends import StateBackend
from deepagents.backends.utils import create_file_data
from langgraph.checkpoint.memory import MemorySaver

checkpointer = MemorySaver()
backend = StateBackend()

skill_url = "https://raw.githubusercontent.com/langchain-ai/deepagents/refs/heads/main/libs/cli/examples/skills/langgraph-docs/SKILL.md"
with urlopen(skill_url) as response:
 skill_content = response.read().decode('utf-8')

skills_files = {
 "/skills/langgraph-docs/SKILL.md": create_file_data(skill_content),
}

agent = create_deep_agent(
 model="openai:gpt-5.5",
 backend=backend,
 skills=["/skills/"],
 checkpointer=checkpointer,
)

result = agent.invoke(
 {
 "messages": [{"role": "user", "content": "What is langgraph?"}],
 # 向StateBackend虚拟文件系统写入文件；虚拟路径必须以 / 开头
 "files": skills_files,
 },
 config={"configurable": {"thread_id": "12345"}},
)
```

StoreBackend、FilesystemBackend的使用样例请查阅原文Skills章节。

## Memory 内存
使用`AGENTS.md`文件为Deep Agent提供附加上下文。创建代理时，将一个或多个文件路径传入`memory`参数。
支持后端：StateBackend、StoreBackend、FilesystemBackend。

> StateBackend示例，注入AGENTS.md到虚拟文件系统：
```python
from urllib.request import urlopen

from deepagents import create_deep_agent
from deepagents.backends.utils import create_file_data
from langgraph.checkpoint.memory import MemorySaver

with urlopen(
 "https://raw.githubusercontent.com/langchain-ai/deepagents/refs/heads/main/examples/text-to-sql-agent/AGENTS.md"
) as response:
 agents_md = response.read().decode("utf-8")
checkpointer = MemorySaver()

agent = create_deep_agent(
 model="openai:gpt-5.5",
 memory=[
 "/AGENTS.md"
 ],
 checkpointer=checkpointer,
)

result = agent.invoke(
 {
 "messages": [
 {
 "role": "user",
 "content": "Please tell me what's in your memory files.",
 }
 ],
 # 向StateBackend虚拟文件系统注入文件，路径以 / 开头
 "files": {"/AGENTS.md": create_file_data(agents_md)},
 },
 config={"configurable": {"thread_id": "123456"}},
)
```

StoreBackend、FilesystemBackend使用样例请查阅原文Memory章节。

## Profiles 配置集
**框架配置集（harness profile）**是一套可复用、按模型绑定的配置集合，当选中对应模型时，`create_deep_agent`会自动加载它。
如果你希望行为跟随模型而不是代码调用位置，就使用Profiles。例如：针对Claude指令风格定制的系统提示词后缀；重写适配GPT的工具描述；只对特定服务商生效的额外中间件。

一份配置集可以包含：自定义基础系统提示词`base_system_prompt`、追加后缀`system_prompt_suffix`、工具描述重写、要排除的工具或中间件、注入额外中间件、修改自动生成的通用子代理。

```python
from deepagents import HarnessProfile, register_harness_profile

# 只要选中 openai:gpt‑5.5，就自动追加系统提示词后缀
register_harness_profile(
 "openai:gpt-5.5",
 HarnessProfile(system_prompt_suffix="回复控制在100词以内。"),
)
```

查阅【Profiles】文档，了解注册key规则、配置合并逻辑、插件打包。
配套的【服务商配置集】API可以封装模型初始化参数（API密钥、超时、重试等）。

## 结构化输出 Structured output
Deep Agents原生支持结构化输出。调用`create_deep_agent`时传入`response_format`指定输出Schema。模型生成结构化数据后，会做校验，结果存放在代理状态的`structured_response`字段中。

```python
import os
from typing import Literal

from pydantic import BaseModel, Field
from tavily import TavilyClient

from deepagents import create_deep_agent

tavily_client = TavilyClient(api_key=os.environ["TAVILY_API_KEY"])

def internet_search(
 query: str,
 max_results: int = 5,
 topic: Literal["general", "news", "finance"] = "general",
 include_raw_content: bool = False,
):
 """网页搜索"""
 return tavily_client.search(
 query,
 max_results=max_results,
 include_raw_content=include_raw_content,
 topic=topic,
 )

class WeatherReport(BaseModel):
 """结构化天气报告，包含当前实况与预报"""
 location: str = Field(description="这份天气报告对应的地点")
 temperature: float = Field(description="当前摄氏温度")
 condition: str = Field(
 description="当前天气状况（例如晴天、多云、下雨）"
 )
 humidity: int = Field(description="湿度百分比")
 wind_speed: float = Field(description="风速 km/h")
 forecast: str = Field(description="未来24小时简短预报")

agent = create_deep_agent(
 model="openai:gpt-5.5",
 response_format=WeatherReport,
 tools=[internet_search],
)

result = agent.invoke(
 {
 "messages": [
 {
 "role": "user",
 "content": "What's the weather like in San Francisco?",
 }
 ]
 }
)
print(result["structured_response"])
# location='San Francisco, California' temperature=18.3 condition='Sunny' humidity=48 wind_speed=7.6 forecast='Pleasant sunny conditions expected to continue with temperatures around 64°F (18°C) during the day, dropping to around 52°F (11°C) at night. Clear skies with minimal precipitation expected.'
```
