# PiSSA

> 一句话摘要：以主奇异方向相关初始化改进低秩适配。 **优先级：P2**。前置：[方法总览](../README.md)；后续：[方法比较](../../../12-Comparisons/PEFT-Methods-Comparison.md)。

## 技术定义与解决的问题

以主奇异方向相关初始化改进低秩适配。 初始化依赖基座权重的分解；代价和量化适配效果需在实际模型验证。

## 核心机制、更新范围与使用

与 LoRA 的核心增量形式相关，差异主要在初始化。 具体可训练参数、训练流程、显存与适用模型必须结合原始论文及实际实现确认；此页只作分类和入门索引，不给未经验证的参数比例或硬件门槛。

## 资料与待核项

[相关官方资料或论文入口](https://huggingface.co/docs/peft/developer_guides/lora)（链接入口，**本页未逐项核实全文与实现版本**；访问核实状态见 [SOURCES](../../../SOURCES.md)）。TODO：补原论文核读、公式、框架 API、适用模型与同预算实验。推荐先读 [LoRA](LoRA.md)，再研究本方法。

### 对应原始论文

- [PiSSA: Principal Singular Values and Singular Vectors Adaptation of Large Language Models](https://arxiv.org/abs/2404.02948)。2026-11-05 已核对论文标题与摘要入口；公式、实验及实现细节仍待核读全文。
