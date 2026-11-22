# PEFT 与量化工具

> 一句话摘要：区分 PEFT 算法实现与 bitsandbytes 等低精度工具。 **优先级：P1**。前置：[知识库总览](../../README.md)；后续：[学习路线](../../LEARNING-ROADMAP.md)。

## 本目录研究什么

区分 PEFT 算法实现与 bitsandbytes 等低精度工具。

## 与其他目录的关系

以[知识地图](../../KNOWLEDGE-MAP.md)区分训练范式、参数更新、软件层次与部署；在本目录研究其中一个维度。

## 学习顺序与重点

先学 PEFT 适配器注入，再学 bitsandbytes 的低比特权重；GPTQ/AWQ 主要放在推理量化语境。

## 官方学习入口

- [PEFT 官方文档](https://huggingface.co/docs/peft)
- [Transformers bitsandbytes 指南（官方）](https://huggingface.co/docs/transformers/quantization/bitsandbytes)
- [Transformers 量化总览（官方）](https://huggingface.co/docs/transformers/quantization/overview)

## 条目

- [AWQ.md](AWQ.md)
- [bitsandbytes.md](bitsandbytes.md)
- [GPTQ.md](GPTQ.md)
- [PEFT.md](PEFT.md)
