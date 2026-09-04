# 适配Python的基础类型

许多 Python 标准数据类型在执行 SQL 查询时，会自动转换为 SQL 对应类型；查询返回结果又会自动转回 Python 对象。

## Booleans 转换

```python
cur.execute("SELECT %s, %s", (True, False))
# equivalent to "SELECT true, false"
```
**这里的%s是SQL语句占位符的意思，不要当作Python的字符串格式化，无论后谜案传入什么类型的数值，前面占位符都用%s，psycopg底层会自动根据传入转换为对应的SQL类型，所以%s附近也不要有'单引号的存在**

boolean <-> boolean

## Numbers 转换

python当中的

int <-> smallint,integer,bigint,numeric  自适应转换
float <-> float8
decimal <-> numeric

## Str 转换

str <-> str

str <- text,varchar

## Binary 转换

bytes(,bytearray,memoryview)  <-> bytes

## Date/time 转换

date <-> data
datetime <-> timestamp
time <-> time,timetz
timedelta <-> interval

- Python datetime 上限：9999‑12‑31，PostgreSQL 时间可以存超过 9999 年，读回 Python 直接报错。
- PG 有两个魔法时间常量：infinity、-infinity，用来代表无限晚、无限早；Python 标准库没有对应的时间对象，fetch 会炸。
- PostgreSQL 允许合法时间24:00:00，代表当天结束（等价次日 00:00:00）；Python 的 time 对象不接受 24 点，读到就抛异常。

## JSON 转换

dict(json) <-> Json

```python
from psycopg.types.json import Jsonb

thing = {"foo": ["bar", 42]}
conn.execute("INSERT INTO mytable VALUES (%s)", [Jsonb(thing)])
```

## List 转换

List <-> Array

Python 的 list 会自动转换成 PostgreSQL array 数组类型，双向自动转换。限制：列表里面元素必须是同一种类型，允许里面混有None。

**千万不要把 Python list 直接丢给 IN %s，这个写法会报错！list、tuple 都不能直接用于 IN**

```python
conn.execute("SELECT * FROM mytable WHERE id IN %s", [[10,20,30]])
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
psycopg.errors.SyntaxError: syntax error at or near "$1"
LINE 1: SELECT * FROM mytable WHERE id IN $1
```

应该使用ANY代替IN：

`conn.execute("SELECT * FROM mytable WHERE id = ANY(%s)", [[10,20,30]])`


## UUID 转换

uuid <-> UUID


```python
conn.execute("select gen_random_uuid()").fetchone()[0]
UUID('97f0dd62-3bd2-459e-89b8-a5e36ea3c16c')

from uuid import uuid4
conn.execute("select gen_random_uuid() = %s", [uuid4()]).fetchone()[0]
False  # long shot
```

## Network data 转换

IPv4Addreass,IPv4Interface -> inet
IPv4Address <- inet(单个ip地址)
IPv4Interface <- inet(带子网掩码的IP)

IPv4Network <- cidr

IPv6系列和IPv4一样


## 枚举类型适配（Enum）
psycopg 3.1 版本新增

Psycopg 可以把 Python 的 **Enum 枚举类** 自动转换成 PostgreSQL 的枚举类型（用 `CREATE TYPE ... AS ENUM` 创建的那种）。

要实现**双向自动转换**，你需要：
1. 用 `EnumInfo` 获取 PG 枚举信息
2. 用 `register_enum()` 注册映射

注册和不注册，行为不一样。



### 如果枚举**没有注册**
- 普通 Enum → 当成**字符串**存入数据库
- 从数据库读回来 → 得到**字符串**，不是 Python 枚举对象
- 混合类型枚举（如 int + Enum）→ 按 int 处理



### 如果枚举**已经注册**
- Python 枚举 ↔ PostgreSQL 枚举 **自动互相转换**
- 存进去：枚举 → PG 枚举标签
- 读出来：PG 枚举标签 → Python 枚举成员

---

# 类与函数翻译
## `EnumInfo` 类
管理 PostgreSQL 枚举类型的信息
- `name`：枚举名称
- `oid`：类型ID
- `array_oid`：数组类型ID
- `labels`：PG 里定义的枚举值列表
- `enum`：注册后，这里会存对应的 Python 枚举类

## `register_enum()` 函数
注册枚举适配器，实现 Python ↔ PG 枚举自动转换

参数：
- `info`：枚举信息（来自 `EnumInfo.fetch`）
- `context`：注册到哪个连接/全局
- `enum`：要绑定的 Python 枚举类
- `mapping`：自定义 Python 枚举 ↔ PG 标签的映射关系



## 自定义映射（mapping）
如果 Python 枚举名字和 PG 不一样，可以用 `mapping` 手动对应：

```python
register_enum(info, conn, UserRole, mapping=[
    (UserRole.ADMIN, "ABBOT"),
    (UserRole.EDITOR, "SCRIBE"),
])
```


