# LoRA+

> 一句话摘要：为 LoRA 两个低秩矩阵设置不同学习率的改进策略。 **优先级：P2**。前置：[方法总览](../README.md)；后续：[方法比较](../../../12-Comparisons/PEFT-Methods-Comparison.md)。

## 技术定义与解决的问题

为 LoRA 两个低秩矩阵设置不同学习率的改进策略。 在固定 LoRA 参数化下改变优化动力学；需按论文条件比较，而非默认加速或提效。

## 核心机制、更新范围与使用

与 LoRA 共享低秩结构，重点在优化设置。 具体可训练参数、训练流程、显存与适用模型必须结合原始论文及实际实现确认；此页只作分类和入门索引，不给未经验证的参数比例或硬件门槛。

## 资料与待核项

[相关官方资料或论文入口](https://huggingface.co/docs/peft/developer_guides/lora)（链接入口，**本页未逐项核实全文与实现版本**；访问核实状态见 [SOURCES](../../../SOURCES.md)）。TODO：补原论文核读、公式、框架 API、适用模型与同预算实验。推荐先读 [LoRA](LoRA.md)，再研究本方法。

### 对应原始论文

- [LoRA+: Efficient Low Rank Adaptation of Large Models](https://arxiv.org/abs/2402.12354)。2026-11-05 已核对论文标题与摘要入口；公式、实验及实现细节仍待核读全文。
