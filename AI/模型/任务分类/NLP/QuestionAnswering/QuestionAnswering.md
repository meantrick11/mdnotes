# Question Answering（问答）

问答任务根据问题寻找答案。Hugging Face 页面展示的主要形式是**抽取式问答**：同时提供问题和上下文，模型从上下文中找出答案片段。页面也区分了依据上下文生成答案和不提供上下文直接生成答案的形式。

## 例子

- 问题：`Which name is also used to describe the Amazon rainforest in English?`
- 上下文：`The Amazon rainforest, also known in English as Amazonia or the Amazon Jungle`
- 答案：`Amazonia`

## 应用场景

把知识库文档作为上下文，自动回答常见问题。如果文档很多，可以先检索或排序可能包含答案的文档，再交给问答模型查找答案。

来源：[Hugging Face：Question Answering](https://huggingface.co/tasks/question-answering)
