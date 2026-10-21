# 可视化Pipelines

你可以将管道可视化为图形，以便更好地理解各组件的连接方式。

Haystack 管道具有 draw() 和 show() 方法，借助这些方法，你可以使用 Mermaid 图表将管道以图形形式可视化。

**使用管道可视化功能时，请谨慎处理敏感数据。**

## 前提条件
要使用 Mermaid 图表，你必须连接互联网，才能访问位于 https://mermaid.ink 的 Mermaid 图表渲染器。

## 显示图表

使用管道的show()方法在Jupyter笔记本中显示图表。

`my_pipeline.show()`

## 保存图表

使用管道的draw()方法，传入你想要保存图表的路径以及图表格式。可用格式包括：mermaid-text和mermaid-image（默认）。

`my_pipeline.draw(path=local_path)`

## 可视化超级组件

如果Pipelines中包含超级组件，且想要其显示出来而非黑盒组件，那么将super_component_expansion参数设置为True：
```py
my_pipeline.show(super_component_expansion=True)

# or

my_pipeline.draw(path=local_path, super_component_expansion=True)
```
## 本地可视化

如果你没有网络连接，或者不想将你的流水线数据发送到远程 https://mermaid.ink，你可以安装本地的 mermaid.ink 服务器并使用它来渲染你的流水线。

们将从https://github.com/jihchi/mermaid.ink/pkgs/container/mermaid.ink获取 mermaid.ink 的官方 Docker 镜像来运行一个本地的 mermaid.ink 服务器。
在这种情况下，我们为搭载 MacOS M3 芯片的系统安装一个本地服务器，并将其暴露在 3000 端口：

`docker run --platform linux/amd64 --publish 3000:3000 --cap-add=SYS_ADMIN ghcr.io/jihchi/mermaid.ink`

访问http://localhost:3000/，检查本地 mermaid.ink 服务器是否正在运行。

你应该会看到一个本地服务器正在运行，现在你只需在调用show()或 draw() 方法时指定 URL，即可使用本地的 mermaid.ink 服务器渲染图片：

```python
my_pipeline.show(server_url="http://localhost:3000")
# or
my_pipeline.draw("my_pipeline.png", server_url="http://localhost:3000")
```

## 示例

```mermaid
flowchart TD
    start["*"]

    start -->|query str| retriever["retriever<br/>MemoryBM25Retriever<br/><br/>Optional Inputs:<br/>• filters (Optional[Dict[str, Any]])<br/>• top_k (Optional[int])<br/>• scale_score (Optional[bool])"]
    start -->|question Any| prompt_builder["prompt_builder<br/>PromptBuilder"]
    start -->|query str| answer_builder["answer_builder<br/>AnswerBuilder<br/><br/>Optional Inputs:<br/>• metadata (Optional[List[Dict[str, Any]]])<br/>• documents (Optional[List[Document]])<br/>• pattern (Optional[str])<br/>• reference_pattern (Optional[str])"]

    retriever -->|documents -> documents<br/>List[Document]| prompt_builder

    prompt_builder -->|prompt -> prompt<br/>str| llm["llm<br/>GPTGenerator"]

    llm -->|replies -> replies<br/>List[str]| answer_builder

    answer_builder -->|answers<br/>List[GeneratedAnswer]| out((output))
```