# Fill-Mask（掩码填充）

Fill-Mask 也叫掩码语言建模：把句子中的一个或多个词遮住，让模型预测合适的词。模型返回候选词及其得分，常用于学习特定领域文本的语言规律。

## 例子

输入：`The <mask> barked at me`（`<mask>` 表示被遮住的词）。

页面给出的候选词包括 `wolf`、`dog`、`cat` 等，并附有各自的预测得分。模型是在补全原句，而不是给整句话分类。

## 应用场景

在医学论文等领域文本上做掩码语言建模，得到更熟悉该领域措辞的模型；之后可以针对文本分类、问答等任务继续微调。

来源：[Hugging Face：Fill-Mask](https://huggingface.co/tasks/fill-mask)
