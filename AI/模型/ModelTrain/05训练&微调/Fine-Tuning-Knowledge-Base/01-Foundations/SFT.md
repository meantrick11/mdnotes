# 监督微调 SFT

> 一句话摘要：在输入与期望输出上最小化监督损失。**优先级：P1**。前置：[知识库总览](../README.md)；后续：按[学习路线](../LEARNING-ROADMAP.md)继续。

## 概念与工程意义

在输入与期望输出上最小化监督损失。对于对话任务，聊天模板和 loss mask 决定模型究竟学习哪些 token；SFT 可采用 Full FT 或 LoRA。

## 资料与建设状态

[官方资料或原论文入口](https://huggingface.co/docs/trl/sft_trainer)。本页为后续阶段的准确定位索引，**尚未按完整笔记模板扩写，也未运行代码或实验**；链接访问状态以 [SOURCES](../SOURCES.md) 为准。
