# Python3.x Python lambda（匿名函数）
Python 使用 lambda 来创建匿名函数。
lambda 函数是一种小型、匿名的、内联函数，它可以具有任意数量的参数，但只能有一个表达式。
匿名函数不需要使用 def 关键字定义完整函数。
lambda 函数通常用于编写简单的、单行的函数，通常在需要函数作为参数传递的情况下使用，例如在 map()、filter()、reduce() 等函数中。

## lambda 函数特点
1. lambda 函数是匿名的，它们没有函数名称，只能通过赋值给变量或作为参数传递给其他函数来使用。
2. lambda 函数通常只包含一行代码，这使得它们适用于编写简单的函数。

## lambda 语法格式
```python
lambda arguments: expression
```
- `lambda`：Python 关键字，用于定义 lambda 函数。
- `arguments`：参数列表，可以包含零个或多个参数，逗号分隔，写在冒号 `:` 前。
- `expression`：唯一表达式，表达式计算结果自动作为返回值。

## 基础示例
### 无参数 lambda
```python
f = lambda: "Hello, world!"
print(f())  # 输出: Hello, world!
```

### 单个参数
```python
x = lambda a : a + 10
print(x(5))  # 输出: 15
```

### 两个参数（相乘）
```python
x = lambda a, b : a * b
print(x(5, 6))  # 输出: 30
```

### 三个参数（相加）
```python
x = lambda a, b, c : a + b + c
print(x(5, 6, 2))  # 输出: 13
```

## lambda 配合高阶内置函数使用
### 1. map()：批量转换元素
```python
numbers = [1, 2, 3, 4, 5]
squared = list(map(lambda x: x**2, numbers))
print(squared)  # 输出: [1, 4, 9, 16, 25]
```

### 2. filter()：过滤符合条件元素
```python
numbers = [1, 2, 3, 4, 5, 6, 7, 8]
even_numbers = list(filter(lambda x: x % 2 == 0, numbers))
print(even_numbers)  # 输出：[2, 4, 6, 8]
```

### 3. reduce()：累积运算（需导入 functools）
```python
from functools import reduce
numbers = [1, 2, 3, 4, 5]
# 计算序列累积乘积
product = reduce(lambda x, y: x * y, numbers)
print(product)  # 输出：120
```
执行逻辑：依次计算 `1*2 → 2*3 → 6*4 → 24*5`，最终得到 120。