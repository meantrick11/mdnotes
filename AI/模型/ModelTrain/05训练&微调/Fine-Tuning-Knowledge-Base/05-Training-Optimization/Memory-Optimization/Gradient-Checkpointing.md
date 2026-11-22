# Gradient Checkpointing

> 一句话摘要：在前向时少保存部分激活，并在反向时重算，以显存换计算。**优先级：P1**。前置：[知识库总览](../../README.md)；后续：按[学习路线](../../LEARNING-ROADMAP.md)继续。

## 概念与工程意义

在前向时少保存部分激活，并在反向时重算，以显存换计算。它不能减少所有训练状态，适合激活占用显著的长序列场景。

## 资料与建设状态

[官方资料或原论文入口](https://docs.pytorch.org/docs/stable/checkpoint.html)。本页为后续阶段的准确定位索引，**尚未按完整笔记模板扩写，也未运行代码或实验**；链接访问状态以 [SOURCES](../../SOURCES.md) 为准。
