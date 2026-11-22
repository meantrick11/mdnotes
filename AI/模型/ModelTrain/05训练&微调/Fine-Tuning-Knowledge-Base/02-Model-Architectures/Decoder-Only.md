# Decoder-only

> 一句话摘要：使用因果注意力预测后续 token，常用于自回归生成。**优先级：P1**。前置：[知识库总览](../README.md)；后续：按[学习路线](../LEARNING-ROADMAP.md)继续。

## 概念与工程意义

使用因果注意力预测后续 token，常用于自回归生成。分类任务也可给 Decoder 模型增加分类头，但数据模板、padding 与最后有效 token 的读取规则必须核实。

## 资料与建设状态

[官方资料或原论文入口](https://huggingface.co/docs/transformers)。本页为后续阶段的准确定位索引，**尚未按完整笔记模板扩写，也未运行代码或实验**；链接访问状态以 [SOURCES](../SOURCES.md) 为准。
