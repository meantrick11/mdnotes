# Datasets

> 一句话摘要：Hugging Face Datasets 管理数据加载、映射、缓存与切分，不决定训练目标。**优先级：P0**。前置：[数据集构建](../../06-Data-Engineering/Dataset-Construction.md)；后续：[Tokenizers](Tokenizers.md)、[Trainer](../05-Training-Frameworks/Trainer.md)。

## 层次、职责与流程

它是数据处理库，可把本地或远端数据转成 `Dataset` / `DatasetDict`，用 `map` 编码字段，再提供给 PyTorch DataLoader 或 Trainer。Arrow 是其常用表格存储基础，不能把 Arrow 与 Datasets 当成同类“训练框架”。

```mermaid
flowchart LR
    RAW[原始样本] --> VALID[字段与标签校验]
    VALID --> SPLIT[先划分 train/validation/test]
    SPLIT --> MAP[map tokenization]
    MAP --> BATCH[Collator / DataLoader]
    BATCH --> TRAIN[训练与评估]
```

## API 与可运行最小例子

下例用内存数据，不访问网络；需安装 `datasets`，本次未执行。

```python
from datasets import Dataset

ds = Dataset.from_dict({"text": ["好", "坏"], "label": [1, 0]})
ds = ds.map(lambda batch: {"length": [len(x) for x in batch["text"]]}, batched=True)
print(ds.column_names, len(ds))
```

真实项目要保留数据来源、许可证、版本与去重策略；同一文档的切片应避免跨训练/测试泄漏。`train_test_split` 可作基础划分，但时间/实体相关数据需要任务特定的分组策略。

## 安装、局限与资料

按[官方安装页](https://huggingface.co/docs/datasets/installation)选择环境。常用 API：`load_dataset`、`Dataset.from_dict`、`map`、`filter`。缓存可加速重复处理，但需要关注磁盘占用与转换函数的可复现性。[Datasets 官方文档](https://huggingface.co/docs/datasets)、[Processing](https://huggingface.co/docs/datasets/process)。访问日期：2026-11-05。
