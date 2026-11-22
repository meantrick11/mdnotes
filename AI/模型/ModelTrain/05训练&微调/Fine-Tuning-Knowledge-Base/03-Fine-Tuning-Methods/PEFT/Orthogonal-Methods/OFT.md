# OFT

> 一句话摘要：用受正交约束的变换适配权重。 **优先级：P2**。前置：[方法总览](../README.md)；后续：[方法比较](../../../12-Comparisons/PEFT-Methods-Comparison.md)。

## 技术定义与解决的问题

用受正交约束的变换适配权重。 约束旨在保留部分几何性质；效果与计算开销需按任务验证。

## 核心机制、更新范围与使用

与低秩加法更新属于不同参数化思路。 具体可训练参数、训练流程、显存与适用模型必须结合原始论文及实际实现确认；此页只作分类和入门索引，不给未经验证的参数比例或硬件门槛。

## 资料与待核项

[相关官方资料或论文入口](https://huggingface.co/docs/peft)（链接入口，**本页未逐项核实全文与实现版本**；访问核实状态见 [SOURCES](../../../SOURCES.md)）。TODO：补原论文核读、公式、框架 API、适用模型与同预算实验。推荐先读 [LoRA](../Low-Rank-Adaptation/LoRA.md)，再研究本方法。

### 对应原始论文

- [Parameter-Efficient Orthogonal Finetuning via Butterfly Factorization（OFT/BOFT）](https://arxiv.org/abs/2311.06243)。2026-11-05 已核对论文标题与摘要入口；公式、实验及实现细节仍待核读全文。
