# Typing 

类型注释模块


## List

b = list[str] #包含全是str的list

## Tuple

item = tuple[int,int,str] #声明一个有三种类型的不可变元组

## Dict

def process_items(prices:dtct[str:int]):
    pass

指定dict的键为str，值为int

## Literal 

### 基本概念

Literal 是Python typing模块中的一种类型注释，用于指定变量或者函数参数只能读取特定的字面值，只允许变量取值限制在指定的一组值中，确保程序值接收特定的常量值，以此减少错误的发生。

在 Python 中，通常我们会使用常见的类型注解来限制变量的类 型，例如 int, str, bool 等。然而，有时我们想要进一步限制一个变量只能取某些特定的值，而不仅仅是某个类型。例如，假设我们有一个变量 color，并且我们希望它只能是 “red”、“green” 或 “blue”，这时我们就可以使用 Literal 来实现这种约束。

### 基本用法

```python
from typing import Literal

def set_color(color:Literal["red","yellow"])->None:
    print(color)
```
通常我们函数类型的声明通过数据类型声明就可以了，比如color:str,但是不够精确，所以可以用literal类，创建一个限定的字面量数据传入

### Literal 的优点

1. 提高代码的可读性和安全性
2. 防止错误的发生
3. 自动补全和类型提示，一些IDE提供Literal的自动补全功能

### 总结

Literal 限定一个变量只能取特定的值（字面量），而不仅仅是类型。
通过 Literal，可以在类型注解中指定函数的参数或类的属性只能接受一组有限的常量值。
它提高了代码的安全性，减少了潜在的运行时错误，尤其适用于函数参数、配置项、状态管理等场景。

## TypeDict'


### 基础概念
`TypedDict` 是 Python `typing` 模块提供的工具，用于**为字典定义固定的键名和对应值类型**，让字典具备类型检查能力，解决普通字典无类型约束、易出错的问题。

- 适用场景：需要明确结构的字典（如配置项、接口参数、数据模型）
- 核心作用：静态类型检查（IDE 提示、代码校验工具），运行时不强制校验类型
- 支持版本：Python 3.8+ 内置，低版本需安装 `typing-extensions`

### 基础用法
#### 定义固定结构的字典
```python
from typing import TypedDict

# 定义 TypedDict 类型
class User(TypedDict):
    name: str  # 键 name，值必须是字符串
    age: int   # 键 age，值必须是整数
    email: str | None  # 键 email，值可为字符串或 None

# 使用：创建符合结构的字典
user: User = {
    "name": "张三",
    "age": 20,
    "email": "zhangsan@example.com"
}
```

#### 关键字参数创建
支持直接通过关键字参数实例化，语法更简洁：
```python
user = User(name="李四", age=25, email=None)
```

### 高级用法
#### 可选键（非必填字段）
使用 `total=False` 声明**非必填键**，默认所有键都是必填的：
```python
from typing import TypedDict

class Product(TypedDict, total=False):
    id: int  # 必填键
    name: str  # 必填键
    price: float  # 可选键（非必填）
    desc: str  # 可选键（非必填）

# 合法：只传必填键
product: Product = {"id": 1, "name": "笔记本"}
# 合法：传全部键
product_full: Product = {"id": 2, "name": "手机", "price": 3999.9}
```

#### 继承扩展
通过类继承，复用已有 `TypedDict` 结构，减少重复代码：
```python
from typing import TypedDict

# 基础类型
class BaseUser(TypedDict):
    id: int
    name: str

# 继承扩展
class Student(BaseUser):
    grade: str
    score: float

# 最终结构：包含 id、name、grade、score
student: Student = {"id": 1, "name": "小明", "grade": "三年级", "score": 95.5}
```

#### 嵌套 TypedDict
支持字典嵌套，定义复杂层级的数据结构：
```python
from typing import TypedDict

class Address(TypedDict):
    province: str
    city: str

class Person(TypedDict):
    name: str
    address: Address  # 嵌套 TypedDict

# 嵌套使用
person: Person = {
    "name": "小红",
    "address": {"province": "广东", "city": "深圳"}
}
```

### 注意事项
1. **运行时无强制校验**：仅用于 IDE 提示和静态检查，运行时传入错误类型不会报错
2. **键名必须是字符串**：`TypedDict` 仅支持字符串类型的键
3. **不可变结构**：定义后不能动态添加/删除键（遵循定义的结构）
4. **兼容普通字典**：`TypedDict` 本质是字典，可使用所有字典方法（`get()`/`items()` 等）

### 简化写法（函数式）
无需定义类，快速创建简单 `TypedDict`：
```python
from typing import TypedDict

# 语法：TypedDict(名称, {键: 类型, ...})
Book = TypedDict("Book", {"title": str, "author": str, "page": int})

book: Book = {"title": "Python入门", "author": "王五", "page": 200}
```

### 总结
1. `TypedDict` 用于给字典**固定键+约束值类型**，提升代码可读性和可维护性
2. 基础用法：类继承 `TypedDict`，声明键和对应类型
3. 高级能力：支持可选键、继承、嵌套
4. 核心价值：静态类型检查，提前发现代码错误