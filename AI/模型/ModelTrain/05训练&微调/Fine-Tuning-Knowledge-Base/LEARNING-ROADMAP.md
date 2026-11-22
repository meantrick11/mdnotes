# 学习路线

> 一句话摘要：先建立能解释训练过程的 P0 能力，再补齐工程选型与扩展方法。**优先级：P0**。前置：Python；后续：[实践项目](11-Practical-Projects/README.md)。

## P0：理解并能手写基本训练流程

- **目标**：解释 Transformer 的计算、Full FT/LoRA/QLoRA 的更新范围，独立写出数据处理、loss、反向传播与评估流程。
- **前置**：Python、张量运算、基本线性代数与概率。
- **阅读顺序**：[PyTorch](04-Framework-Ecosystem/01-Deep-Learning-Frameworks/PyTorch.md) → [Transformer](01-Foundations/Transformer-Architecture.md) → [Encoder / Decoder](02-Model-Architectures/README.md) → [Full FT](03-Fine-Tuning-Methods/Full-Fine-Tuning/Full-FT.md) → [LoRA](03-Fine-Tuning-Methods/PEFT/Low-Rank-Adaptation/LoRA.md) → [QLoRA](03-Fine-Tuning-Methods/PEFT/Low-Rank-Adaptation/QLoRA.md) → [Transformers](04-Framework-Ecosystem/02-Model-Libraries/Transformers.md) → [PEFT](04-Framework-Ecosystem/04-PEFT-and-Quantization/PEFT.md) → [Datasets](04-Framework-Ecosystem/03-Data-Frameworks/Datasets.md) / [Tokenizers](04-Framework-Ecosystem/03-Data-Frameworks/Tokenizers.md) → [Training Loop](07-Training-Engineering/Training-Loop.md) / Loss / Optimizer。
- **官方教程**：[PyTorch Learn the Basics](https://pytorch.org/tutorials/beginner/basics/intro.html)、[Transformers 文档](https://huggingface.co/docs/transformers)、[PEFT 文档](https://huggingface.co/docs/peft)。
- **实践**：用小样本分类任务记录 train/validation loss，分别检查 Full FT 和 LoRA 的可训练参数数目；所有实验固定随机种子并记录数据划分。
- **完成标准**：能画出前向/反向路径、解释 `requires_grad` 与优化器参数集合，说明 QLoRA 基座为何通常冻结。

## P1：工程化选型与评估

- **目标**：选用训练 API、估算资源、识别数据泄漏并建立可比较实验。
- **前置**：P0。
- **阅读顺序**：[Trainer](04-Framework-Ecosystem/05-Training-Frameworks/Trainer.md) → [TRL](04-Framework-Ecosystem/05-Training-Frameworks/TRL.md) → [LLaMA-Factory](04-Framework-Ecosystem/09-End-to-End-Platforms/LLaMA-Factory.md) → [ms-swift](04-Framework-Ecosystem/09-End-to-End-Platforms/ms-swift.md) → [AdaLoRA](03-Fine-Tuning-Methods/PEFT/Low-Rank-Adaptation/AdaLoRA.md) / [DoRA](03-Fine-Tuning-Methods/PEFT/Low-Rank-Adaptation/DoRA.md) → [显存计算](08-Compute-and-Memory/GPU-Memory-Calculation.md) → [数据工程](06-Data-Engineering/README.md) → [评估](09-Evaluation/README.md)。
- **官方教程**：[TRL 文档](https://huggingface.co/docs/trl)、[LLaMA-Factory 文档](https://llamafactory.readthedocs.io/en/latest/)、[ms-swift 仓库](https://github.com/modelscope/ms-swift)。
- **实践**：对同一数据划分比较 Full FT 与 LoRA；记录准确率、F1、峰值显存和训练耗时，并注明硬件及软件版本。
- **完成标准**：能解释指标变化是否超过随机波动，追溯数据、配置和模型检查点。

## P2：建立方法与部署全景

- **目标**：知道何时进一步研究软提示、Adapter、其他 PEFT、分布式和推理优化。
- **前置**：P0；建议完成 P1 的一个对照实验。
- **阅读顺序**：[Prompt Tuning](03-Fine-Tuning-Methods/PEFT/Prompt-Based/Prompt-Tuning.md) / Prefix Tuning → [Adapter](03-Fine-Tuning-Methods/PEFT/Adapter-Based/Houlsby-Adapter.md) → IA³ / VeRA / PiSSA → [DeepSpeed](04-Framework-Ecosystem/06-Distributed-Training/DeepSpeed.md) / FSDP → [量化](05-Training-Optimization/Quantization/Quantization-Overview.md) → [vLLM](04-Framework-Ecosystem/08-Inference-and-Deployment/vLLM.md) / ONNX Runtime → [实验追踪](04-Framework-Ecosystem/07-Experiment-Tracking/README.md)。
- **官方教程**：[PyTorch Distributed](https://docs.pytorch.org/docs/stable/distributed.html)、[DeepSpeed Tutorials](https://www.deepspeed.ai/tutorials/)、[vLLM 文档](https://docs.vllm.ai/)。
- **实践**：对一个已训练模型制定部署方案，列出模型格式、量化条件、延迟/吞吐目标及回归测试。
- **完成标准**：能说明软件框架、算法、硬件与训练优化各自的边界，不凭模型参数量承诺显存或性能。
