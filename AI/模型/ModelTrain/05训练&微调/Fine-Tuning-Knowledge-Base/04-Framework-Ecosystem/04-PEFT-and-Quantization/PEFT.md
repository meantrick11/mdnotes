# PEFT 软件库

> 一句话摘要：Hugging Face PEFT 将 LoRA 等参数高效适配方法实现为可配置模块。**优先级：P0**。前置：[PEFT 方法族](../../03-Fine-Tuning-Methods/PEFT/README.md)、[Transformers](../02-Model-Libraries/Transformers.md)；后续：[LoRA](../../03-Fine-Tuning-Methods/PEFT/Low-Rank-Adaptation/LoRA.md)。

## 是什么、层次与职责

这里的 PEFT 指**软件库**，区别于 PEFT 方法族。它负责选择任务类型、目标模块、适配器配置、注入、保存和加载；训练循环仍由 PyTorch、Trainer、TRL 等执行。并非所有研究论文方法都在任意 PEFT 版本受支持，具体支持范围以[官方文档](https://huggingface.co/docs/peft)为准。

```mermaid
flowchart LR
    MODEL[基础模型] --> CFG[PeftConfig / LoraConfig]
    CFG --> INJECT[get_peft_model]
    INJECT --> TRAIN[Trainer / TRL / PyTorch 循环]
    TRAIN --> SAVE[保存 Adapter]
    SAVE --> LOAD[搭配相同基座加载]
```

## 关键 API 与示例

以下只演示对随机初始化模型注入 LoRA，需兼容的 `torch`、`transformers`、`peft`；未在本次执行。

```python
from transformers import BertConfig, BertForSequenceClassification
from peft import LoraConfig, TaskType, get_peft_model

base = BertForSequenceClassification(BertConfig(
    vocab_size=100, hidden_size=32, num_hidden_layers=1,
    num_attention_heads=4, intermediate_size=64, num_labels=2))
config = LoraConfig(task_type=TaskType.SEQ_CLS, r=4, lora_alpha=8,
                    target_modules=["query", "value"])
model = get_peft_model(base, config)
model.print_trainable_parameters()
```

`query`/`value` 对应这里的 BERT 实现；换模型必须检查模块名和分类头是否仍可训练。保存适配器后需记录基座模型标识及 revision，不能单凭 Adapter 文件复原所有权重。

## 安装、优缺点与资料

使用[官方安装说明](https://huggingface.co/docs/peft/installation)确认当前依赖和支持环境。优势是统一适配器生命周期；限制是版本、模型结构、量化组合和合并兼容性需要实测。[PEFT 官方文档](https://huggingface.co/docs/peft)、[LoRA 指南](https://huggingface.co/docs/peft/developer_guides/lora)。访问日期：2026-11-05。
