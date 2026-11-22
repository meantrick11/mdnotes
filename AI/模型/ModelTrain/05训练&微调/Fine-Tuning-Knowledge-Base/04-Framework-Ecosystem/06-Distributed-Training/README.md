# 分布式执行

> 一句话摘要：理解设备编排、状态分片与多卡训练的边界。 **优先级：P1**。前置：[知识库总览](../../README.md)；后续：[学习路线](../../LEARNING-ROADMAP.md)。

## 本目录研究什么

理解设备编排、状态分片与多卡训练的边界。

## 与其他目录的关系

以[知识地图](../../KNOWLEDGE-MAP.md)区分训练范式、参数更新、软件层次与部署；在本目录研究其中一个维度。

## 学习顺序与重点

先理解 PyTorch Distributed 和 FSDP，再看 Accelerate 与 DeepSpeed 如何组织执行。

## 官方学习入口

- [PyTorch Distributed（官方）](https://docs.pytorch.org/docs/stable/distributed.html)
- [Accelerate 官方文档](https://huggingface.co/docs/accelerate)
- [DeepSpeed 官方教程](https://www.deepspeed.ai/tutorials/)

## 条目

- [Accelerate.md](Accelerate.md)
- [Colossal-AI.md](Colossal-AI.md)
- [DeepSpeed.md](DeepSpeed.md)
- [FSDP.md](FSDP.md)
- [Megatron-LM.md](Megatron-LM.md)
