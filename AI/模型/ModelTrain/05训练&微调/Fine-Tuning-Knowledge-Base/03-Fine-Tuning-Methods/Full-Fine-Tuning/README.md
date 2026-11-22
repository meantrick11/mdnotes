# 全参数与部分参数更新

> 一句话摘要：先理解 Full FT 的全部权重更新，再看冻结部分层的 Partial FT。 **优先级：P1**。前置：[知识库总览](../../README.md)；后续：[学习路线](../../LEARNING-ROADMAP.md)。

## 本目录研究什么

先理解 Full FT 的全部权重更新，再看冻结部分层的 Partial FT。

## 与其他目录的关系

以[知识地图](../../KNOWLEDGE-MAP.md)区分训练范式、参数更新、软件层次与部署；在本目录研究其中一个维度。

## 学习顺序与重点

从 PyTorch Autograd 和优化器理解原权重如何更新，再比较部分冻结。

## 官方学习入口

- [PyTorch Autograd（官方教程）](https://pytorch.org/tutorials/beginner/basics/autogradqs_tutorial.html)
- [Transformers 训练指南（官方）](https://huggingface.co/docs/transformers/training)

## 条目

- [Full-FT.md](Full-FT.md)
- [Partial-FT.md](Partial-FT.md)
