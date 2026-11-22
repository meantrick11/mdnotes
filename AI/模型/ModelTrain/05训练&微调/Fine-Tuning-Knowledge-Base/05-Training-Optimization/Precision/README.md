# 数值精度

> 一句话摘要：比较 FP32、FP16、BF16 的表示范围与训练稳定性。 **优先级：P1**。前置：[知识库总览](../../README.md)；后续：[学习路线](../../LEARNING-ROADMAP.md)。

## 本目录研究什么

比较 FP32、FP16、BF16 的表示范围与训练稳定性。

## 与其他目录的关系

以[知识地图](../../KNOWLEDGE-MAP.md)区分训练范式、参数更新、软件层次与部署；在本目录研究其中一个维度。

## 学习顺序与重点

先比较 FP32、FP16、BF16 的数值范围，再学习混合精度的实际计算路径。

## 官方学习入口

- [PyTorch AMP 官方文档](https://docs.pytorch.org/docs/stable/amp.html)
- [PyTorch AMP 教程（官方）](https://pytorch.org/tutorials/recipes/recipes/amp_recipe.html)

## 条目

- [BF16.md](BF16.md)
