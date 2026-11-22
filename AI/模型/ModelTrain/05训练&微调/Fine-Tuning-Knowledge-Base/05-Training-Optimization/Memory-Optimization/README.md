# 显存优化

> 一句话摘要：分别理解激活重计算、梯度累积、卸载与状态分片。 **优先级：P1**。前置：[知识库总览](../../README.md)；后续：[学习路线](../../LEARNING-ROADMAP.md)。

## 本目录研究什么

分别理解激活重计算、梯度累积、卸载与状态分片。

## 与其他目录的关系

以[知识地图](../../KNOWLEDGE-MAP.md)区分训练范式、参数更新、软件层次与部署；在本目录研究其中一个维度。

## 学习顺序与重点

分别核算激活、优化器状态与权重驻留，再选择重计算、卸载或分片。

## 官方学习入口

- [PyTorch 激活重计算（官方）](https://docs.pytorch.org/docs/stable/checkpoint.html)
- [DeepSpeed ZeRO（官方）](https://www.deepspeed.ai/tutorials/zero/)

## 条目

- [Gradient-Checkpointing.md](Gradient-Checkpointing.md)
