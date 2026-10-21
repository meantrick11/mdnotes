# Skills 

技能包将**工作流、最佳实践、脚本、参考文档和模板等领域专业知识封装到可复用的目录中**。智能体在启动时会获取内容摘要，仅在相关时才查找并读取其中包含的文件。



## 使用方法

1. 创建一个目录用来存放项目的所有skills/
2. 在skills目录下存放技能子目录

每个技能都是一个包含SKILL.md文件的目录：这是一个带有YAML前置内容（name和description）的Markdown文件，后面是技能被激活时智能体需遵循的说明。技能目录还可选择性地包含脚本、参考文档和模板等辅助文件。

比如：
```
skills
|--langgraph-docs kasraph
    |--SKILL.md
    |--scripts
    |--references
    |--assets
```


通常，每个SKILL.md都有前置的yaml文字，SKILL.md通过**渐进性加载**的方式进行加载skill内容，比如首先加载的是头部yaml文件到首次对话的上下文，如果用户选择某一个skill，才会加载全量的skill到上下文，其次才是渐进性加载scripts、assets、reference等等内容，避免一下子将上下文窗口塞满。



**由于skill需要指定文件夹，所以需要指定的后端中包括能够覆盖到对应的skill文件夹**，比如：
```python
from deepagents import craete_deep_agent
from deepagents.backends.filesystem import FilesystemBackend

backend = FilesystemBackend(root_dir="./my_project")

agent = create_deep_agent(
    model = "anthropic:claude-sonnet-4-6"
    backend = backend
    skills = ["./my_project/skills/"]
)   #skills/文件夹能够被backend覆盖，无论是composite还是普通的filesystembackend
```

**注意事项**：

1. 将每个源路径指向一个包含技能目录的目录。直接指向带有 SKILL.md 的技能目录的路径不会被加载。
2. 路径**必须使用正斜杠指定，且相对于后端根目录**。
    - 如果省略，则不加载任何skills
    - 使用 StateBackend（默认）时，请通过 invoke(files={...}) 提供技能文件。请使用来自 deepagents.backends.utils 的 create_file_data() 来格式化文件内容；不支持原始字符串。
    - 使用 FilesystemBackend 和 StoreBackend 创建后端后，调用 backend.upload_files() 添加技能文件，然后将后端传递给 create_deep_agent。对于 FilesystemBackend，使用 virtual_mode=True 对 root_dir 下的路径进行沙箱隔离。root_dir 中已存在于磁盘的技能无需上传即可加载。

## skill的工作原理

skill不可能所有添加到系统提示词中，一个是因为浪费token且没有必要，大部分涉及需要加载其他内容的时候最好的方式就是**渐进性加载**，但是如何保证在间进行加载的过程中还能够在特定情况下激活对应的skill或者tool，就需要适当添加必要的提示词等等情况

技能分三个层级加载。每个层级仅在任务需要时才补充更多细节：

| Level | What loads | When |
| ----- | ---------- | ---- |
| 1. Metadata | 从 SKILL.md 的前置元数据(frontmatter)读取 name 和 description | Agent 启动时，为每一个已配置的技能加载 |
| 2. Instructions | SKILL.md 的全部正文内容 | 技能被调用时 |
| 3. Resources | scripts/、references/、assets/ 目录下的支持文件 | 调用之后，指令引用这些文件时按需加载 |


## 何时使用skill


如果你发现自己在向智能体下达类似的指令，尤其是这些指令很详细且包含多个步骤时，不妨考虑将这些指令整理成固定格式供智能体使用。这样一来，日后你想要完成类似任务时，智能体就会清楚该如何操作。

1. 分布工作流SOP，分支等等情况
2. 特定领域的知识：how to user tools for workflow
3. 可执行脚本
4. 资产assets等等


总共包括三个流程**发现-阅读-执行**
## 如何撰写实用的skill？

1. **保持前置内容简洁**：比如控制SKILL.md全文不超过5000tokens

2. **编写工具描述**:在发现阶段，description字段是agent查看每个skill时唯一能够获取的信息，所以尽可能保持描述的优质

3. **让指令保持简洁聚焦**：《智能体技能》规范建议将你的SKILL.md文件控制在500行以内。当指令篇幅变长时，可将详细的参考资料移至辅助资源文件中，并在主SKILL.md文件中对其进行引用

4. **管理技能数量**：少数几个范围明确的技能优于大量重叠的技能。随着描述相似的技能数量增加，智能体选择正确技能的能力会下降。如果你发现自己拥有许多相关技能，可以考虑：
    - 将相关能力整合为一个技能，并为每个子任务设置对应的章节
    - 使用参考文件让主SKILL.md保持简洁，同时涵盖多个子任务


## 辅助资源

除了SKILL.md之外，技能目录还可以包含任何其他文件或目录。智能体技能规范为常见的资源类型定义了三个可选目录。Deep Agents 在发现或激活时不会加载这些文件。只有当你的SKILL.md说明要求时，智能体才会读取或执行它们。

### scripts/

scripts/ 目录存放智能体可运行的可执行代码，例如 API 客户端、数据转换程序或验证检查工具。脚本应满足：

1. 保持自包含或记录依赖关系
2. 包含有用的错误提示信息
3. 处理边缘情况


### reference/

references/ 目录存放智能体按需读取的**补充文档**。请将内容过于详细、不适合放在 SKILL.md 中但仍与特定任务相关的资料放入其中，例如：

1. REFERENCE.md用于详细的技术参考
2. FROMS.md用于表单模板或者结构化数据格式
3. 领域特定指南（finance.md、legal.md 及类似文件）

### assets/

assets/ 目录存放智能体使用但无需作为指令读取的**静态资源**，例如：
1. 文档或配置模板
2. 图片、图表、示例等等
3. 数据文件


## 组合Skill文件夹

通过composite提供不同的后端，然后路由到不同后端时提供不同的skill文件夹

```python
from deepagents import create_deep_agent

agent = create_deep_agent(
    model="anthropic:claude-sonnet-4-6",
    skills=["/skills/org/", "/skills/team/", "/skills/request/"],
)
```

每个路径都可以指向同一个后端、不同的后端路由，或由运行时上下文限定的后端。

## 重新加载skills

**技能每个线程加载一次并保留在智能体状态中，因此该线程上之后的所有模型调用都会复用同一列表。**自那之后新增、编辑或删除的技能永远不会传递给模型。这适用于配置了检查点的情况。如果未配置检查点，状态无法在运行之间保留，因此每次运行都会重新加载技能。

只需要重新激活，并传入Skill的位置就可以起到刷新的作用了

```python
config = {"configurable": {"thread_id": "1"}}

result = agent.invoke(
    {
        "messages": [{"role": "user", "content": "What is LangGraph?"}],
        "skills_metadata": None,
    },
    config=config,
)
```
这里传入的时None，表示无任何skill内容

