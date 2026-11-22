# DyLoRA

> 一句话摘要：探索在训练中支持不同有效 rank 的低秩适配。 **优先级：P2**。前置：[方法总览](../README.md)；后续：[方法比较](../../../12-Comparisons/PEFT-Methods-Comparison.md)。

## 技术定义与解决的问题

探索在训练中支持不同有效 rank 的低秩适配。 用于需要在同一适配器预算下比较 rank 的场景；具体采样与排序机制待核原论文。

## 核心机制、更新范围与使用

不同于 AdaLoRA 的按模块重要性分配预算。 具体可训练参数、训练流程、显存与适用模型必须结合原始论文及实际实现确认；此页只作分类和入门索引，不给未经验证的参数比例或硬件门槛。

## 资料与待核项

[相关官方资料或论文入口](https://huggingface.co/docs/peft)（链接入口，**本页未逐项核实全文与实现版本**；访问核实状态见 [SOURCES](../../../SOURCES.md)）。TODO：补原论文核读、公式、框架 API、适用模型与同预算实验。推荐先读 [LoRA](LoRA.md)，再研究本方法。

### 对应原始论文

- [DyLoRA: Parameter Efficient Tuning of Pre-trained Models using Dynamic Search-Free Low-Rank Adaptation](https://arxiv.org/abs/2210.07558)。2026-11-05 已核对论文标题与摘要入口；公式、实验及实现细节仍待核读全文。
