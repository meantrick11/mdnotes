# LoftQ

> 一句话摘要：面向量化权重与 LoRA 适配器的联合设计/初始化，以减轻量化误差。 **优先级：P2**。前置：[方法总览](../README.md)；后续：[方法比较](../../../12-Comparisons/PEFT-Methods-Comparison.md)。

## 技术定义与解决的问题

面向量化权重与 LoRA 适配器的联合设计/初始化，以减轻量化误差。 量化条件是方法的一部分，不能只当普通 LoRA 变体；具体初始化流程依实现核对。

## 核心机制、更新范围与使用

QLoRA 侧重量化冻结基座上训练 LoRA，LoftQ 特别关注初始化时的量化误差。 具体可训练参数、训练流程、显存与适用模型必须结合原始论文及实际实现确认；此页只作分类和入门索引，不给未经验证的参数比例或硬件门槛。

## 资料与待核项

[相关官方资料或论文入口](https://huggingface.co/docs/peft/developer_guides/quantization)（链接入口，**本页未逐项核实全文与实现版本**；访问核实状态见 [SOURCES](../../../SOURCES.md)）。TODO：补原论文核读、公式、框架 API、适用模型与同预算实验。推荐先读 [LoRA](LoRA.md)，再研究本方法。

### 对应原始论文

- [LoftQ: LoRA-Fine-Tuning-Aware Quantization for Large Language Models](https://arxiv.org/abs/2310.08659)。2026-11-05 已核对论文标题与摘要入口；公式、实验及实现细节仍待核读全文。
