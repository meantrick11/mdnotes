# LLMToolSelectorMiddleware

使用**大语言模型在主模型前筛选相关工具**，主要适用场景如下：

- 适用于拥有大量工具的Agent，尤其是每个查询中的大多数工具都与当前无关的工具

- 通过过滤不相关的工具来减少token的使用

- 提高主模型的专注度和准确性

## 参数配置

model:string|BaseChatModel

工具选择的模型。可以是模型标识符字符串或BaseChatModel实例。**默认为agent的主模型**

system_prompt:string

选择模型的指令。如果没有指定，则使用内置提示词。

max_tools:number
可选择的工具最大数量。如果模型选择的数量更多，将仅使用前max_tools个。**未指定则无限数量**

always_include:list[string]
始终包含的工具名称。**这些不计入max_tools限制

