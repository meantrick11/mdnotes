# FilesystemMiddleware

**上下文工程是构建高效agent的主要挑战。除了模型能力提升，最重要的就是上下文管理，尤其是长任务的时候**，在使用返回可变长度结果的工具（例如 web_search 和检索增强生成）时，这一点尤其困难，因为过长的工具结果会迅速占满上下文窗口。

FilesystemMiddleware来自Deep agent提供的四种用于交互短期/长期的工具

- ls，列出文件系统中的文件
- read_file,读取整个文件或从文件中读取指定行数的内容
- write_file，向文件系统中写入一个新文件
- edit_file，编辑文件系统中已存在的文件


## 参数配置/示例

这里的backend只是相较于文件系统而言。
```python
from langchain.agents import create_agent
from deepagents.middleware.filesystem import FilesystemMiddleware

# FilesystemMiddleware is included by default in create_deep_agent
# You can customize it if building a custom agent
agent = create_agent(
    model="claude-sonnet-4-6",
    middleware=[
        FilesystemMiddleware(
            backend=None,  # Optional: custom backend (defaults to StateBackend)
            system_prompt="Write to the filesystem when...",  # Optional custom addition to the system prompt
            custom_tool_descriptions={
                "ls": "Use the ls tool when...",
                "read_file": "Use the read_file tool to..."
            },  # Optional: Custom descriptions for filesystem tools
            tools=["read_file", "ls", "glob", "grep"],  # Optional: Allowlist restricting which filesystem tools are exposed
        ),
    ],
)
```




