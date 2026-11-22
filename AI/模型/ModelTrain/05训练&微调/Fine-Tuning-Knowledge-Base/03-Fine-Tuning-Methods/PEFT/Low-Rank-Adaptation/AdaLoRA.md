# AdaLoRA

> 一句话摘要：AdaLoRA 按重要性在不同权重矩阵间动态分配低秩适配预算。**优先级：P1**。前置：[LoRA](LoRA.md)；后续：[方法比较](../../../12-Comparisons/PEFT-Methods-Comparison.md)。

## 定义、问题与原理

固定 rank 的 LoRA 对每个目标矩阵给同样容量，但层与模块的重要性可能不同。AdaLoRA 使用自适应预算分配，在训练过程中依据重要性调整分配。论文采用奇异值分解风格的参数化和重要性估计；它不是自动保证更优的“LoRA 升级版”。

$$\Delta W=P\Lambda Q,\qquad \sum_i r_i\le R,$$

其中 $r_i$ 是第 $i$ 个目标模块的有效秩，$R$ 是总预算；实际正交约束、剪枝和调度应以论文与实现为准。

```mermaid
flowchart LR
    B[全局 rank 预算] --> I[估计模块重要性]
    I --> A[动态分配有效 rank]
    A --> U[更新低秩参数]
    U --> I
```

## 更新范围、训练与资源

冻结基座，训练低秩分解参数；与固定 rank LoRA 相比，多了预算调度和重要性估计配置。适合目标层重要性不均、固定参数预算的研究；限制是超参数与训练流程更复杂，收益需同预算验证。显存仍要计入冻结基座和激活，不能仅由有效 rank 推算。

## 实现、区别与资料

HF PEFT 提供 `AdaLoraConfig`，具体字段随版本核实。与 LoRA 的核心差别是**预算分配随训练变化**，不是训练目标不同。原论文：[AdaLoRA: Adaptive Budget Allocation for Parameter-Efficient Fine-Tuning](https://arxiv.org/abs/2303.10512)；[PEFT 官方文档](https://huggingface.co/docs/peft)。访问日期：2026-11-05。推荐顺序：LoRA → rank 与模块选择 → AdaLoRA → 同预算比较。
