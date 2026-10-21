# 创建Pipelines

## 前提条件

**对于你想在管道中使用的每个组件，你必须了解其输入和输出的名称。**你可以在特定组件的文档页面或该组件的run()方法中查看这些信息。

## 创建管道的步骤

### 1. 导入依赖项

导入所有依赖项，例如管道、文档、文档存储以及你希望在管道中使用的所有组件。例如，要创建一个语义文档搜索管道，你需要Document对象、管道、文档存储、嵌入器和检索器：

`pip install sentence-transformers-haystack`

```python
from haystack import Document, Pipeline
from haystack.document_stores.in_memory import InMemoryDocumentStore
from haystack_integrations.components.embedders.sentence_transformers import (
    SentenceTransformersTextEmbedder,
)
from haystack.components.retrievers.in_memory import InMemoryEmbeddingRetriever
```

### 2. 初始化组件

初始化组件，并传入配置的参数

```python
document_store = InMemoryDocumentStore(embedding_similarity_function="cosine")
text_embedder = SentenceTransformersTextEmbedder()
retriever = InMemoryEmbeddingRetriever(document_store=document_store)
```

### 3. 创建管道

```python
query_pipeline = Pipeline()
```

### 4. 添加组件


向流水线中添加组件。添加组件的顺序无关紧要。你可以通过向add_components()传递一个字典，一次性添加多个组件：
```python
query_pipeline.add_components(
    {
        "text_embedder": text_embedder,
        "retriever": retriever,
    }
)
```

在添加任何内容之前，Haystack 会检查每个名称是否有效、每个值是否为 Haystack 组件实例、无名组件是否属于其他组件，以及没有实例已被分配给其他名称或管道。如果任何检查失败，管道将保持不变。

你也可以使用add_component()逐个添加组件：
```python
query_pipeline.add_component("component_name", component_type)

# Here is an example of how you'd add the components initialized in step 2 above:
query_pipeline.add_component("text_embedder", text_embedder)
query_pipeline.add_component("retriever", retriever)

# You could also add components without initializing them before:
query_pipeline.add_component("text_embedder", SentenceTransformersTextEmbedder())
query_pipeline.add_component(
    "retriever",
    InMemoryEmbeddingRetriever(document_store=document_store),
)
```
**在同一名称下再次添加完全相同的组件实例是无操作的。一个名称不能引用不同的组件，且一个组件实例不能同时对应多个名称或属于多个管道。**add_component() 和 add_components() 均会返回管道，因此你可以链式调用更多管道构建相关的方法。

### 5. 连接组件

**通过指明某个组件的哪个输出端应连接到下一个组件的输入端来连接各组件。**若某组件仅有一个输入或输出端，且连接关系显而易见，你可直接填写该组件名称，无需指明其输入或输出端。

要在一次调用中创建多个连接，请将 (sender, receiver) 对传递给 connect_many()：
```python
query_pipeline.connect_many(
    [
        ("text_embedder.embedding", "retriever"),
        ("retriever", "prompt_builder.documents"),
        ("prompt_builder", "llm"),
    ]
)
```

Haystack 按照提供的顺序建立连接。如果某一对无法连接，后续的配对将不会被处理，而之前成功建立的连接会保留在管道中。重复现有连接属于无操作指令。

**Haystack 按照提供的顺序建立连接。如果某一对无法连接，后续的配对将不会被处理，而之前成功建立的连接会保留在管道中。重复现有连接属于无操作指令。**

```python
# This is the syntax to connect components. Here you're connecting output1 of component1 to input1 of component2:
pipeline.connect("component1.output1", "component2.input1")

# If both components have only one output and input, you can just pass their names:
pipeline.connect("component1", "component2")

# If one of the components has only one output but the other has multiple inputs,
# you can pass just the name of the component with a single output, but for the component with
# multiple inputs, you must specify which input you want to connect

# Here, component1 has only one output, but component2 has multiple inputs:
pipeline.connect("component1", "component2.input1")

# And here's how it should look like for the semantic document search pipeline we're using as an example:
pipeline.connect("text_embedder.embedding", "retriever.query_embedding")
# Because the InMemoryEmbeddingRetriever only has one input, this is also correct:
pipeline.connect("text_embedder.embedding", "retriever")
```

你也可以单独连接各个组件。以下是我们正在组装的流水线的一个显式示例：

```python
# Imagine this pipeline has four components: text_embedder, retriever, prompt_builder and llm.
# Here's how you would connect them into a pipeline:

query_pipeline.connect("text_embedder.embedding", "retriever")
query_pipeline.connect("retriever", "prompt_builder.documents")
query_pipeline.connect("prompt_builder", "llm")

```

由于组件添加和连接方法会返回流水线，因此你可以流畅地构建流水线：

```python
query_pipeline = (
    Pipeline()
    .add_components(
        {
            "text_embedder": text_embedder,
            "retriever": retriever,
            "prompt_builder": prompt_builder,
            "llm": llm,
        }
    )
    .connect_many(
        [
            ("text_embedder.embedding", "retriever"),
            ("retriever", "prompt_builder.documents"),
            ("prompt_builder", "llm"),
        ]
    )
)
```

### 6. 运行管道

等待管道验证组件和连接。一切正常后，你现在就可以运行管道了。Pipeline.run() 有两种调用方式，一种是传入组件名称及其输入的字典，另一种是直接只传入输入。直接传入时，管道会将输入解析到对应的正确组件上。

```python
# Here's one way of calling the run() method
results = pipeline.run({"component1": {"input1_value": value1, "input2_value": value2}})

# The inputs can also be passed directly without specifying component names
results = pipeline.run({"input1_value": value1, "input2_value": value2})

# This is how you'd run the semantic document search pipeline we're using as an example:
query = "Here comes the query text"
results = query_pipeline.run({"text_embedder": {"text": query}})
```


