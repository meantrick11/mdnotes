# PIIMiddleware

通过配置策略检测并处理对话中的个人可识别信息(PII),适用的场景：

- 有合规要求的医疗/金融应用
- 需要清理日志的客户服务agnet
- 任何处理敏感用户数据的应用


## 参数配置

pii_type:string 

**必填**，要检测的个人身份信息类型，可以是内置类型或者自定义类型名称

strategy:string = "redact"

如何处理和检测到的个人识别信息。可选方式：
- "block" 检测到时引发异常
- "redact" 替换为[REDACTED_{PII_TYPE}]
- "mask" 部分掩码（例如，****-****-****-1234）
- "hash" 替换为确定性的hash值


detector:function|regex|re

自定义检测器幻术或者正则表达式，如果未提供则适用针对个人身份类型的内置检测器

>手机号
>邮箱
>身份证号
>银行卡号
>地址
>姓名
>护照 / 驾照等证件号


apply_to_input:boolean = True

在模型调用前检测用户信息

apply_to_output:boolean = False
模型调用后检测AI消息

apply_to_tool_results:boolean = False

执行后检查工具结果消息

## 示例


```python
import re

from langchain.agents import create_agent
from langchain.agents.middleware import PIIMatch, PIIMiddleware


# Method 1: Regex pattern string
agent1 = create_agent(
    model="gpt-5.5",
    tools=[],
    middleware=[
        PIIMiddleware(
            "api_key",
            detector=r"sk-[a-zA-Z0-9]{32}",
            strategy="block",
        ),
    ],
)

# Method 2: Compiled regex pattern
agent2 = create_agent(
    model="gpt-5.5",
    tools=[],
    middleware=[
        PIIMiddleware(
            "phone_number",
            detector=re.compile(r"\+?\d{1,3}[\s.-]?\d{3,4}[\s.-]?\d{4}"),
            strategy="mask",
        ),
    ],
)

# Method 3: Custom detector function
def detect_ssn(content: str) -> list[PIIMatch]:
    """Detect SSNs with validation."""
    matches: list[PIIMatch] = []
    pattern = r"\d{3}-\d{2}-\d{4}"
    for match in re.finditer(pattern, content):
        ssn = match.group(0)
        # Validate: first 3 digits shouldn't be 000, 666, or 900-999
        first_three = int(ssn[:3])
        if first_three not in [0, 666] and not (900 <= first_three <= 999):
            matches.append({
                "type": "ssn",
                "value": ssn,
                "start": match.start(),
                "end": match.end(),
            })
    return matches

agent3 = create_agent(
    model="gpt-5.5",
    tools=[],
    middleware=[
        PIIMiddleware(
            "ssn",
            detector=detect_ssn,
            strategy="hash",
        ),
    ],
)
```