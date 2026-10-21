## 调试Pipelines

你有多种方法可以调试你的管道

## 1. 查看组件输出

要查看特定管道组件的输出，请在执行管道时添加 include_outputs_from 参数。将其放在输入字典之后，并设置为你希望其输出包含在结果中的组件名称。

例如，下面展示了如何在该管道中打印PromptBuilder的输出：
```python
from haystack import Pipeline, Document
from haystack.utils import Secret
from haystack.components.generators.chat import OpenAIChatGenerator
from haystack.components.builders.chat_prompt_builder import ChatPromptBuilder
from haystack.dataclasses import ChatMessage

# Documents
documents = [
    Document(content="Joe lives in Berlin"),
    Document(content="Joe is a software engineer"),
]

# Define prompt template
prompt_template = [
    ChatMessage.from_system("You are a helpful assistant."),
    ChatMessage.from_user(
        "Given these documents, answer the question.\nDocuments:\n"
        "{% for doc in documents %}{{ doc.content }}{% endfor %}\n"
        "Question: {{query}}\nAnswer:",
    ),
]

# Define pipeline
p = Pipeline()
p.add_component(
    instance=ChatPromptBuilder(
        template=prompt_template,
        required_variables={"query", "documents"},
    ),
    name="prompt_builder",
)
p.add_component(
    instance=OpenAIChatGenerator(api_key=Secret.from_env_var("OPENAI_API_KEY")),
    name="llm",
)
p.connect("prompt_builder", "llm.messages")

# Define question
question = "Where does Joe live?"

# Execute pipeline
result = p.run(
    {"prompt_builder": {"documents": documents, "query": question}},
    include_outputs_from="prompt_builder",
)

# Print result
print(result)
```

## 日志

根据你的调试需求调整日志格式。

## 实时管道日志

使用 Haystack 的 LoggingTracer 日志可实时检查流经管道的数据。

该功能在实验和原型设计阶段特别实用，因为你无需预先设置任何追踪后端。

下面是启用此跟踪器的方法。在本示例中，我们将添加颜色标签（这是可选的）来突出显示组件名称和输入内容：

```python
import logging
from haystack import tracing
from haystack.tracing.logging_tracer import LoggingTracer

logging.basicConfig(
    format="%(levelname)s - %(name)s -  %(message)s",
    level=logging.WARNING,
)
logging.getLogger("haystack").setLevel(logging.DEBUG)

tracing.tracer.is_content_tracing_enabled = (
    True  # to enable tracing/logging content (inputs/outputs)
)
tracing.enable_tracing(
    LoggingTracer(
        tags_color_strings={
            "haystack.component.input": "\x1b[1;31m",
            "haystack.component.name": "\x1b[1;34m",
        },
    ),
)
```

## 追踪

要更全面地了解管道的性能，试试用Langfuse对其进行追踪。

我们的追踪页面详细介绍了适用于 Haystack 的其他追踪解决方案。