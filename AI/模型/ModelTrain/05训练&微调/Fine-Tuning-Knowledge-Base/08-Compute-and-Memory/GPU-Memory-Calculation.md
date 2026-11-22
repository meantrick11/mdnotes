# GPU 显存计算

> 一句话摘要：峰值显存应分项估算：模型权重、梯度、优化器状态、训练激活、临时工作区及可能的 KV cache。**优先级：P1**。前置：[知识库总览](../README.md)；后续：按[学习路线](../LEARNING-ROADMAP.md)继续。

## 概念与工程意义

峰值显存应分项估算：模型权重、梯度、优化器状态、训练激活、临时工作区及可能的 KV cache。权重理论下界为参数量乘每参数字节；量化元数据、未量化层、batch/序列长度和分片方式会改变实际峰值。

## 资料与建设状态

[官方资料或原论文入口](https://huggingface.co/docs/transformers/quantization/bitsandbytes)。本页为后续阶段的准确定位索引，**尚未按完整笔记模板扩写，也未运行代码或实验**；链接访问状态以 [SOURCES](../SOURCES.md) 为准。
