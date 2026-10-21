`SummarizationMiddleware` 的具体流程围绕几个关键常量展开，下面逐一拆解。

### 🎯 触发：什么时候启动摘要？

触发条件由 **`trigger`** 参数控制，它**不会**在 token 数一超标就立刻触发，而是**在下一次调用 LLM 之前的 `before_model` 钩子中检查并执行**。

触发条件支持多种形式：

| 形式 | 示例 | 语义 |
|---|---|---|
| 单个元组 | `("tokens", 4000)` | token 数 ≥ 4000 时触发 |
| 单个字典 | `{"tokens": 4000, "messages": 10}` | token ≥ 4000 **且** 消息数 ≥ 10（AND） |
| 列表 | `[("tokens", 4000), ("messages", 100)]` | 满足任一条件即触发（OR） |

关于 token 计数，默认使用 **`count_tokens_approximately`**，这是一种**基于字符的近似估算**，对于图片仅按每张固定 **85 token** 计算。这意味着它**不包含** system prompt、工具定义等固定开销，实际 provider 端的 prompt token 可能远高于中间件估算值。如果需要精确控制，可以传入自定义的 `token_counter` 函数。

### 📦 保留：压缩后保留哪一部分？

保留窗口由 **`keep`** 参数控制，默认值是 **`("messages", 20)`**，即保留最近 **20 条消息**。`keep` 同样支持 `("tokens", N)` 或 `("fraction", 0.8)` 等形式。

### ✂️ 切割：如何确定要摘要的“旧消息”前缀？

中间件会计算一个**切割索引（cutoff index）**，该索引之前的消息将被摘要，之后的将被保留。在确定切割点时，实现会做一次**结构性调整**：

如果切割点恰好落在一条 **`ToolMessage`** 上，中间件会**向前搜索最多 5 条消息**，找到对应的 **`AIMessage`**（包含 `tool_calls` 的那条），并将这条 `AIMessage` 也纳入**保留范围**，而不是摘要范围。这样做的目的是**避免 AI 的工具调用与其返回结果被拆散**，防止模型收到孤立的工具结果而失去上下文。这个搜索范围由常量 **`_SEARCH_RANGE_FOR_TOOL_PAIRS = 5`** 控制。

### 📝 摘要：具体怎么生成？

一旦确定了“要摘要的消息前缀”，中间件会**独立调用一次摘要模型**（即你传入的 `model` 参数，通常用轻量模型如 `gpt-4o-mini`）来生成摘要。

**使用的提示词**是 **`DEFAULT_SUMMARY_PROMPT`**，其核心指令是让模型从对话历史中**提取最高质量、最相关的上下文**，并按照固定的结构输出：

1. **SESSION INTENT** — 用户的主要目标或请求是什么
2. **SUMMARY** — 重要的选择、结论、策略，以及决策背后的理由和被否决的选项
3. **ARTIFACTS** — 创建、修改或访问的文件和资源
4. **NEXT STEPS** — 为达成目标还需要完成的具体任务

**生成摘要前还有一道“瘦身”步骤**：中间件会先对要摘要的消息进行裁剪，限制其 token 数不超过 **`_DEFAULT_TRIM_TOKEN_LIMIT = 4000`**。这是为了避免摘要调用本身消耗过多 token，因为“要摘要的文本”本身也可能很长。

此外，摘要调用默认带有**重试机制**（通过 `Runnable.with_retry`），最多重试 **3 次**以应对限流或超时；如果仍然失败，错误会向上传播，而不会用错误文本冒充摘要。

### 🔄 状态更新：如何替换旧消息？

摘要生成后，中间件需要**从图状态中彻底移除被摘要的旧消息**，并插入摘要。在 LangChain v1 中，这是通过返回一个包含 **`RemoveMessage(id=REMOVE_ALL_MESSAGES)`** 指令的更新来实现的。

具体来说，`before_model` 返回的更新包含：

1. **`RemoveMessage(id=REMOVE_ALL_MESSAGES)`** — 一个**批量清除操作**，从检查点状态中移除**所有**现有消息
2. **摘要消息** — 通常是一条 `HumanMessage`，内容为 `"## Previous conversation summary:"` 加上生成的摘要文本
3. **保留的近期消息** — 之前计算出的、位于切割点之后的消息原文

这三部分一起写回 `state["messages"]`，通过 `add_messages` reducer 完成持久化替换。

### ⚙️ 关键默认值速查

| 常量 | 默认值 | 含义 |
|---|---|---|
| `_DEFAULT_MESSAGES_TO_KEEP` | 20 | 默认保留最近 20 条消息 |
| `_DEFAULT_TRIM_TOKEN_LIMIT` | 4000 | 生成摘要前，待摘要文本的 token 上限 |
| `_SEARCH_RANGE_FOR_TOOL_PAIRS` | 5 | 向前搜索 AI/Tool 配对的最大消息数 |
| `_DEFAULT_FALLBACK_MESSAGE_COUNT` | 15 | 备用消息计数阈值 |
| `SUMMARY_PREFIX` | `"## Previous conversation summary:"` | 摘要消息的前缀 |

总的来说，这个中间件的设计逻辑是：**用近似计数做快速决策，用结构感知的切割保护工具调用配对，用独立的轻量模型生成结构化摘要，最后用 RemoveMessage 做原子替换**。


```python
SummarizationMiddleware(
    model="openai:gpt-4o-mini",
    trigger={"tokens": 4000},
    keep={"messages": 20},
    token_counter=...,
    summary_prompt="...",
)
```