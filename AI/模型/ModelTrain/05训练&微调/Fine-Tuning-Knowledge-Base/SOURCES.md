# 官方资料与原始论文

> 摘要：记录知识库核心内容的一手依据、实际访问状态与核实日期；链接可访问不等于代码已运行。学习优先级：P0。
> 核实日期：2026-11-05（Asia/Singapore）。“已打开”表示当天通过 ego-browser 加载并检查页面标题或正文。动态文档的 API 仍需按运行环境版本重新核对。

## 使用规则

- 概念、API 和平台能力优先依据官方文档；算法主张与数学表达式回到原始论文。本库中文解释是归纳，不照抄教程。
- Hugging Face 默认文档 URL 常指向动态版本。复现实验应记录本机依赖版本并切换至匹配文档；本轮未确认任何可运行的依赖组合。
- 论文报告的性能、显存和硬件数字只适用于其特定实验条件，不能当作本库的预期结果。
- 页面可访问不能单独证明持续维护；项目状态需检查官方仓库发布与提交记录。

## 核心官方文档（已打开）

| 标题 | 来源 URL | 核实结果与用途 |
| --- | --- | --- |
| PyTorch documentation | [PyTorch 官方文档](https://docs.pytorch.org/docs/stable/index.html) | 已打开；页面标示 v2.15.0 (stable)。张量、自动微分、训练循环与分布式基础。 |
| Transformers | [Hugging Face 官方文档](https://huggingface.co/docs/transformers/index) | 已打开；动态版本页面。模型加载、配置、Tokenizer 与任务接口。 |
| Trainer | [Transformers 官方 API](https://huggingface.co/docs/transformers/main_classes/trainer) | 已打开；动态版本页面。Trainer、TrainingArguments 参数须按安装版本核对。 |
| Datasets | [Hugging Face 官方文档](https://huggingface.co/docs/datasets/index) | 已打开；动态版本页面。数据集加载、转换、划分。 |
| PEFT | [Hugging Face 官方文档](https://huggingface.co/docs/peft/index) | 已打开；页面显示 v0.21.0。参数高效适配库，可与 Transformers、Diffusers、Accelerate 集成。 |
| LoRA API | [PEFT 官方 API](https://huggingface.co/docs/peft/package_reference/lora) | 已打开；动态版本页面。LoraConfig 和变体选项；旧版本不一定支持全部选项。 |
| PEFT Quantization | [PEFT 官方指南](https://huggingface.co/docs/peft/developer_guides/quantization) | 已打开；动态版本页面。量化模型与 PEFT 配合的实践入口。 |
| bitsandbytes | [Hugging Face 官方文档](https://huggingface.co/docs/bitsandbytes/index) | 已打开；动态版本页面。低精度量化与相关优化器的工具定位。 |
| Transformers bitsandbytes | [Transformers 官方指南](https://huggingface.co/docs/transformers/quantization/bitsandbytes) | 已打开；动态版本页面。BitsAndBytesConfig 接口需按安装版本核对。 |
| TRL SFT Trainer | [TRL 官方文档](https://huggingface.co/docs/trl/sft_trainer) | 已打开；动态版本页面。SFTTrainer 的工作流及配置入口。 |
| Accelerate | [Hugging Face 官方文档](https://huggingface.co/docs/accelerate/index) | 已打开；动态版本页面。训练设备与分布式执行抽象。 |
| LlamaFactory | [项目官方文档](https://llamafactory.readthedocs.io/en/latest/) | 已打开；首页有数据准备、SFT、保存/合并、推理、评估与分布式训练导航。 |
| ms-swift | [项目官方 GitHub](https://github.com/modelscope/ms-swift) | 已打开；可见代码、文档与近期提交。维护状态只在核实日成立。 |

原始的 [PyTorch 文档入口](https://pytorch.org/docs/stable/index.html) 首次访问时浏览器未能读取正文；以上 PyTorch 条目依据实际打开的 docs.pytorch.org 地址。[TRL 首页](https://huggingface.co/docs/trl/index) 导航超时，故采用已成功打开的 SFT Trainer 页面。

## 原始论文（已打开摘要页）

| 标题 | 来源 URL | 核心事实与使用范围 |
| --- | --- | --- |
| Attention Is All You Need | [Vaswani 等，arXiv:1706.03762](https://arxiv.org/abs/1706.03762) | Transformer 以注意力机制构建编码器—解码器架构。注意力公式还需对照论文正文。 |
| LoRA: Low-Rank Adaptation of Large Language Models | [Hu 等，arXiv:2106.09685](https://arxiv.org/abs/2106.09685) | 冻结预训练权重，注入可训练低秩更新矩阵。 |
| QLoRA: Efficient Finetuning of Quantized LLMs | [Dettmers 等，arXiv:2305.14314](https://arxiv.org/abs/2305.14314) | 通过冻结的 4-bit 量化基座反向传播至 LoRA 参数；论文还提出 NF4 等显存节省技术。 |
| AdaLoRA: Adaptive Budget Allocation for Parameter-Efficient Fine-Tuning | [Zhang 等，arXiv:2303.10512](https://arxiv.org/abs/2303.10512) | 动态分配低秩参数预算，避免对所有矩阵均匀分配秩。 |
| DoRA: Weight-Decomposed Low-Rank Adaptation | [Liu 等，arXiv:2402.09353](https://arxiv.org/abs/2402.09353) | 将权重更新拆分为 magnitude 与 direction，以改善 LoRA 与全量微调间的表现差距。 |

以上论文摘要页可访问；不代表已逐页校对 PDF 全部数学证明与实验设置。所有条目均于 2026-11-05 访问。

## 后续资料核查清单

以下为用户指定或目录规划所需的一手入口，**本轮尚未逐页打开核实**。扩写对应笔记前应核实 URL、版本、维护状态和 API：

- [PyTorch Tutorials](https://pytorch.org/tutorials/)、[PyTorch Distributed](https://docs.pytorch.org/docs/stable/distributed.html)、[DeepSpeed](https://www.deepspeed.ai/)、[DeepSpeed Tutorials](https://www.deepspeed.ai/tutorials/)。
- [TensorFlow](https://www.tensorflow.org/)、[JAX](https://docs.jax.dev/)、[Tokenizers](https://huggingface.co/docs/tokenizers)。
- [Axolotl](https://docs.axolotl.ai/)、[Unsloth](https://docs.unsloth.ai/)、[torchtune](https://meta-pytorch.org/torchtune/)。
- [vLLM](https://docs.vllm.ai/)、[ONNX Runtime](https://onnxruntime.ai/docs/)、[TensorRT-LLM](https://nvidia.github.io/TensorRT-LLM/)。
- [MLflow](https://mlflow.org/docs/latest/)、[Weights & Biases](https://docs.wandb.ai/)、[TensorBoard](https://www.tensorflow.org/tensorboard)。
- [Prefix-Tuning 原论文](https://arxiv.org/abs/2101.00190)、[Prompt Tuning 原论文](https://arxiv.org/abs/2104.08691)、[IA³ 原论文](https://arxiv.org/abs/2205.05638)、[ReFT 原论文](https://arxiv.org/abs/2404.03592)。

## 待处理的来源问题

- TODO：为 rsLoRA、LoRA+、DyLoRA、VeRA、PiSSA、LoftQ、MiLoRA 等逐一核实原论文和官方实现状态。
- TODO：为各框架记录可复现的兼容版本组合和平台条件。
- TODO：核实低使用量项目的最近发布、提交和文档更新日期，避免以“可访问”代替“仍维护”。

## 补充原始论文入口（2026-11-05）

以下链接已打开原始论文摘要页（FISH Mask 为 NeurIPS 论文 PDF）；本次仅核对题名、入口和与方法的对应关系，尚未逐篇核读全文。

| 方法 | 原始资料 | 状态 |
| --- | --- | --- |
| DyLoRA | [DyLoRA: Parameter Efficient Tuning of Pre-trained Models using Dynamic Search-Free Low-Rank Adaptation](https://arxiv.org/abs/2210.07558) | 入口已核对；全文待读 |
| VeRA | [VeRA: Vector-based Random Matrix Adaptation](https://arxiv.org/abs/2310.11454) | 入口已核对；全文待读 |
| PiSSA | [PiSSA: Principal Singular Values and Singular Vectors Adaptation of Large Language Models](https://arxiv.org/abs/2404.02948) | 入口已核对；全文待读 |
| LoftQ | [LoftQ: LoRA-Fine-Tuning-Aware Quantization for Large Language Models](https://arxiv.org/abs/2310.08659) | 入口已核对；全文待读 |
| MiLoRA | [MiLoRA: Harnessing Minor Singular Components for Parameter-Efficient LLM Finetuning](https://arxiv.org/abs/2406.09044) | 入口已核对；全文待读 |
| LoRA-Plus | [LoRA+: Efficient Low Rank Adaptation of Large Models](https://arxiv.org/abs/2402.12354) | 入口已核对；全文待读 |
| rsLoRA | [A Rank Stabilization Scaling Factor for Fine-Tuning with LoRA](https://arxiv.org/abs/2312.03732) | 入口已核对；全文待读 |
| OFT | [Parameter-Efficient Orthogonal Finetuning via Butterfly Factorization（OFT/BOFT）](https://arxiv.org/abs/2311.06243) | 入口已核对；全文待读 |
| BOFT | [Parameter-Efficient Orthogonal Finetuning via Butterfly Factorization](https://arxiv.org/abs/2311.06243) | 入口已核对；全文待读 |
| FourierFT | [Parameter-Efficient Fine-Tuning with Discrete Fourier Transform](https://arxiv.org/abs/2405.03003) | 入口已核对；全文待读 |
| FishMask | [Training Neural Networks with Fixed Sparse Masks（FISH Mask）](https://papers.nips.cc/paper/2021/file/cb2653f548f8709598e8b5156738cc51-Paper.pdf) | 入口已核对；全文待读 |
