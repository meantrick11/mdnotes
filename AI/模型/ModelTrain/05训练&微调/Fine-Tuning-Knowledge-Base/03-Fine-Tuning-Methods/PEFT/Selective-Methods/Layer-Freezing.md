# Layer Freezing

> 一句话摘要：冻结选定层，只更新其余原模型参数。 **优先级：P2**。前置：[方法总览](../README.md)；后续：[方法比较](../../../12-Comparisons/PEFT-Methods-Comparison.md)。

## 技术定义与解决的问题

冻结选定层，只更新其余原模型参数。 有助于控制训练状态成本，也可能限制表示变化；解冻策略需要消融。

## 核心机制、更新范围与使用

它与 Partial FT 交叉，纳入 PEFT 是宽泛分类，不是独立新算法。 具体可训练参数、训练流程、显存与适用模型必须结合原始论文及实际实现确认；此页只作分类和入门索引，不给未经验证的参数比例或硬件门槛。

## 资料与待核项

[相关官方资料或论文入口](https://huggingface.co/docs/transformers/training)（链接入口，**本页未逐项核实全文与实现版本**；访问核实状态见 [SOURCES](../../../SOURCES.md)）。TODO：补原论文核读、公式、框架 API、适用模型与同预算实验。推荐先读 [LoRA](../Low-Rank-Adaptation/LoRA.md)，再研究本方法。
