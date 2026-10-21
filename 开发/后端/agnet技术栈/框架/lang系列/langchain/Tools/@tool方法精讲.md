# 1. 什么能被 @tool 装饰？（所有可调用对象）
在 Python 里，满足 **`obj()` 能直接调用** 的，都能用 `@tool`：
1. **普通函数**（最常用）
2. **类的实例方法**
3. **实现了 `__call__` 方法的类实例**（类变成可调用）
4. **lambda 匿名函数**
5. **functools.partial 包装的函数**

---

## 2. 最终被转换成了什么？
不管你传入什么，`@tool` 装饰后，**统一变成：`StructuredTool` 对象**
- 它是 Langchain 标准工具类
- 拥有固定属性：`name`、`description`、`args_schema`、`invoke()`
- Agent 只能识别这种标准化工具

**底层逻辑：**
```
你的函数/可调用对象
   ↓ @tool 包装
StructuredTool（Langchain 标准工具）
```

---

## 3. 代码示例：3 种不同东西，都能 @tool
### ① 装饰普通函数（最常见）
```python
from langchain.tools import tool

@tool
def add(a: int, b: int) -> int:
    """加法计算"""
    return a + b

print(type(add))  # <class 'langchain.tools.base.StructuredTool'>
```

### ② 装饰实现 __call__ 的类（可调用类）
```python
@tool
class Calculator:
    def __call__(self, a: int, b: int):
        """计算两个数之和"""
        return a + b

calc = Calculator()
print(type(calc))  # <class 'langchain.tools.base.StructuredTool'>
```

### ③ 装饰 lambda
```python
mul = tool(lambda a, b: a * b)
print(type(mul))  # <class 'langchain.tools.base.StructuredTool'>
```

---

## 4. 核心结论（一句话）
**@tool 是一个标准化包装器**：
- 输入：**任意可调用对象**（函数、类实例、lambda…）
- 输出：**统一的 StructuredTool 实例**
- 目的：让 Agent 能统一调用所有工具
