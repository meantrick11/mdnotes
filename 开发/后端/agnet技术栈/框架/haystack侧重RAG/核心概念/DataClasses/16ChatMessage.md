
ChatMessage是用来表示大预言模型消息的核心抽象，他包括角色、元数据以及多种类型的内容，包括文本、图像、工具调用结果和推理内容。


要创建 `ChatMessage` 实例，请使用 `from_user`、`from_system`、`from_assistant` 和 `from_tool` 类方法。

随后可通过[内容](https://docs.haystack.deepset.ai/docs/chatmessage#types-of-content)、`ChatMessage`的`text`、`texts`、`image`、`images`、`file`、`files`、`tool_call`、`tool_calls`、`tool_call_result`、`tool_call_results`、`reasoning`和`reasonings`属性来查看content。

### 内容类型

`ChatMessage`当前支持`TextContent`、`ImageContent`、`FileContent`、`ToolCall`、`ToolCallResult`和`ReasoningContent`类型的内容：

```python
@dataclass
class TextContent:
    """
    The textual content of a chat message.

    :param text: The text content of the message.
    """

    text: str


@dataclass
class ToolCall:
    """
    Represents a Tool call prepared by the model, usually contained in an assistant message.

    :param tool_name: The name of the Tool to call.
    :param arguments: The arguments to call the Tool with.
    :param id: The ID of the Tool call.
    :param extra: Dictionary of extra information about the Tool call. Use to store provider-specific
        information. To avoid serialization issues, values should be JSON serializable.
    """

    tool_name: str
    arguments: Dict[str, Any]
    id: Optional[str] = None  # noqa: A003
    extra: Optional[Dict[str, Any]] = None


@dataclass
class ToolCallResult:
    """
    Represents the result of a Tool invocation.

    :param result: The result of the Tool invocation.
    :param origin: The Tool call that produced this result.
    :param error: Whether the Tool invocation resulted in an error.
    """

    result: str | Sequence[TextContent | ImageContent]
    origin: ToolCall
    error: bool


@dataclass
class ImageContent:
    """
    The image content of a chat message.

    :param base64_image: A base64 string representing the image.
    :param mime_type: The MIME type of the image (e.g. "image/png", "image/jpeg").
        Providing this value is recommended, as most LLM providers require it.
        If not provided, the MIME type is guessed from the base64 string, which can be slow and not always reliable.
    :param detail: Optional detail level of the image (only supported by OpenAI). One of "auto", "high", or "low".
    :param meta: Optional metadata for the image.
    :param validation: If True (default), a validation process is performed:
        - Check whether the base64 string is valid;
        - Guess the MIME type if not provided;
        - Check if the MIME type is a valid image MIME type.
        Set to False to skip validation and speed up initialization.
    """

    base64_image: str
    mime_type: Optional[str] = None
    detail: Optional[Literal["auto", "high", "low"]] = None
    meta: Dict[str, Any] = field(default_factory=dict)
    validation: bool = True


@dataclass
class FileContent:
    """
    The file content of a chat message.

    :param base64_data: A base64 string representing the file.
    :param mime_type: The MIME type of the file (e.g. "application/pdf").
        Providing this value is recommended, as most LLM providers require it.
        If not provided, the MIME type is guessed from the base64 string, which can be slow and not always reliable.
    :param filename: Optional filename of the file. Some LLM providers use this information.
    :param extra: Dictionary of extra information about the file. Can be used to store provider-specific information.
        To avoid serialization issues, values should be JSON serializable.
    :param validation: If True (default), a validation process is performed:
        - Check whether the base64 string is valid;
        - Guess the MIME type if not provided.
        Set to False to skip validation and speed up initialization.
    """

    base64_data: str
    mime_type: str | None = None
    filename: str | None = None
    extra: dict[str, Any] = field(default_factory=dict)
    validation: bool = True


@dataclass
class ReasoningContent:
    """
    Represents the optional reasoning content prepared by the model, usually contained in an assistant message.

    :param reasoning_text: The reasoning text produced by the model.
    :param extra: Dictionary of extra information about the reasoning content. Use to store provider-specific
        information. To avoid serialization issues, values should be JSON serializable.
    """

    reasoning_text: str
    extra: Dict[str, Any] = field(default_factory=dict)
    
```

### 使用ChatMessage

#### 1.  from_user 

#### text实例

```python
from haystack.dataclasses import ChatMessage

user_message = ChatMessage.from_user("What is the capital of Australia?")

print(user_message)
# >> ChatMessage(
# >>    _role=<ChatRole.USER: 'user'>,
# >>    _content=[TextContent(text='What is the capital of Australia?')],
# >>    _name=None,
# >>    _meta={}
# >> )

print(user_message.text)
# >> What is the capital of Australia?

print(user_message.texts)
# >> ['What is the capital of Australia?']

```

#### TextContent+ImageContent

```python
from haystack.dataclasses import ChatMessage, ImageContent

lion_image_url = (
    "https://images.unsplash.com/photo-1546182990-dffeafbe841d?"
    "ixlib=rb-4.0&q=80&w=1080&fit=max"
)

image_content = ImageContent.from_url(lion_image_url, detail="low")

user_message = ChatMessage.from_user(
    content_parts=["What does the image show?", image_content]
)

print(user_message)
# >> ChatMessage(
# >>     _role=<ChatRole.USER: 'user'>,
# >>     _content=[
# >>         TextContent(text='What does the image show?'),
# >>         ImageContent(
# >>             base64_image='/9j/4...',
# >>             mime_type='image/jpeg',
# >>             detail='low',
# >>             meta={
# >>                 'content_type': 'image/jpeg',
# >>                 'url': '...'
# >>             }
# >>         )
# >>     ],
# >>     _name=None,
# >>     _meta={}
# >> )

print(user_message.text)
# >> What does the image show?

print(user_message.texts)
# >> ['What does the image show?']

print(user_message.image)
# >> ImageContent(
# >>     base64_image='/9j/4...',
# >>     mime_type='image/jpeg',
# >>     detail='low',
# >>     meta={
# >>         'content_type': 'image/jpeg',
# >>         'url': '...'
# >>     }
# >> )

```

#### TextContent&FileContent


```python
from haystack.dataclasses import ChatMessage, FileContent

paper_url = "https://arxiv.org/pdf/2309.08632"

file_content = FileContent.from_url(paper_url)

user_message = ChatMessage.from_user(
    content_parts=[file_content, "Summarize this paper in 100 words."]
)

print(user_message)
# >> ChatMessage(
# >>     _role=<ChatRole.USER: 'user'>,
# >>     _content=[
# >>         FileContent(
# >>             base64_data='JVBERi0...',
# >>             mime_type='application/pdf',
# >>             filename='2309.08632',
# >>             extra={}
# >>         ),
# >>         TextContent(text='Summarize this paper in 100 words.')
# >>     ],
# >>     _name=None,
# >>     _meta={}
# >> )

print(user_message.text)
# >> Summarize this paper in 100 words.

print(user_message.texts)
# >> ['Summarize this paper in 100 words.']

print(user_message.file)
# >> FileContent(
# >>     base64_data='JVBERi0...',
# >>     mime_type='application/pdf',
# >>     filename='2309.08632',
# >>     extra={}
# >> )
```

### 2. from_assistant


#### TextContent

```python
from haystack.dataclasses import ChatMessage

assistant_message = ChatMessage.from_assistant("How can I assist you today?")

print(assistant_message)
# >> ChatMessage(
# >>    _role=<ChatRole.ASSISTANT: 'assistant'>,
# >>    _content=[TextContent(text='How can I assist you today?')],
# >>    _name=None,
# >>    _meta={}
# >> )

print(assistant_message.text)
# >> How can I assist you today?

print(assistant_message.texts)
# >> ['How can I assist you today?']
```

### ToolCall

```python
from haystack.dataclasses import ChatMessage, ToolCall

tool_call = ToolCall(tool_name="weather_tool", arguments={"location": "Rome"})

assistant_message_w_tool_call = ChatMessage.from_assistant(tool_calls=[tool_call])

print(assistant_message_w_tool_call)
# >> ChatMessage(
# >>    _role=<ChatRole.ASSISTANT: 'assistant'>,
# >>    _content=[ToolCall(tool_name='weather_tool', arguments={'location': 'Rome'}, id=None)],
# >>    _name=None,
# >>    _meta={}
# >> )

print(assistant_message_w_tool_call.text)
# >> None

print(assistant_message_w_tool_call.texts)
# >> []

print(assistant_message_w_tool_call.tool_call)
# >> ToolCall(tool_name='weather_tool', arguments={'location': 'Rome'}, id=None)

print(assistant_message_w_tool_call.tool_calls)
# >> [ToolCall(tool_name='weather_tool', arguments={'location': 'Rome'}, id=None)]

print(assistant_message_w_tool_call.tool_call_result)
# >> None

print(assistant_message_w_tool_call.tool_call_results)
# >> []
```



### 3. From_tool

```python
from haystack.dataclasses import ChatMessage

tool_message = ChatMessage.from_tool(
    tool_result="temperature: 25°C", origin=tool_call, error=False
)

print(tool_message)
# >> ChatMessage(
# >>    _role=<ChatRole.TOOL: 'tool'>,
# >>    _content=[ToolCallResult(
# >> 							   result='temperature: 25°C',
# >>                origin=ToolCall(tool_name='weather_tool', arguments={'location': 'Rome'}, id=None),
# >>                error=False
# >>                )],
# >>    _name=None,
# >>    _meta={}
# >> )

print(tool_message.text)
# >> None

print(tool_message.texts)
# >> []

print(tool_message.tool_call)
# >> None

print(tool_message.tool_calls)
# >> []

print(tool_message.tool_call_result)
# >> ToolCallResult(
# >>     result='temperature: 25°C',
# >>     origin=ToolCall(tool_name='weather_tool', arguments={'location': 'Rome'}, id=None),
# >>     error=False
# >> )

print(tool_message.tool_call_results)
# >> [
# >>     ToolCallResult(
# >>         result='temperature: 25°C',
# >>         origin=ToolCall(tool_name='weather_tool', arguments={'location': 'Rome'}, id=None),
# >>         error=False
# >>     )
# >> ]
```


