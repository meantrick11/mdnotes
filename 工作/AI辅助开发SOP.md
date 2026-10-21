# AI 辅助开发 SOP：从需求到落地的可复用工作流

1. 配置项目agent规范：/setup-matt-pocock-skills

任务放在哪里：GitHub Issues、GitLab Issues，或本地 Markdown。
任务状态怎么标记：例如 ready-for-agent。
领域知识放在哪里：GLOSSARY.md 和 ADR，并把使用约定写入 AGENTS.md 或 CLAUDE.md。

2. grill-with-docs → to-spec → to-tickets → implement






> 核心原则：**不要让 AI 从"需求"直接跳到"代码"。** 中间必须经过 What → How → Where → Code 四层转换。

---

## 一、整体流程图

```
需求
 ↓
1. Specify   需求澄清 / 验收标准
 ↓
2. Explore   理解现有系统 / 影响分析
 ↓
3. Plan      技术方案 / 定边界
 ↓
4. Tasks     任务拆解
 ↓
5. Implement AI 实现
 ↓
6. Verify    测试 / Code Review
 ↓
7. Converge  对照验收标准收敛，而非"能跑就完事"
```

---

## 二、第一步：Specify（需求规格化）

**目标**：把主观描述变成可验证的行为。不要写"实现XX功能"，要写清楚四件事：

1. **用户是谁 / 场景是什么**
2. **现状行为**（当前系统怎么工作）
3. **新增/变化行为**
4. **验收标准**（Given / When / Then 格式）

**可复用模板**：
```
Given：[前置条件]
When：[触发动作]
Then：
1. [行为1]
2. [行为2]
3. [异常/边界情况的处理]
```

**关键经验**：验收标准要覆盖正常、边界、异常三种情况，且必须是"可检查"的，而不是"感觉写清楚了"。

---

## 三、第二步：Explore（先读项目，再动代码）

**目标**：建立"需求 → 系统组件"的映射，而不是让 AI 一上来就改代码。

**给 AI 的指令模板**：
```
先不要修改任何代码。阅读项目并告诉我这个需求会影响哪些部分。
```

**期望产出（影响分析）**：
```
本需求涉及：

Backend
├── api/xxx.py         → 新增/修改的接口
├── services/xxx.py    → 新增/修改的业务逻辑
├── agent/xxx.py        → 复用的已有能力

Database
└── 是否需要改动（明确写"不需要"或具体字段）

Frontend
├── xxx.vue            → UI 改动点
└── api/xxx.ts         → 接口调用

Tests
├── test_xxx.py
```

---

## 四、第三步：Plan（技术方案 + 划定边界）

### 4.1 技术方案要回答的问题
- 数据流是什么（画一条 A → B → C 的链路）
- 是否需要新增数据库字段/表
- 是否需要新增 API
- 是否需要改前端
- 能否复用已有模块，而不是重新造一个

### 4.2 划定边界（Scope / Out of Scope）—— 控制 AI 乱改代码的关键手段

```
Scope（这次要做的）：
✓ ...
✓ ...

Out of Scope（这次明确不做的）：
✗ 不修改数据库结构
✗ 不重构现有模块
✗ 不做架构升级
✗ 不增加额外功能
```

**为什么重要**：AI 很容易"顺手重构"，一旦没有边界约束，小需求可能演变成大范围改动甚至破坏项目。**Out of Scope 比 Scope 更重要。**

---

## 五、第四步：Tasks（拆解为可独立执行/验证的任务）

**原则**：每个 Task 应该足够小，能独立完成、独立测试。

**模板**：
```
- [ ] Task 1：新增 xxxService.xxx()
- [ ] Task 2：新增 API 接口
- [ ] Task 3：新增单元测试
- [ ] Task 4：前端调用接口
- [ ] Task 5：前端 UI/交互（含 loading、error 状态）
- [ ] Task 6：集成测试
- [ ] Task 7：Code Review
```

---

## 六、第五步：Implement（下达实现指令）

**给 AI Coding Agent 的标准指令模板**：
```
根据已经确定的 spec、plan 和 tasks，实现 Task N。

要求：
- 不修改无关代码
- 遵循现有项目结构
- 先查看相关实现，再动手改
- 完成后运行对应测试
- 汇报修改了哪些文件、测试结果如何
- 如果发现需求与现有架构冲突，不要自行扩大范围，先说明问题
```

**分工原则**：
- 人负责：需求、边界、架构、验收标准
- AI 负责：搜代码、写代码、补测试、跑命令、debug

---

## 七、第六步：Verify（测试 + Code Review）

### 7.1 测试要覆盖三类场景
```
正常情况：输入正常 → 得到正确结果
边界情况：空输入、不存在的资源、超长输入、并发/重复请求
异常情况：依赖服务超时、数据库失败、参数错误
```

### 7.2 让 AI 自己做一次 Code Review

**指令模板**：
```
现在不要继续修改代码。请作为 Code Reviewer，
根据本次 feature 的 specification、implementation plan 和实际 git diff，
对实现进行审查。
```

**Review 检查清单**：
- Requirement coverage：需求是否全部实现
- Architecture：是否破坏了现有架构
- Scope：是否修改了无关代码
- Error handling：异常处理是否完整
- Security：是否有安全隐患
- Performance：是否有明显性能问题
- Tests：测试覆盖是否足够
- Regression：是否破坏了原有功能

---

## 八、第七步：Converge（收敛，而非"跑起来就结束"）

发现问题 → 修改 → 重新测试 → 重新 Review，循环直到实现与 spec/plan/tasks 一致，且系统稳定。

---

## 九、每个阶段的"产物 + 检查标准"对照表

这是把"描述清不清楚"这种主观判断，转化为客观检查点的核心方法：

| 阶段 | 产物 | 检查标准 |
|---|---|---|
| 需求 | Spec | 能否判断功能对不对 |
| 系统理解 | Impact Analysis | 是否知道改哪里 |
| 方案 | Plan | 是否知道怎么实现 |
| 边界 | Scope / Out of Scope | 是否知道哪些不做 |
| 拆解 | Tasks | 每个任务能否独立完成 |
| 实现 | Code + Test | 能否运行 |
| 验证 | Test / Review | 是否符合 Spec |
| 收尾 | Diff / Docs | 是否留下可维护的结果 |

---

## 十、轻量落地方式（不用整套工具，先用文件夹结构）

在项目里为每个功能建一个文件夹，强制 AI 按此工作：

```
feature/
├── spec.md     # 需求、验收标准、Out of Scope
├── plan.md     # 技术方案：Backend / Agent / DB / API / Frontend
└── tasks.md    # 可勾选的任务清单
```

让 Coding Agent（Claude Code / Cursor / Qoder 等）**严格按这三个文件干活**，而不是每次重新用一段自然语言描述需求。

---

## 十一、一句话总结

> 你要练的不是"如何写 Prompt 让 AI 写代码"，
> 而是"如何把一个模糊的业务需求，工程化地转换成 AI 可以执行、验证、迭代的开发任务"。

这条链是：**需求理解 → 系统分析 → 方案设计 → 任务拆解 → AI 实现 → 测试 → Review → 迭代收敛。**

（参考：GitHub Spec Kit 的 Constitution → Specify → Plan → Tasks → Implement → Converge 流程，本文档是其思想的轻量化、可直接套用版本。）