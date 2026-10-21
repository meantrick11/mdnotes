# deep agent的model

Deep Agent的model设置可以直接照搬langchain/graph的模型配置

## 支持的模型


以 provider:model 格式指定模型（例如，google_genai:gemini-3.6-flash、openai:gpt-5.4 或 anthropic:claude-sonnet-4-6）。提供程序前缀选择 LangChain 集成，冒号后的所有内容将作为模型标识符传递给该提供程序。有关有效的提供程序字符串，请参阅 init_chat_model 的 model_provider 参数。

模型标识符必须符合提供商预期的格式。部分提供商使用简单名称，如 gpt-5.5；另一些则使用带命名空间的 ID 或部署路径，如 zai-org/GLM-5.2，因此完整的 Deep Agents 字符串应为 baseten:zai-org/GLM-5.2。请查看提供商的模型目录或集成文档获取最新的标识符。

## 推荐模型

推荐模型这里不过多介绍，当然越强越好，不过token越贵

## 模型配置参数

因为模型可以从langchain的模型配置直接复用，那么就直接可以使用langchain.init_chat_model进行模型初始化，追溯其底层依旧是模型提供商的SDK，langchain只是对他做了上层的封装(所以你可以直接使用模型提供商的SDK初始化模型)

详细模型初始化可以查看langchian的教学文档

## Agent运行时选择模型


底层使用的langchain，可以同样使用@wrap_model_call进行中间件包裹模型节点进行选择。

```python
from dataclasses import dataclass
from typing import Callable

from langchain.agents.middleware import ModelRequest, ModelResponse, wrap_model_call
from langchain.chat_models import init_chat_model
from deepagents import create_deep_agent


@dataclass
class Context:
    model: str


@wrap_model_call
def configurable_model(
    request: ModelRequest,
    handler: Callable[[ModelRequest], ModelResponse],
) -> ModelResponse:
    model_name = request.runtime.context.model
    model = init_chat_model(model_name)
    return handler(request.override(model=model))


agent = create_deep_agent(
    model="google_genai:gemini-3.6-flash",
    middleware=[configurable_model],
    context_schema=Context,
)

# Invoke with the user's model selection
result = agent.invoke(
    {"messages": [{"role": "user", "content": "Hello!"}]},
    context=Context(model="openai:gpt-5.5"),
)
```





