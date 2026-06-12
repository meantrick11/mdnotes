# 热门AI词汇

> **相关笔记**: [[MLnote|ML笔记]] | [[agnet技术栈/bilicourse/agent应用开发指南|Agent开发]]

除了 **Skill**，在当前的 AI（特别是大模型应用）领域，确实还有很多热门的词汇。结合你刚了解的 `RedTeam-Agent` 项目，这几个是最相关且有用的：

---

### 🤖 Agent（智能体）
这正是 `RedTeam-Agent` 的核心概念。

-   **是什么**：'An Agent is a system that leverages an AI model to interact with its environment in order to achieve a user-defined objective. It combines reasoning, planning, and the execution of actions (often via external tools) to fulfill tasks.'


### 🔌 MCP (Model Context Protocol，模型上下文协议)
这个在项目介绍中直接提到过。

-   **是什么**：由 Anthropic 推出的一种标准协议，旨在统一 AI 模型与外部工具、数据源的连接方式。你可以把它想象成 **AI 界的“USB-C 接口”**。任何支持 MCP 的工具，都可以被任何支持 MCP 的 AI 客户端直接调用。
-   **结合项目**：`RedTeam-Agent` 可以作为 **MCP 服务器** 运行，这样你无需手动整合，Claude Desktop 等 AI 就能通过标准协议直接发现并调用它的所有红队工具和技能。

### 🛠️ Function Calling / Tool Use（函数调用/工具使用）
这是让 AI 能够“动手”的基础能力。

-   **是什么**：让大模型不仅能生成文本，还能在必要时生成一个结构化的指令（如一个 JSON），去调用一个预先定义好的外部函数或 API（例如“查询天气”、“执行扫描”）。
-   **结合项目**：Skill 文件里定义的 `fscan`、`nxc` 等工具，正是通过这个机制被 AI 调用的。AI 判断需要扫描网络时，就会生成一个调用 `fscan` 的指令，终端执行后，结果再传回给 AI 分析。

### 📚 RAG (Retrieval-Augmented Generation，检索增强生成)
是目前解决 AI “胡说八道”和无法访问私有知识的核心技术。

-   **是什么**：在 AI 回答问题前，先从外部知识库（如文档、数据库）中检索最相关的信息，把检索到的内容“塞”给 AI 作为参考，再让它生成回答。这就像 **开卷考试和闭卷考试的区别**。
-   **结合项目**：严格来说，`RedTeam-Agent` 的 Skill 文件直接注入上下文，是一种更直接的“上下文注入”，而 RAG 通常指动态检索。但广义上，给 AI 配备一个可查询的漏洞知识库（如 CVE 详情）就是典型的 RAG 应用。

### ✍️ Prompt Engineering（提示词工程）
这是所有 AI 应用的基础手艺。

-   **是什么**：设计和优化输入给 AI 的提示词（Prompt），以引导它产生特定、高质量的输出。好的提示词能极大提升 AI 表现的稳定性。
-   **结合项目**：`.github/copilot-instructions.md` 文件就是一种高级的 **Prompt Engineering**，它为 AI 预设了角色、行为规则和注意事项，是整个 Skill 能正常运作的前提。

---

### 💎 总结一下这些词汇的关系

你可以这样理解它们在一个 AI Agent 项目中的角色：

-   **Agent（智能体）** 是总称，是最终的表现形态。
-   **Skill（技能）** 是给 Agent 装上的“职业培训手册”。
-   **Function Calling（函数调用）** 是 Agent 的“手”，让它能操作具体工具。
-   **MCP（模型上下文协议）** 是 Agent 和工具之间的“通用接口”，让连接更标准化。
-   **RAG（检索增强生成）** 可以成为 Agent 的“外部大脑”，提供实时或专业知识。
-   **Prompt Engineering（提示词工程）** 则是一切能顺利进行的“地基”，决定了 Agent 的智商和情商。