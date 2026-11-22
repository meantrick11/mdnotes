# 模型架构

> 一句话摘要：区分 Encoder-only、Decoder-only 与 Encoder-Decoder；架构影响任务头、损失掩码与模型选择。 **优先级：P1**。前置：[知识库总览](../README.md)；后续：[学习路线](../LEARNING-ROADMAP.md)。

## 本目录研究什么

区分 Encoder-only、Decoder-only 与 Encoder-Decoder；架构影响任务头、损失掩码与模型选择。

## 与其他目录的关系

以[知识地图](../KNOWLEDGE-MAP.md)区分训练范式、参数更新、软件层次与部署；在本目录研究其中一个维度。

## 学习顺序与重点

按 Encoder-only → Decoder-only → Encoder-Decoder 比较注意力掩码、任务头和训练目标。

## 官方学习入口

- [Transformers 模型文档（官方）](https://huggingface.co/docs/transformers/model_doc/auto)
- [BERT（原论文）](https://arxiv.org/abs/1810.04805)
- [T5（原论文）](https://arxiv.org/abs/1910.10683)

## 条目

- [Decoder-Only.md](Decoder-Only.md)
- [Encoder-Decoder.md](Encoder-Decoder.md)
- [Encoder-Only.md](Encoder-Only.md)
