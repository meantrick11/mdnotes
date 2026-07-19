# Context Engineering

1. 压缩(context过长之后的策略)
    - 后端写代码逻辑压缩
    - LLM自主压缩(需要LLM的参数支持，大多数都不喜压缩。)
    - 使用Sub-agent减少主agent的上下文窗口
2. 过滤(在context产生的时候)
    - 按需加载
    - Agentic Context Engineering
*大多数的token全都用在了读无关内容(网页、log)等等操作中*

比如openclaw使用memory_search()&memory_get()函数获取记忆中的一部分，而不是全盘塞给LLM



## 语言模型

每次都需要添加上下文，然后传入给LLM，但是随着上下文长度太长，那么会超过上下文窗口，如果太短，则损失信息，LLM不知道刚才的其他对话信息(前文)


## Context Engineering

### 未做Context Engineering
```plaintext
$I_1 \leftarrow \text{initial input}$
$C_1 \leftarrow \text{empty}$
For $t = 1$ to $\infty$
　$O_t = LLM(I_t, C_t)$
　$C_{t+1} \leftarrow C_t \mid I_t \mid O_t$
```

每次对话都添加到新一轮消息中，包括Question&Answer,每次直接打包发给LLM，这种情况下就会导致LLM上下文窗口暴涨

### 做Context Engineering
```plaintext
$I_1 \leftarrow \text{initial input}$
$C_1 \leftarrow \text{empty}$
For $t = 1$ to $\infty$
　$O_t = LLM(I_t, C_t)$
　$C_{t+1} \leftarrow C_t \mid I_t \mid O_t$
```

每次对话结束后对于前文(Question&Answer)做一些变换,让在有限的窗口添加更多有用的信息(基础信息，当前上下文等)，以便LLM下一轮的回复

**方法：**

- 压缩上下文(如何压缩才不能失去关键信息？"这里有一个工具执行，放在log1.txt，重拾记忆")

## Memory

长期记忆：可以跨对话的记忆信息

短期记忆：仅用于当前对话的记忆信息

**上述的记忆都可以放到硬盘，通过是否跨对话，而不是是否存放**

**Context表示所有的上下文，而prompt表示最终被输入到LLM的提示词，有可能是上下文的压缩问题**

## Summarization for Agent

尽可能在有限的窗口来输入prompt，不丢失关键信息，然后得到更好的答案

有可能原本能解决的问题但压缩之后不能解决了。

reforcelearning的方法取训练ai的摘要能力，如何压缩？

## 什么时候压缩？

模型不喜欢压缩工具，所以Agent都是达到某一个程度然后代码硬执行压缩等操作(大多数时候如果使用prompt提示使用压缩工具，LLM都会效果较差)或者在训练的过程中微调参数使得LLM适应使用压缩工具。

## SubAgent可以视为自主压缩

但是分裂SubAgent需要模型在训练阶段就有分裂agent的能力



