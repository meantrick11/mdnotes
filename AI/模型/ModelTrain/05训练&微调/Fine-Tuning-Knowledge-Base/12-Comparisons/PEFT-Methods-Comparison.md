# PEFT 方法比较

> 一句话摘要：LoRA 以低秩矩阵适配权重；AdaLoRA 分配动态 rank；DoRA 拆分幅值与方向；IA³ 学缩放向量；Prompt/Prefix 训练软提示。**优先级：P1**。前置：[知识库总览](../README.md)；后续：按[学习路线](../LEARNING-ROADMAP.md)继续。

## 概念与工程意义

LoRA 以低秩矩阵适配权重；AdaLoRA 分配动态 rank；DoRA 拆分幅值与方向；IA³ 学缩放向量；Prompt/Prefix 训练软提示。QLoRA 同时涉及量化，不宜仅以参数数量对齐比较。

## 资料与建设状态

[官方资料或原论文入口](https://huggingface.co/docs/peft)。本页为后续阶段的准确定位索引，**尚未按完整笔记模板扩写，也未运行代码或实验**；链接访问状态以 [SOURCES](../SOURCES.md) 为准。
