# langchain.create_agent


`create_agent` 是 LangChain 中用于构建智能体的核心函数，它生成一个由 LangGraph 驱动的状态机，让你能灵活地组合模型、工具和中间件来定制智能体行为。

### 核心参数

创建智能体时，最基础且必需的参数是 `model` 和 `tools`，`system_prompt` 则用于定义其核心指令。

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| **model** | `str` \| `BaseChatModel` | **必需**。指定智能体使用的语言模型。可以是模型标识符字符串（如 `"openai:gpt-5.5"`），也可以是已初始化的模型实例。(通过init_model或者openai SDK等模型实例SDK实例化的模型) |
| **tools** | `Sequence[BaseTool \| Callable \| dict]` \| `None` | **必需**。提供给智能体使用的工具列表。支持 LangChain 工具、任意可调用对象或工具字典。 |
| **system_prompt** | `str` \| `SystemMessage` \| `None` | 可选。设置智能体的系统提示词，用于指导其行为和角色。 |

### 高级配置参数

除了基础参数，`create_agent` 还提供了丰富的选项来控制智能体的状态、行为和集成能力。

| 参数 | 类型 | 说明 |
| :--- | :--- | :--- |
| **middleware** | `Sequence[AgentMiddleware]` | 中间件列表，用于在智能体执行的各个阶段插入自定义逻辑，如人工审核、错误处理、日志记录等。 |
| **response_format** | `ResponseFormat \| type \| None` | 定义智能体返回结构化输出的格式。可传入 Pydantic 模型、数据类或 `ToolStrategy`/`ProviderStrategy` 策略。 |
| **state_schema** | `type[AgentState] \| None` | 自定义智能体的状态结构。当需要追踪和管理额外的状态字段时使用。 |
| **context_schema** | `type \| None` | 定义运行时上下文的结构，用于在工具和中间件中注入依赖（如数据库连接、用户ID）。 |
| **checkpointer** | `Checkpointer \| None` | 用于持久化智能体状态，实现短期记忆（线程级）。传入 `MemorySaver()` 等实例即可启用。 |
| **store** | `BaseStore \| None` | 用于跨会话的长期记忆存储，可在不同线程间共享信息。 |
| **interrupt_before** | `list[str] \| None` | 指定在哪些节点**执行前**暂停。常用于在执行危险工具前请求人工确认。 |
| **interrupt_after** | `list[str] \| None` | 指定在哪些节点**执行后**暂停，便于检查中间状态。 |
| **debug** | `bool` | 是否输出详细的调试日志。 |
| **name** | `str \| None` | 为智能体指定名称，便于在日志和追踪中识别。 |
| **cache** | `BaseCache \| None` | 配置缓存机制，用于缓存模型响应以提高性能。 |

### 参数选型建议

*   **基础场景**：只需配置 `model` 和 `tools`，即可快速创建一个能调用工具的对话智能体。
*   **需要记忆功能**：传入 `checkpointer` 参数（如 `MemorySaver()`）来实现多轮对话的上下文记忆。
*   **需要结构化输出**：使用 `response_format` 参数，让智能体返回格式化的数据（如 JSON）而非自然语言，便于程序解析。
*   **需要人工干预**：结合 `middleware`（如 `HumanInTheLoopMiddleware`）和 `interrupt_before` 参数，在关键步骤前暂停，等待人工确认后再继续执行。
*   **需要注入依赖**：使用 `context_schema` 定义上下文结构，并在工具函数中通过 `ToolRuntime` 访问，实现依赖注入。

你可以根据具体的业务需求，从这些参数中组合出最适合的智能体配置。


## 节点

知道langgraph的超级步和pregel设计，如果不添加其他节点的话，create_agent(...)创建的是一个简单的只有LLM+TOOL的React循环agent模式，只有两个节点