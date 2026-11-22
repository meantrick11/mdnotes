# Megatron-LM

> 一句话摘要：Megatron-LM 属于大模型并行训练系统，主要负责张量/流水线等模型并行研究与实现。**优先级：P2**。前置：[框架生态图](../Framework-Ecosystem.md)；后续：[框架比较](../../12-Comparisons/Framework-Comparison.md)。

## 框架是什么、层次与职责

Megatron-LM 的定位是**大模型并行训练系统**。张量/流水线等模型并行研究与实现。不要将它与训练目标（SFT/DPO）或参数更新算法（LoRA）混为同层概念。

## 使用场景与关系

选型时先明确任务、模型架构、硬件和交付目标，再确认它与 PyTorch/Transformers/执行后端的具体兼容关系。当前页仅定位，不提供未经版本核实的安装命令或完整训练示例。

## 官方资料与待办

[官方入口](https://github.com/NVIDIA/Megatron-LM)（链接入口；**本页未逐项核实最新维护状态、API 和 Quickstart**）。TODO：按实际选定版本补环境、最小可运行示例、组件图与局限；维护状态待核者尤其不能据此假定仍活跃。访问日期记录以 [SOURCES](../../SOURCES.md) 的已核条目为准。
