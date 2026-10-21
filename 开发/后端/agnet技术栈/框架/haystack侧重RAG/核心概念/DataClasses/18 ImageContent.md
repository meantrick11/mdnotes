`ImageContent`是一个 Haystack 数据类，用于表示聊天消息和多模态人工智能管道中的基于图像的内容。

它通常与以下工具配合使用：
- 多模态大语言模型
- 视觉语言模型
- 图像管制聊天应用
- 文档/图像处理工作流

图像内容将图像以base64编码字符串的形式存储，并搭配MIME类型、图像细节级别等元数据。


## 创建ImageContent

你可以直接从 base64 字符串创建一个`ImageContent`对象：

```python
from haystack.dataclasses import ImageContent

image = ImageContent(base64_image="your_base64_encoded_image", mime_type="image/png")

print(image)
```

## from_file_path

`from_file_path()` 类方法提供了一种加载本地图像文件的便捷方式。

```python
from haystack.dataclasses import ImageContent

image = ImageContent.from_file_path("sample.png", detail="low")

print(image)
```
OpenAI 视觉模型目前支持可选的`detail`参数，其接受：

- `"auto"`
- `"high"`
- `"low"`

你也可以在加载时调整图片大小：

```python
image = ImageContent.from_file_path("sample.png", size=(512, 512))

```

这有助于减少内存使用量、处理时间、载荷大小

## from_url

你也可以直接从图片 URL 创建一个 `ImageContent` 对象：
```python
from haystack.dataclasses import ImageContent

image = ImageContent.from_url(
    "https://images.unsplash.com/photo-1546182990-dffeafbe841d",
    detail="low",
)

print(image)
```
在内部，Haystack 会下载该图片并将其转换为 base64 编码形式。

## 使用转换器Converter生成图像内容

在管道中，你通常不会手动创建 `ImageContent` 对象。相反，你会使用转换器组件来读取文件，并为你生成 `ImageContent`：
- [`ImageFileToImageContent`](https://docs.haystack.deepset.ai/docs/imagefiletoimagecontent) 会将本地图像文件（如 PNG 或 JPEG 格式）转换为 `ImageContent` 对象。
- [`PDFToImageContent`](https://docs.haystack.deepset.ai/docs/pdftoimagecontent) 将 PDF 文件的页面渲染为 `ImageContent` 对象。

```python
from haystack.components.converters.image import (
    ImageFileToImageContent,
    PDFToImageContent,
)

image_converter = ImageFileToImageContent()
image_contents = image_converter.run(sources=["image.jpg", "another_image.png"])[
    "image_contents"
]

pdf_converter = PDFToImageContent()
pdf_image_contents = pdf_converter.run(sources=["file.pdf"])["image_contents"]
```

两个转换器都接受可选的 `detail` 和 `size` 参数，这些参数会被转发到它们创建的 `ImageContent` 对象中。


## ImageContent与ChatMessage配合使用

```python
from haystack.dataclasses import ChatMessage, ImageContent

image = ImageContent.from_url(
    "https://images.unsplash.com/photo-1546182990-dffeafbe841d",
    detail="low",
)

message = ChatMessage.from_user(content_parts=["What does this image show?", image])

print(message)
```

这使得多模态大语言模型能够同时处理：
- 文本提示词
- 图像输入


## 元数据

可选的 ``meta`` 参数允许你为图片附加自定义元数据。
```python
image = ImageContent.from_url(
    "https://images.unsplash.com/photo-1546182990-dffeafbe841d",
    meta={"source": "example-dataset"},
)
```

适用于一下场景：
- 追踪
- 工作流元数据
- 自定义程序逻辑
## 验证

默认情况下，ImageContent会进行以下验证：
- base64编码
- MIME类型正确
- 图像MIME兼容性
为了更快速/提高性能，可以禁用：
```python
image = ImageContent(
    base64_image="your_base64_encoded_image",
    mime_type="image/png",
    validation=False,
)
```

## 序列化

`ImageContent`支持字典序列化。

```python
image_dict = image.to_dict()

restored_image = ImageContent.from_dict(image_dict)
```
## 显示图像
`show()` 方法可直接在以下位置显示图像：
- Jupyter笔记本
- 本地桌面环境
image.show()

不过，这需要安装python的pillow包