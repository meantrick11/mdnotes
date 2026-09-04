# Context 工作流

flowchart TD
  start[用户在同一会话发送本句] --> newTurn[开启当前 Turn 分配 turn_id]
  newTurn --> saveUser[本句 user 写入 Persistent History]
  saveUser --> hasSum{已有 running_summary?}
  hasSum -->|有| withSum[装入旧摘要]
  hasSum -->|无| noSum[摘要为空]
  withSum --> watermark{watermark 是否为空?}
  noSum --> watermark
  watermark -->|空 从未压过| fullHist[全部已完成 Persistent History]
  watermark -->|已推进| tailHist[仅 watermark 之后的未压缩 Persistent History]
  fullHist --> histKind["Persistent: user + 最终assistant + tool stub 无全文 Tool Result"]
  tailHist --> histKind
  histKind --> skipCur["当前未完成 Turn 的 stub 不作为可压缩旧历史"]
  skipCur --> hasDraft{有待审 draft?}
  hasDraft -->|是| packD[加 draft 一行]
  hasDraft -->|否| packN[不加 draft]
  packD --> packExt
  packN --> packExt["外部 Context: sys + 工具定义 + 当前user + running_summary? + draft? + 未压缩 Persistent History + 当前 Runtime 若已有内容"]
  packExt --> ext80{外部 Context 约占窗口 80%?}
  ext80 -->|否| llm[调用 LLM]
  ext80 -->|是| compactExt[压缩 Persistent History]
  compactExt --> fromTail[从历史末端向前累计 token]
  fromTail --> keepTurns["尽量保留约 K tokens 的最近已完成 Turn"]
  keepTurns --> turnBound{若切开会拆开某个 Turn?}
  turnBound -->|是| wholeTurn[扩大到该 Turn 完整边界 宁可略超 K]
  turnBound -->|否| wholeTurn
  wholeTurn --> neverCur[禁止把当前未完成 Turn 纳入被摘要段]
  neverCur --> cSum[只摘要更早的完整 Turn 保住任务目标]
  cSum --> cJoin[新 running_summary = 旧摘要拼接本段]
  cJoin --> cMark[watermark 推进到被摘要的最后完整 Turn 之后]
  cMark --> packExt2["外部压后: sys + 工具定义 + 当前user + 新摘要 + draft? + 近端完整 Turn 的 Persistent History"]
  packExt2 --> llm

  llm --> out{模型输出?}
  out -->|最终 assistant 正文| saveA[最终 assistant 入库 当前 Turn 完成]
  saveA --> ui[前端只显示 user 与最终 assistant]
  saveA --> dropRt[丢弃本 Turn Runtime 全文 Tool Result]
  dropRt --> again[下一句或重启]
  again --> start

  out -->|工具调用 含 read_file| exec[执行工具 得到全文 Tool Result]
  exec --> stub["Persistent: 立刻写 tool stub 名+参数+前150字+status"]
  stub --> runtime["Runtime: 追加 assistant tool_call + 完整 Tool Result 到当前 Turn"]
  runtime --> packInt["内部 Context: sys + 工具定义 + 当前user + running_summary? + draft? + 未压缩 Persistent History 不含与全文重复的当前 stub + 当前 Turn trajectory + 当前完整 Tool Result"]
  packInt --> int80{内部 Context 约占窗口 80%?}
  int80 -->|否| llm
  int80 -->|是| protect["高优先级保护: 当前 Turn / 当前 Tool Chain / 当前完整 Tool Result"]
  protect --> compactInt[只压缩更早 Persistent History 与已完成旧 Turn]
  compactInt --> fromTail2[从旧历史末端向前累计 token]
  fromTail2 --> keepTurns2["尽量保留约 K tokens 的最近已完成 Turn"]
  keepTurns2 --> turnBound2{会拆开 Turn?}
  turnBound2 -->|是| wholeTurn2[在完整 Turn 边界切开]
  turnBound2 -->|否| wholeTurn2
  wholeTurn2 --> neverCur2[watermark 不指向当前未完成 Turn]
  neverCur2 --> cSum2[旧完整 Turn 写入 running_summary]
  cSum2 --> cMark2[推进 watermark]
  cMark2 --> packInt2["内部压后: sys + 工具定义 + 当前user + 新摘要 + draft? + 近端完整旧 Turn 的 Persistent + 当前 Runtime 全文 Tool Result 保持完整"]
  packInt2 --> llm

## 数据库存储内容：

1. 用户的发送消息
2. Agent最终输出
3. 每次Agent内部的推理（不存储），且不作为历史消息
4. 工具调用，数据库存储每次工具调用的名称、参数、结果信息截断

以用户发送的消息最为turn进行管理对话

### 推理内容不持久化，Tool Result 默认不全量持久化

考虑到数据库存储成本，如果将 Agent 每轮的完整 Tool Result 全部持久化，
随着会话增长，数据库空间会持续增大。

因此当前 NoteAgent 采用轻量化方案：

- Agent 内部推理过程不持久化
- Tool Call 的元数据持久化
- Tool Result 默认只保存截断内容（如前 150 字）
- 完整 Tool Result 仅存在当前 Agent Run 的 Runtime Context 中
- 当前 Turn 完成后释放完整 Tool Result
- 后续如果需要之前的 Tool Result，由 Agent 根据历史 Tool Call 信息重新调用工具获取

这种方式牺牲了一定的历史执行现场恢复能力，但实现简单，
对于当前 NoteAgent 的项目规模已经足够。

Claude Code / Codex 等成熟 Coding Agent 则会持久化更完整的 Session / Tool
执行记录，以支持 Session Resume、Replay、Debug 等能力。
如果后续需要增强 NoteAgent 的历史恢复能力，可以再考虑 Session 文件、
Tool Result 独立存储等方案。

## 触发压缩时的K如何确定？

### K 的定义

> **K = 压缩后，从当前往前希望保留的“历史 Turn”所占的 Token 预算。**
### K 怎么算

先定义：

```text
W = 模型 Context Window
T = 压缩后的目标水位
F = 压缩后必须保留的非历史内容
```

那么：

```text
K = T - F
```

其中：

```text
T = W × 目标比例
```

例如你采用：

```text
触发水位 = 80%
目标水位 = 60%
```

32K 模型：

```text
W = 32K

触发：
32K × 80% = 25.6K

目标：
32K × 60% = 19.2K
```

假设压缩后必须保留：

```text
System            2K
Tools             2K
Summary           2K
当前 User         1K
Runtime Context   3K
Output Reserve    3K
Safety Buffer     1K
────────────────────
F                 14K
```

那么：

```text
K = 19.2K - 14K
  = 5.2K
```

**所以本次 K = 5.2K。**
### 然后怎么用 K？

从当前往历史方向：

```text
Turn 10 = 1.5K
Turn 9  = 1.8K
Turn 8  = 1.2K
Turn 7  = 2.0K
```

累积：

```text
Turn 10 + Turn 9 + Turn 8
= 4.5K
```

加入 Turn 7：

```text
4.5K + 2.0K = 6.5K > K
```

所以：

```text
保留 Turn 8～10
≈ 4.5K

Turn 1～7
→ 压缩
```

**不为了凑满 5.2K 而拆 Turn。**

---

### 最终记住这一条公式

```text
K
=
（Context Window × 压缩目标比例）
-
（System + Tools + Summary + 当前User
 + Runtime Context + Output Reserve + Buffer）
```

