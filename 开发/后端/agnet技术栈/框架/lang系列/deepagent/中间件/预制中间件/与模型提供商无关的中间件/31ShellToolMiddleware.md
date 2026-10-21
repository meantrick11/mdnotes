# ShellToolMiddleware

想Agent暴露一个车就话的Shell会话以执行命令。Shell工具中间件适用于以下场景：
- 需要执行系统命令的agent
- 开发和部署自动化任务
- 测试与验证工作流
- 文件系统操作和脚本执行

## 参数配置

workspace_root:str|Path|None

Shell会话的基础陌路。如果省略，agent启动时会创建一个临时目录，并在agent结束时将其删除

start_commands:tuple[str,...]|list[str]|str|None

会话启动后按顺序执行的可选命令

shutdown_commands:tuple[str,...]|list[str]|str|None

会话关闭前的执行可选命令

execution_policy:BaseExecutionPolicy|None
控制超时、输出限制和资源配置的执行策略。可选值：
- HostExecutionPolicy：完全主机访问（默认）；最适合代理已在容器或虚拟机中运行的可信环境
- DockerExecutionPolicy：为每次代理运行启动一个独立的Docker容器，提供更强的隔离性
- CodexSandboxExecutionPolicy：重用 Codex CLI 沙箱以实现额外的系统调用/文件系统限制

redaction_rules:tuple[RedactionRule, ...]|list[RedactionRule]|None
用于在将命令输出返回给模型之前对其进行清理的可选脱敏规则。脱敏规则在执行后才会生效，使用HostExecutionPolicy时无法阻止机密或敏感数据的泄露

tool_description:str|None
可选覆盖已注册的 shell 工具描述

shell_command:Sequence[str]|str|None
用于启动持久会话的可选 shell 可执行文件或参数序列。默认为 /bin/bash

env:Mapping[str, Any]|None
要提供给 shell 会话的可选环境变量。在执行命令前，值会被强制转换为字符串

## 示例

```python
from langchain.agents import create_agent
from langchain.agents.middleware import (
    ShellToolMiddleware,
    HostExecutionPolicy,
    DockerExecutionPolicy,
    RedactionRule,
)


# Basic shell tool with host execution
agent = create_agent(
    model="gpt-5.5",
    tools=[search_tool],
    middleware=[
        ShellToolMiddleware(
            workspace_root="/workspace",
            execution_policy=HostExecutionPolicy(),
        ),
    ],
)

# Docker isolation with startup commands
agent_docker = create_agent(
    model="gpt-5.5",
    tools=[],
    middleware=[
        ShellToolMiddleware(
            workspace_root="/workspace",
            startup_commands=["pip install requests", "export PYTHONPATH=/workspace"],
            execution_policy=DockerExecutionPolicy(
                image="python:3.11-slim",
                command_timeout=60.0,
            ),
        ),
    ],
)

# With output redaction (applied post execution)
agent_redacted = create_agent(
    model="gpt-5.5",
    tools=[],
    middleware=[
        ShellToolMiddleware(
            workspace_root="/workspace",
            redaction_rules=[
                RedactionRule(pii_type="api_key", detector=r"sk-[a-zA-Z0-9]{32}"),
            ],
        ),
    ],
)
```
