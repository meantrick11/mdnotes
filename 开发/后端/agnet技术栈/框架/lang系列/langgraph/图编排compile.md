`compile()` 方法的核心作用是将你定义的 `StateGraph` 转换为一个可执行的 `CompiledStateGraph` 对象。它接收的参数主要控制图的**运行时行为**，比如持久化、中断、缓存等。

### 主要参数

| 参数                   | 类型                                        | 说明                                                               |
| :------------------- | :---------------------------------------- | :--------------------------------------------------------------- |
| **checkpointer**     | `BaseCheckpointSaver` \| `None` \| `bool` | 启用状态持久化，是使用 `interrupt_before` / `interrupt_after` 和“时间旅行”功能的前提。 |
| **interrupt_before** | `list[str]`                               | 指定在哪些节点**执行前**暂停。传入 `"*"` 可表示所有节点。                               |
| **interrupt_after**  | `list[str]`                               | 指定在哪些节点**执行后**暂停。传入 `"*"` 可表示所有节点。                               |
| **store**            | `BaseStore` \| `None`                     | 传入一个 `BaseStore` 实例，为图提供跨会话的长期记忆存储能力。                            |
| **cache**            | `BaseCache` \| `None`                     | 配置节点级别的缓存后端，用于缓存节点执行结果，避免重复计算。                                   |
| **debug**            | `bool`                                    | 是否启用调试模式，打印详细的执行日志。默认为 `False`。                                  |
| **name**             | `str`                                     | 为编译后的图指定一个名称，便于在日志和追踪中识别。                                        |

### 其他方式配置的参数

部分参数不在 `compile()` 中直接传入，而是通过其他方式配置：

*   **recursion_limit（递归限制）**：控制图执行的最大步数，防止无限循环。它**不是** `compile()` 的参数，而是在调用 `invoke()` 或 `stream()` 时，通过配置传入的。例如：`graph.invoke(input, config={"recursion_limit": 50})`。

### 使用示例

```python
from langgraph.graph import StateGraph, START, END
from langgraph.checkpoint.memory import InMemorySaver
from langgraph.store.memory import InMemoryStore
from langgraph.cache.memory import InMemoryCache

# 假设你已经定义好了 State、节点和边
builder = StateGraph(State)
# ... 添加节点和边 ...

# 编译图，启用持久化、中断、长期记忆和缓存
compiled_graph = builder.compile(
    checkpointer=InMemorySaver(),          # 短期记忆（线程级）
    store=InMemoryStore(),                 # 长期记忆（跨线程）
    cache=InMemoryCache(),                 # 节点缓存
    interrupt_before=["tools"],            # 在 tools 节点执行前暂停
    interrupt_after=["agent"],             # 在 agent 节点执行后暂停
    debug=True,                            # 开启调试日志
    name="my_agent_workflow"               # 为图命名
)

# 调用时设置递归限制
config = {"configurable": {"thread_id": "1"}, "recursion_limit": 100}
result = compiled_graph.invoke({"messages": [...]}, config)
```

总结一下，`compile()` 的参数主要用于配置图的**运行时能力**（持久化、中断、缓存等），而像递归限制这类执行时参数，则需要在调用时传入。