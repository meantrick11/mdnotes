# Visual Document Retrieval（视觉文档检索）

输入文字查询和一组以图像形式呈现的文档页面，模型按相关性找出最可能有用的页面，并给出得分。这里的目标是**定位文档**，不是直接生成答案。

## 例子

页面示例询问 `Is the model in this paper the fastest for inference?`，检索结果把第 10 页排在前面，相关性得分为 `0.700`。

## 应用场景

先为 PDF 页面建立索引，再从大量页面中找出与问题最相关的几页，供后续文档问答或多模态 RAG 使用。

来源：[Hugging Face：Visual Document Retrieval](https://huggingface.co/tasks/visual-document-retrieval)
