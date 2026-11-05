# Token Classification（词元分类）

词元分类给文本中的词或词元分配标签，而不是只给整段文本一个标签。页面列出的常见子任务有**命名实体识别**（NER，识别人名、地点等）和**词性标注**（PoS，标记名词、动词等）。

## 例子

输入：`My name is Omar and I live in Zürich.`

页面示例把 `Omar` 标为 `PERSON`（人名），把 `Zürich` 标为 `GPE`（地缘政治实体）。

## 应用场景

先用 OCR 读取发票文字，再用 NER 提取日期、公司名称等关键实体。

来源：[Hugging Face：Token Classification](https://huggingface.co/tasks/token-classification)
