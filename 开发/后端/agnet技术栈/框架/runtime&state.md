# Runtime VS State

agent的tuntime我认为是执行引擎，也就驱动整个agent运行提供的环境，而State则侧重agent在运行过程中的数据维持，在每个节点的runtime都不相同，其中包括state，因为数据也是当前节点的执行环境一部分。


langgraph中的runtime有两个含义：
1. **广义的runtime：执行引擎**：

除了用户定义的节点、边、state之外，只描述了流程怎么走，真正执行时，还需要runtime进行负责。

- 决定这一轮执行哪些节点。
- 把当前 State 交给节点。
- 收集节点返回的更新，按 reducer 规则合并。
- 继续下一轮，直到结束。
- 配合相关配置处理持久化、暂停恢复、重试、流式输出等。

这套执行机制就叫 runtime（运行时）。LangGraph 的底层执行引擎是 Pregel；编译后的图通过它运行。其核心循环是：规划本轮节点、执行节点、应用更新。

**runtime 管理执行过程，State 是执行过程中被读取、更新的数据。**

2. **代码里的runtime：提供运行依赖和能力对象**：

这里的runtime不是整个执行引擎本身，而是框架注入给节点一个能访问引擎的入口，提供如下内容：

| 内容 | 用途 | 举例 |
|---|---|---|
| `runtime.context` | 本次运行的固定上下文、依赖 | 用户 ID、模型客户端、数据库连接 |
| `runtime.store` | 访问长期存储 | 读取用户跨会话的偏好 |
| `runtime.stream_writer` | 发送自定义进度事件 | “正在检索资料” |
| `runtime.previous` | 获取该线程上次返回值，有使用条件 | Functional API 配合 checkpointer |

**State 保存任务状态，Runtime 提供运行资源，节点把需要的信息组装成模型请求，引擎负责调度节点并应用状态更新。**