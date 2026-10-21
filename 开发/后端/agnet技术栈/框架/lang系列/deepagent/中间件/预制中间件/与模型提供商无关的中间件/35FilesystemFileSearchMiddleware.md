# FilesystemFileSearchMiddleware

在文件系统上提供Glob和Grep搜索工具，主要适用于如下场景：

- 代码探索与分析
- 按文件名模式查找文件
- 适用正则表达式搜索代码内容
- 需要进行文件检索的代行代码



通常不需要同时使用，因为 FilesystemMiddleware 已经包含了 glob 和 grep 搜索工具。

但在以下特定场景中，FilesystemFileSearchMiddleware 的价值会凸显：

只需要搜索，不需要读写：如果你只希望 Agent 具备查找文件的能力，而不想赋予它修改文件系统的权限，单独使用 FilesystemFileSearchMiddleware 是更轻量、更安全的选择。

需要独立的搜索配置：FilesystemFileSearchMiddleware 提供了独立的 root_path 和 use_ripgrep 参数，允许你对搜索行为进行更细粒度的控制（例如指定特定的搜索根目录或强制使用 ripgrep）。

与状态文件搜索配合：在某些架构中，搜索功能可能需要独立于文件操作中间件进行配置，以便与 StateFileSearchMiddleware（用于搜索状态中的虚拟文件）等其他搜索中间件协同工作。

**FileSystemMiddleware的提供更多文件操作，属于子集关系**
## 参数配置

root_path str

**必填**，提供搜索根目录，所有文件搜索操作基于此目录

use_ripgre:bool=True
是否适用ripgrep进行搜索。如果ripgrep不可用则退回到Python正则表达式

max_file_size_mb:int=10

要搜索的最大文件大小。超过此文件大小的文件将被跳过。


## 示例

```python
from langchain.agents import create_agent
from langchain.agents.middleware import FilesystemFileSearchMiddleware
from langchain.messages import HumanMessage


agent = create_agent(
    model="gpt-5.5",
    tools=[],
    middleware=[
        FilesystemFileSearchMiddleware(
            root_path="/workspace",
            use_ripgrep=True,
            max_file_size_mb=10,
        ),
    ],
)

# Agent can now use glob_search and grep_search tools
result = agent.invoke({
    "messages": [HumanMessage("Find all Python files containing 'async def'")]
})

# The agent will use:
# 1. glob_search(pattern="**/*.py") to find Python files
# 2. grep_search(pattern="async def", include="*.py") to find async functions
```