# 训练优化

> 一句话摘要：先分清数值精度、量化、内存与计算优化；具体效果依赖硬件和实现。 **优先级：P1**。前置：[知识库总览](../README.md)；后续：[学习路线](../LEARNING-ROADMAP.md)。

## 本目录研究什么

先分清数值精度、量化、内存与计算优化；具体效果依赖硬件和实现。

## 与其他目录的关系

以[知识地图](../KNOWLEDGE-MAP.md)区分训练范式、参数更新、软件层次与部署；在本目录研究其中一个维度。

## 学习顺序与重点

按精度、量化、内存和计算优化逐项分析瓶颈；不要把 INT4 等同于训练显存总量。

## 官方学习入口

- [PyTorch AMP 官方文档](https://docs.pytorch.org/docs/stable/amp.html)
- [Transformers 量化总览（官方）](https://huggingface.co/docs/transformers/quantization/overview)

## 条目

- [Compute-Optimization](Compute-Optimization/README.md)
- [Memory-Optimization](Memory-Optimization/README.md)
- [Precision](Precision/README.md)
- [Quantization](Quantization/README.md)
