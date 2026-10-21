# LLMToolEmulator

为了测试目的适用大预言模型模拟工具执行，用人工智能生成地回复代替实际地工具调用。大预言模型模拟器可以在以下场景使用：

- 不执行真实地工具地情况下测试agent行为
- 在外部工具不可用+成本高昂时开发agent
- 在实施实际工具之前对agent工作流进行原型设计


## 参数配置

tools:list[string|BaseTool]=None
要模拟的工具名称（字符串）列表或 BaseTool 实例列表。如果None（默认值），则模拟所有工具。如果是空列表[]，则不模拟任何工具。如果是包含工具名称/实例的数组，则仅模拟这些工具。

model:string|baseChatModel

用于生成模拟工具响应的模型。可以是模型标识符字符串（例如 'google_genai:gemini-3.6-flash'）或 BaseChatModel 实例。如果未指定，默认使用智能体的模型。



