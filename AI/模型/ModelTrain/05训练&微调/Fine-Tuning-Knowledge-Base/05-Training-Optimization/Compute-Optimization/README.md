# 计算优化

> 一句话摘要：研究 attention kernel、编译器与混合精度的计算路径。 **优先级：P1**。前置：[知识库总览](../../README.md)；后续：[学习路线](../../LEARNING-ROADMAP.md)。

## 本目录研究什么

研究 attention kernel、编译器与混合精度的计算路径。

## 与其他目录的关系

以[知识地图](../../KNOWLEDGE-MAP.md)区分训练范式、参数更新、软件层次与部署；在本目录研究其中一个维度。

## 学习顺序与重点

从 attention IO 和矩阵乘法入手，区分算法、kernel 与编译器的职责。

## 官方学习入口

- [FlashAttention（原论文）](https://arxiv.org/abs/2205.14135)
- [PyTorch scaled dot-product attention（官方）](https://docs.pytorch.org/docs/stable/generated/torch.nn.functional.scaled_dot_product_attention.html)
- [Triton 官方文档](https://triton-lang.org/main/index.html)

## 条目

- [FlashAttention.md](FlashAttention.md)
