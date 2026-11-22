# Framework Ecosystem

> 一句话摘要：从底层计算到数据、模型、训练执行与推理服务，分别看清软件工具的职责。**优先级：P0**。前置：[知识地图](../KNOWLEDGE-MAP.md)；后续：[框架比较](../12-Comparisons/Framework-Comparison.md)。

```mermaid
flowchart TB
    HW[GPU / CPU 硬件] --> CUDA[CUDA GPU 计算平台]
    CUDA --> PT[PyTorch 深度学习框架]
    PT --> TF[Transformers 模型库]
    PT --> DS[Datasets / Tokenizers 数据工具]
    TF --> PEFT[PEFT 适配算法库]
    TF --> TRAIN[Trainer 通用训练 API]
    TF --> TRL[TRL 后训练工具]
    PT --> ACC[Accelerate 设备与执行抽象]
    ACC --> FSDP[FSDP 分片数据并行]
    ACC --> DEEP[DeepSpeed 分布式优化系统]
    PT --> BNB[bitsandbytes 低精度工具]
    TF --> PLAT[LLaMA-Factory / ms-swift 集成工具链]
    PEFT --> PLAT
    TRL --> PLAT
    TRAIN --> PLAT
    ACC --> PLAT
    PLAT --> CKPT[模型 / Adapter 产物]
    CKPT --> VLLM[vLLM 推理引擎]
    CKPT --> ONNX[ONNX Runtime 推理运行时]
    TRAIN --> TRACK[MLflow / W&B / TensorBoard 实验追踪]
```

箭头表示典型使用关系，不表示所有工具必须同时安装，也不承诺每个工具仅有一种后端。**FSDP 是分片执行技术，CUDA 是计算平台，LoRA 是方法，Trainer 是 API，vLLM 是推理引擎**；它们不能被当作同一类“训练框架”。更多交叉比较见[Framework Comparison](../12-Comparisons/Framework-Comparison.md)。

## 官方入口

[PyTorch](https://docs.pytorch.org/docs/stable/index.html) · [Transformers](https://huggingface.co/docs/transformers) · [PEFT](https://huggingface.co/docs/peft) · [Accelerate](https://huggingface.co/docs/accelerate) · [DeepSpeed](https://www.deepspeed.ai/) · [vLLM](https://docs.vllm.ai/)。访问记录见 [SOURCES](../SOURCES.md)。
