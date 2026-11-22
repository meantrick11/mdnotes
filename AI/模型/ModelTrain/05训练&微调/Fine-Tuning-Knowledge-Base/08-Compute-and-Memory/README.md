# 算力与显存

> 一句话摘要：用权重、梯度、优化器状态、激活和临时缓冲分项估算，避免固定显存表误导。 **优先级：P1**。前置：[知识库总览](../README.md)；后续：[学习路线](../LEARNING-ROADMAP.md)。

## 本目录研究什么

用权重、梯度、优化器状态、激活和临时缓冲分项估算，避免固定显存表误导。

## 与其他目录的关系

以[知识地图](../KNOWLEDGE-MAP.md)区分训练范式、参数更新、软件层次与部署；在本目录研究其中一个维度。

## 学习顺序与重点

从参数量×字节的权重下界开始，再逐项加上梯度、优化器、激活和临时张量。

## 官方学习入口

- [PyTorch CUDA 内存管理（官方）](https://docs.pytorch.org/docs/stable/notes/cuda.html#memory-management)
- [PEFT 量化指南（官方）](https://huggingface.co/docs/peft/developer_guides/quantization)
- [QLoRA（原论文）](https://arxiv.org/abs/2305.14314)

## 条目

- [Full-FT-Memory.md](Full-FT-Memory.md)
- [GPU-Memory-Calculation.md](GPU-Memory-Calculation.md)
- [QLoRA-Memory.md](QLoRA-Memory.md)
