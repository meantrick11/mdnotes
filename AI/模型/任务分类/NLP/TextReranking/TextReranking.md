# Text Reranking

## 简介

**文本排序是指根据文本与查询的相关性对一组文本进行排序的任务。**文本排序模型在包含大量查询和相关文档的数据集上进行训练，以学习如何根据文档与查询的相关性对文档进行排序。该任务对搜索引擎和信息检索系统尤为有用。

比如：

```text
Input:
Source sentence
Machine learning is so easy.

Sentences to compare to
Deep learning is so straightforward.

This is so difficult, like rocket science.

I can't believe how much I struggled with this.

===>


Output:
Deep learning is so straightforward.
2.201
This is so difficult, like rocket science.
-6.263
I can't believe how much I struggled with this.
```

## 应用场景

### 信息检索

你可以在常见的“检索与重排流水线”中通过将文本排序模型用作重排器来改进信息检索的搜索栈。首先，你可以使用句子相似度或特征提取模型作为检索器，为一个查询找到（例如）100个最相关的文档。之后，你可以用文本排序模型对这100个文档分别进行重排，以选出更新后的前10个。通常情况下，这种方式比仅使用检索器模型能带来更好的检索性能。

## 任务变体

### 1. Passage Ranking

段落排序是指根据文档与给定查询的相关性对文档进行排序的任务。该任务的评估指标包括归一化折损累积增益、平均倒数排名或平均精确率。这些模型接收一个查询和多个文档，并根据与查询的相关性返回排序后的文档。


重排器的性能通常优于**句子相似度或特征提取模型**，但它们针对查询对所有文档进行重排的速度过慢。这就是它们通常被用于对检索器返回的顶级文档进行最终重排的原因：既能借助检索器模型实现高效检索，又能发挥重排器的出色性能。