**也就是说你可以从头开始运行整个管道，也可以通过从某一个组件开始运行后续的链路，因为组件本身就支持run**

#### 管道输入

如果你需要了解运行管道所需的组件输入，Haystack 提供了一个实用的管道函数 .inputs()，它会列出组件的所有必要输入。

```python
# A short pipeline example that converts webpages into documents
from haystack import Pipeline
from haystack.document_stores.in_memory import InMemoryDocumentStore
from haystack.components.fetchers import LinkContentFetcher
from haystack.components.converters import HTMLToDocument
from haystack.components.writers import DocumentWriter

document_store = InMemoryDocumentStore()
fetcher = LinkContentFetcher()
converter = HTMLToDocument()
writer = DocumentWriter(document_store=document_store)

pipeline = Pipeline()
pipeline.add_component(instance=fetcher, name="fetcher")
pipeline.add_component(instance=converter, name="converter")
pipeline.add_component(instance=writer, name="writer")

pipeline.connect("fetcher.streams", "converter.sources")
pipeline.connect("converter.documents", "writer.documents")

# Requesting a list of required inputs
pipeline.inputs()

# {'fetcher': {'urls': {'type': typing.List[str], 'is_mandatory': True}},
# 'converter': {'meta': {'type': typing.Union[typing.Dict[str, typing.Any], typing.List[typing.Dict[str, typing.Any]], NoneType],
# 'is_mandatory': False,
# 'default_value': None},
# 'extraction_kwargs': {'type': typing.Optional[typing.Dict[str, typing.Any]],
# 'is_mandatory': False,
# 'default_value': None}},
# 'writer': {'policy': {'type': typing.Optional[haystack.document_stores.types.policy.DuplicatePolicy],
# 'is_mandatory': False,
# 'default_value': None}}}
```


## 示例

```python
# import necessary dependencies
from haystack import Pipeline, Document
from haystack.components.generators.chat import OpenAIChatGenerator
from haystack.components.retrievers import InMemoryBM25Retriever
from haystack.document_stores.in_memory import InMemoryDocumentStore
from haystack.components.builders import ChatPromptBuilder
from haystack.utils import Secret
from haystack.dataclasses import ChatMessage

# create a document store and write documents to it
document_store = InMemoryDocumentStore()
document_store.write_documents(
    [
        Document(content="My name is Jean and I live in Paris."),
        Document(content="My name is Mark and I live in Berlin."),
        Document(content="My name is Giorgio and I live in Rome."),
    ],
)

# A prompt corresponds to an NLP task and contains instructions for the model. Here, the pipeline will go through each Document to figure out the answer.
prompt_template = [
    ChatMessage.from_system(
        """
        Given these documents, answer the question.
        Documents:
        {% for doc in documents %}
            {{ doc.content }}
        {% endfor %}
        Question:
        """,
    ),
    ChatMessage.from_user("{{question}}"),
    ChatMessage.from_system("Answer:"),
]

# create the components adding the necessary parameters
retriever = InMemoryBM25Retriever(document_store=document_store)
prompt_builder = ChatPromptBuilder(template=prompt_template, required_variables="*")
llm = OpenAIChatGenerator(
    api_key=Secret.from_env_var("OPENAI_API_KEY"),
    model="gpt-4o-mini",
)

# Create the pipeline and add the components to it. The order doesn't matter.
# At this stage, the Pipeline validates the components without running them yet.
rag_pipeline = Pipeline()
rag_pipeline.add_component("retriever", retriever)
rag_pipeline.add_component("prompt_builder", prompt_builder)
rag_pipeline.add_component("llm", llm)

# Arrange pipeline components in the order you need them. If a component has more than one inputs or outputs, indicate which input you want to connect to which output using the format ("component_name.output_name", "component_name, input_name").
rag_pipeline.connect("retriever", "prompt_builder.documents")
rag_pipeline.connect("prompt_builder", "llm")

# Run the pipeline by specifying the first component in the pipeline and passing its mandatory inputs. Optionally, you can pass inputs to other components.
question = "Who lives in paris?"
results = rag_pipeline.run(
    {
        "retriever": {"query": question},
        "prompt_builder": {"question": question},
    },
)

print(results["llm"]["replies"])
```


```mermaid
flowchart TD
    START["*"]

    START -->|query<br/>str| retriever["retriever<br/>InMemoryBM25Retriever<br/><br/>Optional inputs:<br/>• filters (Optional[dict[str,Any]])<br/>• top_k (Optional[int])<br/>• scale_score<br/>(Optional[bool])"]
    START -->|question<br/>Any| prompt_builder["prompt_builder<br/>ChatPromptBuilder<br/><br/>Optional inputs:<br/>• template<br/>(Union[list[ChatMessage], str])<br/>• template_variables<br/>(Optional[dict[str, Any]])"]

    retriever -->|documents -> documents<br/>list[Document]| prompt_builder

    prompt_builder -->|prompt -> messages<br/>list[ChatMessage]| llm["llm<br/>OpenAIChatGenerator<br/><br/>Optional inputs:<br/>• streaming_callback<br/>(Union[Callable[], Callable[, Awaitable[]]])<br/>• generation_kwargs<br/>(Optional[dict[str, Any]])<br/>• tools (Union[list[Tool],<br/>list[Toolset],<br/>list[Union[Tool, Toolset]],<br/>Toolset])<br/>• tools_strict (Optional[bool])"]

    llm -->|replies<br/>list[ChatMessage]| END["*"]
```