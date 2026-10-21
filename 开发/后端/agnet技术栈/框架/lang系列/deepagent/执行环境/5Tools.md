# Tools 工具

Deep Agent的工具传入和调用和langchain没有本质区别，同样是传入@tool构造的工具或者直接的定义的函数都可以，但是在create_deep_agent和langchain一样，都会先转换为**标准的Tool对象**
1. 使用@tool方法构建模型会自动转换为标准的Tool对象：
    它自动帮你做 4 件事：
    - 提取函数名 → 作为工具名
    - 提取文档字符串（docstring）→ 作为工具描述（给大模型看的）
    - 提取函数参数 → 作为工具入参格式
    - 包装成 StructuredTool / BaseTool 实例
```python
from langchain_core.tools import tool

@tool
def add(a: int, b: int) -> int:
    """加法计算工具，输入两个数字返回和"""
    return a + b
#和以下代码等价
from langchain_core.tools import StructuredTool

def add_func(a: int, b: int):
    return a + b

add = StructuredTool.from_function(
    func=add_func,
    name="add",
    description="加法计算工具，输入两个数字返回和"
)
```


2. 直接子类继承BaseTool：自己写一个类，实现 _run，手动写 name /description/args_schema。
```python
class AddTool(BaseTool):
    name="add"
    description="加法"
    args_schema=AddInputSchema
    def _run(self,a,b):
        return a+b
```

3. langchian v1.0+支持直接传入普通函数，其中自动进行隐式转换


## 自定义工具

自定义工具就是上面所讲的方法构造的工具，然后传入langchain的create_agent的tool参数，会连接到agent上
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
    """Run a web search"""
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



## MCP工具

MCP 是一种开放协议，它允许智能体通过标准接口连接到不断扩展的服务器生态系统——包括数据库、API、文件系统、浏览器等各类服务。可以理解为agent的专属RESTful协议标准

不过需要先安装langchain[mcp]

```python
pip install "langchain[mcp]"
```
```python
import asyncio

from deepagents import create_deep_agent
from langchain.mcp import MCPAdapter


async def main():
    config = {"mcpServers": {"my_server": {"url": "http://localhost:8000/mcp"}}}
    async with MCPAdapter(config) as adapter:
        tools = await adapter.list_tools()
        agent = create_deep_agent(
            model="openai:gpt-5.5",
            tools=tools,
        )
        await agent.ainvoke(
            {
                "messages": [
                    {"role": "user", "content": "Use the MCP server to help me."}
                ]
            },
            config={"configurable": {"thread_id": "1"}},
        )
```


## 内置工具

除了上述的自定义+MCP工具之外，在调用create_deep_agent的时候框架会自动注册一些内置工具比如：

| Tool 工具 | Description 描述 |
| ---- | ---- |
| ls | 列出目录中的文件。 |
| read_file | 读取文件内容（支持分页和多模态）。 |
| write_file | 创建新文件，或覆盖现有文件。 |
| edit_file | 在文件中执行精确的字符串替换。 |
| delete | 删除文件或目录及其所有内容。`delete` 工具需要 `deepagents>=0.7`。 |
| glob | 查找匹配 glob 模式的文件 |
| grep | 搜索文件内容 |
| execute | 运行 shell 命令（仅适用于沙箱后端） |
| task | 生成一个子智能体来处理委派的任务。 |

## 多模态工具输出

当然agent支持多模态工具输出，这里不过多讲解

