# SummarizationMiddleware

**隶属于@before_model中间件配置**

当token超过某一个限制时候，上下文窗口不够用时，需要进行上下文压缩，这属于**上下文工程**的一部分，大概率是针对冗余的输出/过早的历史消息进行压缩形成摘要作为SystemMessage作为开头，主要适用如下场景：

- 超出上下文窗口的长时间对话
- 拥有大量历史记录的多轮对话
- 适用于需要完整对话上下文的应用场景

## 参数配置

### model 模型
类型：字符串 | BaseChatModel，**必填**
用于生成摘要的模型。可以传入模型标识符字符串（例如 `'openai:gpt-5.4-mini'`），或是 `BaseChatModel` 的实例对象。更多信息可参考 `init_chat_model`。

### trigger 触发条件
类型：ContextSize | TriggerClause | list[ContextSize | TriggerClause] | None
触发摘要的条件，支持以下形式：
1. 单个 ContextSize 元组：需要满足该指定阈值
2. 单个 TriggerClause 字典：字典内**所有阈值必须同时满足（逻辑与 AND）**
3. 混合上述两种形式组成的列表：**满足列表中任意一项即可（逻辑或 OR）**

支持的阈值类型：
- `fraction` (浮点数)：模型上下文窗口的占比（取值0~1）
- `tokens` (整数)：token绝对数量
- `messages` (整数)：消息条数

> ContextSize 元组仅表达**单个阈值**。
> TriggerClause 字典可包含一个或多个阈值，例如 `{"tokens": 4000, "messages": 10}`，字典内全部阈值必须同时满足（逻辑与）。
> 每个 TriggerClause 字典至少需要定义一个阈值。
> 如果不配置 `trigger`，摘要功能**不会自动触发**。
> 更多信息参考 API 文档里 ContextSize、TriggerClause 的说明。

### keep 保留
类型：ContextSize，默认值：`("messages", 20)`
摘要执行后需要保留多少上下文。**三选一，只能指定其中一项**：
- `fraction` (浮点数)：需要保留的上下文占模型上下文窗口的比例（0~1）
- `tokens` (整数)：需要保留的token绝对数量
- `messages` (整数)：需要保留的最近消息条数


### token_counter
类型：函数
自定义token计数函数。默认采用**基于字符**的计数方式。

### summary_prompt 摘要提示词
类型：字符串
自定义摘要提示模板。不配置时使用内置模板。模板中必须包含占位符 `{messages}`，对话历史会被填充到这个位置。


### trim_tokens_to_summarize 用于摘要的令牌修剪上限
类型：数字，默认值：`4000`
生成摘要时，最多纳入多少token。在执行摘要之前，原始消息会先做截断，保证内容不超过该上限。


### summary_prefix 摘要前缀
类型：字符串，**已弃用**
已废弃：请改用 `summary_prompt` 传入完整提示词。

### max_tokens_before_summary 摘要前最大令牌数
类型：数字，**已弃用**
已废弃：请改用 `trigger: ("tokens", value)`。原本作用是触发摘要的token阈值。

## 示例

```python
from langchain.agents import create_agent
from langchain.agents.middleware import SummarizationMiddleware


# Single condition: trigger if tokens >= 4000
agent = create_agent(
    model="gpt-5.5",
    tools=[your_weather_tool, your_calculator_tool],
    middleware=[
        SummarizationMiddleware(
            model="gpt-5.4-mini",
            trigger=("tokens", 4000),
            keep=("messages", 20),
        ),
    ],
)

# Multiple conditions: trigger if number of tokens >= 3000 OR messages >= 6
agent2 = create_agent(
    model="gpt-5.5",
    tools=[your_weather_tool, your_calculator_tool],
    middleware=[
        SummarizationMiddleware(
            model="gpt-5.4-mini",
            trigger=[
                ("tokens", 3000),
                ("messages", 6),
            ],
            keep=("messages", 20),
        ),
    ],
)

# AND logic: trigger only when tokens >= 4000 AND messages >= 10
agent3 = create_agent(
    model="gpt-5.5",
    tools=[your_weather_tool, your_calculator_tool],
    middleware=[
        SummarizationMiddleware(
            model="gpt-5.4-mini",
            trigger={"tokens": 4000, "messages": 10},
            keep=("messages", 20),
        ),
    ],
)

# Combine AND and OR: trigger if (tokens >= 5000 AND messages >= 3)
# OR (tokens >= 3000 AND messages >= 6)
agent4 = create_agent(
    model="gpt-5.5",
    tools=[your_weather_tool, your_calculator_tool],
    middleware=[
        SummarizationMiddleware(
            model="gpt-5.4-mini",
            trigger=[
                {"tokens": 5000, "messages": 3},
                {"tokens": 3000, "messages": 6},
            ],
            keep=("messages", 20),
        ),
    ],
)

# Using fractional limits
agent5 = create_agent(
    model="gpt-5.5",
    tools=[your_weather_tool, your_calculator_tool],
    middleware=[
        SummarizationMiddleware(
            model="gpt-5.4-mini",
            trigger=("fraction", 0.8),
            keep=("fraction", 0.3),
        ),
    ],
)
```