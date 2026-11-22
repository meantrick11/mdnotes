# 微调方法比较

> 一句话摘要：Full FT 更新基座全部可训练参数；Partial FT 只更新选定原参数；LoRA 更新低秩增量；Adapter 训练插入模块；Prompt Tuning 训练软提示。**优先级：P1**。前置：[知识库总览](../README.md)；后续：按[学习路线](../LEARNING-ROADMAP.md)继续。

## 概念与工程意义

Full FT 更新基座全部可训练参数；Partial FT 只更新选定原参数；LoRA 更新低秩增量；Adapter 训练插入模块；Prompt Tuning 训练软提示。比较必须控制训练目标、数据、预算与评估方式。

## 资料与建设状态

[官方资料或原论文入口](https://huggingface.co/docs/peft)。本页为后续阶段的准确定位索引，**尚未按完整笔记模板扩写，也未运行代码或实验**；链接访问状态以 [SOURCES](../SOURCES.md) 为准。
