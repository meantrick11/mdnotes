# ModelFallBackMiddleware
**隶属@wrap_model_call**
ModelFallBackMiddleware中间件是用来解决如果主模型出现故障时自动切换到备选模型，模型切换适用于以下场景：
- 构建高可用的agent
- 切换到更便宜的模型来优化成本
- 覆盖多个模型提供商等

简单示例：
```python
from langchain.agents import create_agent
from langchain.agents.middleware import ModelFallbackMiddleware

agent = create_agent(
    model="gpt-5.5",
    tools=[],
    middleware=[
        ModelFallbackMiddleware(
            "gpt-5.4-mini",
            "claude-3-5-sonnet-20241022",
        ),
    ],
)
```

## 参数配置

first_model string|BaseChatModel 

**必选参数**，除了create_deep_agent的模型之外的第一个备用模型

*additional_models string|BaseChatModel

其他调用模型，**顺序为依次尝试**


## 示例

如简单示例

