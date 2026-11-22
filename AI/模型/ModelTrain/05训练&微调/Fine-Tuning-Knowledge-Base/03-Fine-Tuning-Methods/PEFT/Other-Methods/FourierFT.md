# FourierFT

> 一句话摘要：在频域参数化权重更新，以较少参数表示适配增量。 **优先级：P2**。前置：[方法总览](../README.md)；后续：[方法比较](../../../12-Comparisons/PEFT-Methods-Comparison.md)。

## 技术定义与解决的问题

在频域参数化权重更新，以较少参数表示适配增量。 适合探索结构化更新与压缩，实际收益应按相同训练预算比较。

## 核心机制、更新范围与使用

不是 LoRA 的低秩矩阵分解。 具体可训练参数、训练流程、显存与适用模型必须结合原始论文及实际实现确认；此页只作分类和入门索引，不给未经验证的参数比例或硬件门槛。

## 资料与待核项

[相关官方资料或论文入口](https://huggingface.co/docs/peft)（链接入口，**本页未逐项核实全文与实现版本**；访问核实状态见 [SOURCES](../../../SOURCES.md)）。TODO：补原论文核读、公式、框架 API、适用模型与同预算实验。推荐先读 [LoRA](../Low-Rank-Adaptation/LoRA.md)，再研究本方法。

### 对应原始论文

- [Parameter-Efficient Fine-Tuning with Discrete Fourier Transform](https://arxiv.org/abs/2405.03003)。2026-11-05 已核对论文标题与摘要入口；公式、实验及实现细节仍待核读全文。
