# collections

## deque
特性，首尾增删改查速度很快，list对头操作O(n)
```python
from collections import deque
d = deque([1,2,3],maxlen=5)#自动淘汰旧数据

# 头尾增删
d.append(4)        # 右侧加
d.appendleft(0)    # 左侧加
d.pop()            # 右侧删并返回
d.popleft()        # 左侧删并返回

# 其他
d[0] # 取队首，d[-1]取队尾
d[1] # 取第二个元素，但是是链表，速度比list还慢
d.clear()
d.rotate(1) # 向右旋转一位
```

## namedtuple

给元组添加对应的值的注释，相较于字典，更快更节省内存，**能够索引的元组基类**
```python
namedtuple(
    类名字符串,    # 只能一个！
    字段列表        # 只能一个列表！
)
#举例：
from collections import namedtuple

# 一个 namedtuple 包含：x, y, r, g, b
Pixel = namedtuple("Pixel", ["x", "y", "r", "g", "b"])

# 创建一个实例
p = Pixel(100, 200, 255, 0, 0)

# 取值
# 由于是tuple，所以也可以用索引直接获取，比如p.x等效于p[0]
print("x 坐标:", p.x)
print("y 坐标:", p.y)
print("红色:", p.r)
print("绿色:", p.g)
print("蓝色:", p.b)
```

## defaultdict

解决普通字典访问不存在的key报错问题，如果不存在的key，defaultdict会自动创建，并给一个默认值

**2种默认值**：
1. defaultdict(int),默认值为0，可用做计数
2. defaultdict(list),默认值为空列表[],可用做分组

**计数示例**：
```python
from collections import defaultdict

d = defaultdict(int)  # 不存在 → 默认为 0

d["apple"] += 1   # 没有 apple？自动变成 0+1=1
d["apple"] += 1   # 再加 1 → 2

print(d["apple"])  # 2
print(d["banana"]) # 不存在 → 输出 0，不报错！
```

**分组示例**：
```python
d = defaultdict(list)  # 不存在 → 默认为空列表

d["fruit"].append("苹果")
d["fruit"].append("香蕉")
d["animal"].append("小狗")

print(d["fruit"])  # ['苹果', '香蕉']
print(d["animal"]) # ['小狗']
```

## Counter

给你一个列表 / 字符串，自动统计每个元素出现多少次！

1. 统计字符串字符出现多少次
```python
from collections import Counter

s = "aabbccddeeff"
c = Counter(s)

print(c["a"])  # 2
print(c["b"])  # 2
print(c["z"])  # 0（不存在也返回0，但不新增，不报错）
```

2. 统计列表元素

```python
lst = ["苹果", "苹果", "香蕉", "橙子", "苹果"]
c = Counter(lst)

print(c["苹果"]) # 3
```

3. most_common(n) 取出现子树最多的前n个

`print(c.most_common(2))`
