# Framework Comparison

> 一句话摘要：按职责而非流行度比较微调软件栈中的工具。**优先级：P0**。前置：[生态图](../04-Framework-Ecosystem/Framework-Ecosystem.md)；后续：各工具笔记。

| 工具 / 类型 | 核心职责 | PyTorch 关系 | 直接训练 | 实现微调算法 | 分布式 | 推理 | 典型关系与场景 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| PyTorch / 计算框架 | 张量、Autograd、模块与优化 | 自身 | 是，需训练循环 | 不专属实现 PEFT | 原生支持 | 是，基础前向 | 自定义训练底座 |
| CUDA / 计算平台 | NVIDIA GPU 计算与内核 | 可作为 PyTorch 后端 | 否 | 否 | GPU 通信有相关组件 | 是，底层 | 硬件/驱动层，非 Python 训练框架 |
| Transformers / 模型库 | 模型结构、预训练权重加载、任务接口 | 常用 PyTorch 后端，亦有其他后端视版本 | 经 Trainer 或自写循环 | 不以 PEFT 为主 | 结合 Accelerate 等 | 是 | 模型与 tokenizer 核心层 |
| Datasets / 数据工具 | 加载、转换、缓存数据集 | 不要求模型依赖 PyTorch | 否 | 否 | 支持数据处理场景 | 否 | 给 Trainer 或自写循环供数据 |
| PEFT / 算法库 | LoRA 等适配器配置和注入 | 主要配合 PyTorch 模型 | 需训练 API/循环 | 是 | 借助执行层 | 可加载适配器推理 | 与 Transformers、TRL 组合 |
| bitsandbytes / 低精度工具 | 量化层与优化器 | 常与 PyTorch 集成 | 需训练 API/循环 | 否，量化并非微调目标 | 借助执行层 | 支持量化加载 | QLoRA 中压缩冻结基座 |
| Trainer / 训练 API | 标准训练、评估与 checkpoint 循环 | 基于 Transformers/PyTorch 常用路径 | 是 | 需 PEFT 等 | 可集成 Accelerate | 可评估/预测 | 通用监督任务 |
| TRL / 后训练工具 | SFT、偏好优化等目标的训练器 | 基于 PyTorch/Transformers 生态 | 是 | 目标算法；PEFT 借助 PEFT 库 | 可集成 Accelerate | 非主职责 | LLM 后训练 |
| Accelerate / 执行抽象 | 设备、混合精度、分布式启动 | 基于 PyTorch | 为循环提供执行层 | 否 | 是 | 非主职责 | 将单卡代码扩展到多设备 |
| DeepSpeed / 分布式系统 | ZeRO、并行、卸载等 | 与 PyTorch 集成 | 是，需模型与循环 | 否 | 是 | 有推理能力，版本核实 | 大模型训练资源优化 |
| FSDP / 分片技术 | 参数、梯度、状态分片 | PyTorch 内置 | 需训练循环 | 否 | 是 | 非主职责 | 多卡显存分摊 |
| LLaMA-Factory / 集成工具 | 配置化微调、评估、导出 | 基于 PyTorch 生态 | 是 | 集成 PEFT 等 | 集成执行后端 | 有推理/导出流程 | 快速配置实验 |
| ms-swift / 集成工具 | 模型训练、微调、推理流程 | 基于 PyTorch 生态 | 是 | 集成 PEFT 等 | 集成执行后端 | 是 | ModelScope 生态工作流 |
| vLLM / 推理引擎 | 高吞吐 LLM serving | 使用 PyTorch 生态，非训练依赖层 | 否 | 否 | 推理并行 | 是 | 部署模型产物 |

“是/否”表示主要公开职责，不保证某一版本的全部特性或平台兼容性。部署栈、硬件支持与 API 应在实际项目锁定版本后重新核实。官方来源总表见 [SOURCES](../SOURCES.md)。

## 官方入口

[PyTorch](https://docs.pytorch.org/docs/stable/index.html) · [Transformers](https://huggingface.co/docs/transformers) · [PEFT](https://huggingface.co/docs/peft) · [Trainer](https://huggingface.co/docs/transformers/main_classes/trainer) · [TRL](https://huggingface.co/docs/trl) · [Accelerate](https://huggingface.co/docs/accelerate) · [DeepSpeed](https://www.deepspeed.ai/) · [vLLM](https://docs.vllm.ai/)。
