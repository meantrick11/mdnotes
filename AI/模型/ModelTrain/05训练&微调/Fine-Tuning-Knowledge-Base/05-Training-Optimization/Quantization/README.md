# 量化

> 一句话摘要：区分训练时冻结基座的低比特加载与推理量化。 **优先级：P1**。前置：[知识库总览](../../README.md)；后续：[学习路线](../../LEARNING-ROADMAP.md)。

## 本目录研究什么

区分训练时冻结基座的低比特加载与推理量化。

## 与其他目录的关系

以[知识地图](../../KNOWLEDGE-MAP.md)区分训练范式、参数更新、软件层次与部署；在本目录研究其中一个维度。

## 学习顺序与重点

先区分权重/激活量化、训练时量化加载与训练后推理量化，再看 NF4。

## 官方学习入口

- [Transformers 量化总览（官方）](https://huggingface.co/docs/transformers/quantization/overview)
- [Transformers bitsandbytes 指南（官方）](https://huggingface.co/docs/transformers/quantization/bitsandbytes)
- [QLoRA（原论文）](https://arxiv.org/abs/2305.14314)

## 条目

- [Quantization-Overview.md](Quantization-Overview.md)
