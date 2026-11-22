# 术语表

> 一句话摘要：统一常见中英文术语及其所属维度。**优先级：P0**。前置：无；后续：[知识地图](KNOWLEDGE-MAP.md)。

| 中文 / English | 含义与入口 |
| --- | --- |
| 训练范式 / Training Paradigm | 数据形式与训练目标的安排，如 [CPT、SFT、DPO](01-Foundations/Training-Paradigms.md) |
| 预训练 / Pretraining | 通用语料上的初始训练阶段 |
| 继续预训练 / Continued Pretraining, CPT | 在已有模型上继续语言建模训练 |
| 监督微调 / Supervised Fine-Tuning, SFT | 用标注输入输出对优化监督损失 |
| 人类反馈强化学习 / RLHF | 基于人类偏好信号的强化学习对齐流程 |
| 直接偏好优化 / DPO | 对偏好对直接优化策略的目标 |
| 全参数微调 / Full Fine-Tuning | 更新选定模型的全部可训练权重 |
| 参数高效微调 / PEFT | 仅训练相对少量参数的方法族，不等于单一算法 |
| 低秩适配 / LoRA | 冻结基座权重，用低秩矩阵表达增量 |
| 量化低秩适配 / QLoRA | 量化冻结基座并训练 LoRA 适配器 |
| 适配器 / Adapter | 插入小模块并主要训练这些模块的方法 |
| 软提示 / Soft Prompt | 可学习的连续向量提示，不等于手写自然语言 prompt |
| 分词器 / Tokenizer | 文本与 token ID 之间的编码/解码工具 |
| 梯度累积 / Gradient Accumulation | 多个 microbatch 累积梯度后更新一次 |
| 激活重计算 / Activation Checkpointing | 反向时重算部分前向激活以节省显存 |
| 分片数据并行 / FSDP | 分片参数、梯度和优化器状态的数据并行执行方式 |
| ZeRO | DeepSpeed 的冗余状态分片优化体系 |
| NF4 | QLoRA 使用的 4-bit NormalFloat 数值格式 |
| 混合精度 / Mixed Precision | 在运算中组合不同精度及必要的数值稳定措施 |
| 困惑度 / Perplexity | 语言模型 token 预测负对数似然的指数形式 |
| 检查点 / Checkpoint | 训练或推理所需的模型状态及可能的优化器状态 |
| 推理服务 / Inference Serving | 对外提供模型推理能力的运行服务 |

术语分类会随论文与工具生态变化；本表优先说明其在本知识库中的检索位置，不宣称唯一分类体系。

## 官方术语入口

[PyTorch 官方文档](https://docs.pytorch.org/docs/stable/index.html)、[Transformers 官方文档](https://huggingface.co/docs/transformers)、[PEFT 官方文档](https://huggingface.co/docs/peft)、[TRL 官方文档](https://huggingface.co/docs/trl)。单个术语的具体论文和 API 请按表内笔记追溯。
