# 创建Skills

## 编写SKILL.md

一个标准的SKILL.md是以名称和描述开始文件的，比如：

```
---
name: tabletop-dice
description: Roll one or more dice for tabletop games and report each result and the total.
---

Use this skill when the user asks to roll dice.

1. Parse requests written as `NdS` as N dice with S sides. For example, `3d6`
   means three six-sided dice.
2. Call `roll_dice` once for each requested die and pass S as `sides`.
3. Report each tool result in order.
4. When the user requests multiple dice, add the results and report the total.

Do not invent, replace, or reroll a result unless the user asks you to.
```


**描述决定了模型何时考虑使用该Skill。陈述工作流以及触发它的条件。详细说明流程、格式、以及正文重的安全说明**


## 定义工作流程边界

将每个skill与多个用例联系起来：

- 工作流预期输入
- 模型应该遵循的步骤
- 用户/下游应该接收到什么输出
- 模型不能推断哪些事实
- 工作流程何时应该提问、停止或拒绝
- 模型应该查阅哪些相关支持的文件


## 添加支持的资源

SKILL.md尽可能保持间接，并将相关的材料放到一个Skill的其他文件夹下：
1. **references**:政策、格式/规范、例子、背景材料
2. **assets**：模板、工作流等等
3. **scripts**：需要计算脚本、文件操作等等


## 测试SKILL

需要根据一下维度进行测试脚本：

1. 直接调用SKILL测试
2. 间接调用SKILL测试
3. 不完整的输入
4. 测试不应该调用该SKILL的例子
5. SKILL的边缘，不应支持的信息&操作

