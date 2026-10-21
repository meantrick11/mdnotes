# Data Classes

在 Haystack 中，有一些核心类会在许多不同的地方被频繁


Haystack 使用数据类以简单且模块化的方式帮助组件之间进行通信。通过这种方式，数据可以在 Haystack 管道中无缝流转。本页面将介绍 Haystack 中可用的数据类：字节流（ByteStream）、答案（Answer，及其变体提取答案 ExtractedAnswer 和生成答案 GeneratedAnswer）、聊天消息（ChatMessage）、文件内容（FileContent）、图像内容（ImageContent）、文档（Document）以及流式处理块（StreamingChunk），并说明它们如何为 Haystack 生态系统做出贡献。


## 1. Answer Class

Answer 类是 Haystack 中生成的回复的基类，包含答案数据、原始查询以及额外元数据。

### 特性

- 自适应数据处理，可适配任何数据类型（data）。
- 基于上下文相关性的查询跟踪（query）。
- 为详细的答案描述提供全面的元数据支持。

### 属性
```py
@dataclass
class Answer:
    data: Any
    query: str
    meta: Dict[str, Any]
```

## 2. ExtractAnswer Class

ExtractedAnswer是Answer的一个子类，专门处理从文档中获取的答案，并提供了更详细的属性。

### 特性
- 包含对原始Document的引用。
- Score 属性用于量化答案的置信度水平。
- 用于精确定位答案在源数据中位置的可选起始和结束索引。

### 属性

```python
@dataclass
class ExtractedAnswer:
    query: str
    score: float
    data: Optional[str] = None
    document: Optional[Document] = None
    context: Optional[str] = None
    document_offset: Optional["Span"] = None
    context_offset: Optional["Span"] = None
    meta: Dict[str, Any] = field(default_factory=dict)
```


## 3. GeneratedAnswer 

GeneratedAnswer扩展了Answer类，以适配从多个文档生成的答案。

### 特性

- 处理字符串类型数据。
- 链接至Document对象列表，提升答案的可追溯性。

### 属性
```python
@dataclass
class GeneratedAnswer:
    data: str
    query: str
    documents: List[Document]
    meta: Dict[str, Any] = field(default_factory=dict)
```

## 4. ByteStream Class


ByteStream是 Haystack 框架中的二进制对象抽象，对于处理各类二进制数据格式至关重要。

### 特性

- 存储二进制数据及相关元数据。

- 为实现灵活性，可指定可选的 MIME 类型。
- 文件交互方法（to_file、from_file_path、from_string）可实现便捷的数据操作。

### 属性

```python
@dataclass(repr=False)
class ByteStream:
    data: bytes
    meta: Dict[str, Any] = field(default_factory=dict, hash=False)
    mime_type: Optional[str] = field(default=None)
```

## 5. ChatMessage

ChatMessage 是用于表示大语言模型消息的核心抽象。它包含角色、元数据以及多种类型的内容，包括文本、工具调用和工具调用结果。

## 6. FileContent 

FileContent表示可附加到ChatMessage的文件负载，包括base64数据、MIME类型、文件名以及特定于提供方的元数据。

## 7. ImageContent

ImageContent指的是在多模态聊天消息和视觉语言流程中使用的基于图像的内容。

## 8. Document

Document是Haystack中的核心数据抽象，可存储文本、表格和二进制数据。

### 特性

- **每个文档的唯一标识符。**
- 支持多种内容类型：文本、二进制（blob）。
- 用于高级文档管理的自定义元数据和评分功能。
- 适用于基于人工智能的应用程序的可选嵌入

### 属性

```py
@dataclass
class Document(metaclass=_BackwardCompatible):
    id: str = field(default="")
    content: Optional[str] = field(default=None)
    blob: Optional[ByteStream] = field(default=None)
    meta: Dict[str, Any] = field(default_factory=dict)
    score: Optional[float] = field(default=None)
    embedding: Optional[List[float]] = field(default=None)
    sparse_embedding: Optional[SparseEmbedding] = field(default=None)
    
```

## 9. StreamingChunk

`StreamingChunk`表示部分流式传输的大语言模型响应，支持对大语言模型响应进行实时处理。它封装了一段流式传输的内容及相关元数据，并提供有关流式传输状态的全面信息。

### 特性
- 用于文本块的基于字符串的内容表示
- 支持工具调用及工具调用结果
- 组件跟踪和元数据管理
- 流式状态指示器（启动、完成原因）


### 属性

```python
@dataclass
class StreamingChunk:
    content: str
    meta: dict[str, Any] = field(default_factory=dict, hash=False)
    component_info: Optional[ComponentInfo] = field(default=None)
    index: Optional[int] = field(default=None)
    tool_calls: Optional[list[ToolCallDelta]] = field(default=None)
    tool_call_result: Optional[ToolCallResult] = field(default=None)
    start: bool = field(default=False)
    finish_reason: Optional[FinishReason] = field(default=None)
    reasoning: Optional[ReasoningContent] = field(default=None)
```

## 10. ToolCallDelta


`ToolCallDelta`表示由模型准备的一次工具调用，在流式传输过程中通常包含在助手消息中。

### 属性

```python
@dataclass
class ToolCallDelta:
    index: int
    tool_name: Optional[str] = field(default=None)
    arguments: Optional[str] = field(default=None)
    id: Optional[str] = field(default=None)
    extra: Optional[Dict[str, Any]] = field(default=None)
```

## 11.  ComponentInfo 

`ComponentInfo` 类表示 Haystack 管道中某个组件的相关信息。它用于追踪生成或处理数据的组件的类型和名称，有助于在整个管道中进行调试、跟踪以及元数据管理。

### 特性

-  存储组件的类型（包括模块和类名）。
- 可选地存储在管道中分配给该组件的名称。
- 提供了一个便捷的类方法，用于从 `Component` 对象创建 `ComponentInfo` 实例。

### 属性

```python
@dataclass
class ComponentInfo:
    type: str
    name: Optional[str] = field(default=None)

    @classmethod
    def from_component(cls, component: Component) -> "ComponentInfo": ...
```


## 12.  SparseEmbedding

`SparseEmbedding` 类表示稀疏嵌入：一种大多数值均为零的向量。

### 属性
- `indices`: List of indices of non-zero elements in the embedding.indices：嵌入中非零元素的索引列表。
- `values`: List of values of non-zero elements in the embedding.`values`：嵌入中非零元素的值列表

## 13. Tool

`Tool`是一个数据类，用于表示语言模型可以为其准备调用请求的工具。

