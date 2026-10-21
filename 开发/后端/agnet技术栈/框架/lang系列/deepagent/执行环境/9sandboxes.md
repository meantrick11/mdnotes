# Sandboxes

智能体生成代码、与文件系统交互并运行 Shell 命令。由于无法预测智能体的行为，因此其环境必须保持隔离，以防它访问凭证、文件或网络。沙箱通过在智能体的执行环境与主机系统之间建立边界，实现这种隔离。


在深度智能体中，沙箱是后端，用于定义智能体运行的环境。与仅暴露文件操作的其他后端（状态、文件系统、存储）不同，沙箱后端还会为智能体提供execute工具，用于运行 Shell 命令。

**沙箱后端是在一个安全的“笼子”里执行命令，而 LocalShellBackend 是让智能体直接在你的电脑上执行命令。**


## 什么是沙箱？

沙箱用于安全目的。它允许代理执行任意代码、访问文件以及使用网络，而不会泄露你的凭据、本地文件或主机系统。当代理自主运行时，这种隔离至关重要。

1. 编码智能体：可自主运行的智能体可以使用 shell、git 克隆代码仓库（许多供应商提供原生 git 应用程序接口，例如 Daytona 的 git 操作），并通过 Docker-in-Docker 来执行构建和测试流水线
2. 数据分析智能体：在安全、隔离的环境中加载文件、安装数据分析库（pandas、numpy 等）、执行统计计算，并生成 PowerPoint 演示文稿等输出内容


## 基础用法

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
    system_prompt="You are a Python coding assistant with sandbox access.",
    backend=backend,
)
try:
    result = agent.invoke(
        {
            "messages": [
                {
                    "role": "user",
                    "content": "Create a small Python package and run pytest",
                }
            ]
        }
    )
finally:
    client.delete_sandbox(ls_sandbox.name)

```

## 生命周期和作用域


大多数应用程序要么选择每个线程一个沙箱（线程作用域），要么为同一助手上的所有线程共享一个沙箱（助手作用域）。

沙箱会消耗资源并产生费用，直到其被关闭。请确保在沙箱不再使用时将其关闭。

## 线程作用域（默认）

每次对话都会拥有独立的沙箱。首次运行时会创建沙箱，同一对话线程的后续交互会复用该沙箱。当对话线程结束或沙箱的生存时间（TTL）过期时，对应的运行环境会随之销毁。请将沙箱名称或元数据相关的映射关系存储下来，例如如下示例所示，确保每次运行都能对应到同一个沙箱。
```python
from deepagents import create_deep_agent
from deepagents.backends.langsmith import LangSmithSandbox
from langchain_core.runnables import RunnableConfig
from langsmith.sandbox import SandboxClient

client = SandboxClient()


async def agent(config: RunnableConfig):
    thread_id = config["configurable"]["thread_id"]
    sandbox_name = f"thread-{thread_id}"
    existing = [
        sb
        for sb in client.list_sandboxes()
        if getattr(sb, "name", None) == sandbox_name
    ]
    if existing:
        ls_sandbox = existing[0]
    else:
        ls_sandbox = client.create_sandbox(
            name=sandbox_name,
            idle_ttl_seconds=3600,  # TTL: clean up when idle
        )
    return create_deep_agent(
        model="openai:gpt-5.5",
        backend=LangSmithSandbox(sandbox=ls_sandbox),
    )
```

## agent作用域

同一个智能体下的每个线程都会复用一个沙箱。文件、已安装的包以及克隆的仓库会在多次对话中持续存在。

```python
from deepagents import create_deep_agent
from deepagents.backends.langsmith import LangSmithSandbox
from langchain_core.runnables import RunnableConfig
from langsmith.sandbox import SandboxClient

client = SandboxClient()


async def agent(config: RunnableConfig):
    assistant_id = config["configurable"]["assistant_id"]
    sandbox_name = f"assistant-{assistant_id}"
    existing = [
        sb
        for sb in client.list_sandboxes()
        if getattr(sb, "name", None) == sandbox_name
    ]
    if existing:
        ls_sandbox = existing[0]
    else:
        ls_sandbox = client.create_sandbox(name=sandbox_name)
    return create_deep_agent(
        model="openai:gpt-5.5",
        backend=LangSmithSandbox(sandbox=ls_sandbox),
    )
```

## 集成模式和沙箱模式


## 沙箱的工作原理

所有沙箱提供程序都会保护你的主机系统免受agent的文件系统和 shell 操作的影响。代理无法读取你的本地文件、访问你计算机上的环境变量或干扰其他进程。然而，仅靠沙箱并不能防范以下情况：

1. 上下文注入

2. 网络数据窃取：除非网络访问被阻止，否则注入上下文的agent可通过 HTTP 或 DNS 将数据发送出沙箱。部分服务商支持阻止网络访问（例如，在 Modal 上设置 blockNetwork: true）。

### execute方法

沙箱后端拥有简单的架构：提供方唯一需要实现的方法是 execute()，该方法会运行 shell 命令并返回其输出结果。

