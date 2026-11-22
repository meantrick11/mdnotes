# LLM Fine-Tuning Knowledge Base

> 一句话摘要：按训练目标、参数更新、软件工具与部署阶段建立可扩展的微调知识地图。**优先级：P0**。

本库面向具备 Python 基础、正在学习 ML/DL/LLM 的工程学习者。建议先读[学习路线](LEARNING-ROADMAP.md)，再用[知识地图](KNOWLEDGE-MAP.md)定位概念，查阅[术语表](GLOSSARY.md)和[资料索引](SOURCES.md)。目前 Phase 1–3 为建设重点；未完成章节与未运行实验均明确标记。

## 核心分类

| 维度 | 回答的问题 | 示例 | 入口 |
| --- | --- | --- | --- |
| Training Paradigm | 用什么数据与目标训练？ | CPT、SFT、DPO、RLHF | [训练范式](01-Foundations/Training-Paradigms.md) |
| Model Architecture | 模型怎样计算？ | Encoder-only、Decoder-only | [模型架构](02-Model-Architectures/README.md) |
| Fine-tuning Method | 哪些参数更新？ | Full FT、LoRA、Prompt Tuning | [方法分类](03-Fine-Tuning-Methods/README.md) |
| PEFT | 少量可训练参数的方法族 | LoRA、Adapter、IA³ | [PEFT](03-Fine-Tuning-Methods/PEFT/README.md) |
| Training Optimization | 如何节约计算与显存？ | BF16、激活重计算、ZeRO | [优化](05-Training-Optimization/README.md) |
| Framework / Tool | 软件各负责什么？ | PyTorch、Transformers、PEFT | [框架生态](04-Framework-Ecosystem/README.md) |
| Deployment | 如何交付推理服务？ | 合并适配器、量化、vLLM | [部署](10-Deployment/README.md) |

同一实验可同时是 **SFT + LoRA + BF16 + FSDP**：它们分别描述训练目标、更新策略、精度与执行方式，并非互斥选项。QLoRA 是量化基座与 LoRA 训练的组合；LoftQ 是面向量化误差的 LoRA 初始化/联合设计，详见各自笔记。

## 目录导航

| 阶段 | 目录 | 作用 |
| --- | --- | --- |
| 理论 | [01 Foundations](01-Foundations/README.md)、[02 Model Architectures](02-Model-Architectures/README.md) | 理解训练目标与网络结构 |
| 方法 | [03 Fine-Tuning Methods](03-Fine-Tuning-Methods/README.md) | 选择参数更新策略 |
| 工具 | [04 Framework Ecosystem](04-Framework-Ecosystem/README.md) | 组合训练软件栈 |
| 工程 | [05 Optimization](05-Training-Optimization/README.md)、[06 Data](06-Data-Engineering/README.md)、[07 Training](07-Training-Engineering/README.md)、[08 Compute](08-Compute-and-Memory/README.md) | 训练与资源规划 |
| 验证交付 | [09 Evaluation](09-Evaluation/README.md)、[10 Deployment](10-Deployment/README.md) | 评估和服务化 |
| 应用 | [11 Projects](11-Practical-Projects/README.md)、[12 Comparisons](12-Comparisons/README.md) | 实践与选型 |

## 学习与实践

按 [P0 → P1 → P2 学习路线](LEARNING-ROADMAP.md)推进。项目入口预留 BERT Full FT、BERT LoRA、Qwen QLoRA；**训练、推理和评估尚未运行验证**，不能将目录规划当作可运行实验。

## 资料与维护

优先查阅[官方资料和原论文](SOURCES.md)。软件 API 会随版本变化；示例应记录实际锁定版本、硬件、数据集版本和运行日期后再宣称可复现。现有目录外的旧笔记未纳入本库的准确性背书。

### 直接开始学习

- [PyTorch Learn the Basics（官方教程）](https://pytorch.org/tutorials/beginner/basics/intro.html)：张量、数据集、模型、Autograd 和优化循环。
- [Transformers 官方文档](https://huggingface.co/docs/transformers)：模型、Tokenizer 与训练接口。
- [PEFT 官方文档](https://huggingface.co/docs/peft)：LoRA 等适配器的配置、训练与保存。
- [Datasets 官方文档](https://huggingface.co/docs/datasets)：数据集加载、处理与划分。
- [TRL 官方文档](https://huggingface.co/docs/trl)：SFT 与偏好优化相关训练器。
