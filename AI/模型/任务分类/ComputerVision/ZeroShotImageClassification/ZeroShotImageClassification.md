# Zero-Shot Image Classification（零样本图像分类）

零样本图像分类根据给定的候选类别判断图片内容，即使这些类别没有作为该任务的训练标签出现过。模型利用已学到的视觉与语义知识进行匹配。

## 例子

页面给出候选类别 `cat, dog, bird`；对示例图片，模型把 `Cat` 排在首位，得分为 `0.664`。

## 应用场景

为不断新增的图片类别建立检索和分类能力，而不必每新增一类就收集完整的标注图片集。

来源：[Hugging Face：Zero-Shot Image Classification](https://huggingface.co/tasks/zero-shot-image-classification)
