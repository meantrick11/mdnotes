# 使用Transactions & the DBAPI

准备好Engine对象后，我们可以深入了解Engine的基本操作及其主要端点，即Connection和Result。我们还将介绍 ORM 针对这些对象的外观，也就是Session。

使用 ORM 时，Engine 由 Session 管理。现代 SQLAlchemy 中的 Session 强调一种事务性和 SQL 执行模式，该模式与下文讨论的 Connection 的执行模式基本一致，因此尽管本小节以核心层为中心，但此处的所有概念也都适用于 ORM 的使用，建议所有 ORM 学习者学习。本节末尾将把 Connection 所使用的执行模式与 Session 进行对比。

由于我们尚未介绍作为 SQLAlchemy 核心特性的 SQLAlchemy 表达式语言，我们将使用该包中一个简单的构造——text() 构造——来以 文本 SQL 的形式编写 SQL 语句。请放心，在日常的 SQLAlchemy 使用中，文本 SQL 属于特例而非常态，但它始终是可用的。


## 获取连接

Engine的用途是通过提供一个Connection对象来连接数据库。在直接使用Core进行操作时，所有与数据库的交互都是通过Connection对象完成的。由于Connection会与数据库建立开放资源，因此我们希望将该对象的使用限制在特定上下文中。实现这一点的最佳方式是使用Python上下文管理器，也称为with语句。下面我们使用文本SQL语句来展示“Hello World”。文本SQL是通过名为text()的构造创建的，我们将在后面详细讨论它：

```python
from sqlalchemy import text

with engine.connect() as conn:
    result = conn.execute(text("select 'hello world'"))
    print(result.all())

```

**text(SQL)包裹的是原生的SQL语句，直接会在Database中执行**

在上面的示例中，上下文管理器会创建一个数据库连接，并在事务中执行操作。Python DBAPI 的默认行为是事务始终处于开启状态；当连接被释放时，会发出一条回滚（ROLLBACK）语句来结束事务。事务不会自动提交；如果我们想要提交数据，就需要调用Connection.commit()，这一点我们将在下一节中看到。

**我们的 SELECT 语句的结果会返回到一个名为Result的对象中，这个对象我们稍后会讨论。目前我们要补充一点，最好在“connect”代码块中使用这个对象，而不要在我们的连接作用域之外使用它。**

## 提交更改

我们刚刚了解到** DBAPI 连接不会自动提交。**如果我们想要提交某些数据该怎么办呢？我们可以修改上面的示例，创建一个表、插入一些数据，然后在拥有 Connection 对象的 内部 代码块中，使用 Connection.commit() 方法提交事务：

```python
with engine.connect() as conn:
    conn.execute(text("CREATE TABLE some_table (x int, y int)"))
    conn.execute(
        text("INSERT INTO some_table (x, y) VALUES (:x, :y)"),
        [{"x": 1, "y": 1}, {"x": 2, "y": 4}],
    )
    conn.commit()
```

以上，我们执行了两条 SQL 语句，一条是 `[1]` 中的“CREATE TABLE”语句，另一条是带参数化的“INSERT”语句（我们将在 `发送多个参数` 中稍后讨论参数化语法）。要提交我们在代码块中完成的操作，我们会调用 `Connection.commit()` 方法来提交事务。完成这一步后，我们可以继续执行更多 SQL 语句，并再次调用 `Connection.commit()` 来提交这些语句的事务。SQLAlchemy 将这种方式称为 `逐次提交`。

还有另一种提交数据的方式。我们可以事先将我们的“连接”块声明为事务块。为此，我们使用Engine.begin()方法来获取连接，而不是使用Engine.connect()方法。该方法将管理Connection的作用域，并且还会将事务内的所有内容都封装起来——如果块执行成功，则在末尾执行COMMIT；如果引发异常，则执行ROLLBACK。这种方式被称为**一次性开启**：

```python
with engine.begin() as conn:
    conn.execute(
        text("INSERT INTO some_table (x, y) VALUES (:x, :y)"),
        [{"x": 6, "y": 8}, {"x": 9, "y": 10}],
    )
```

**Engine.begin()的连接方式会在Connction域执行过后自动提交，所以无论执行多少次，那么都会一起提交，即为一次性提交。而Engine.connect()方法就不会自动提交，需要手动cnt.commit().**


**你应该优先选择“一次性开始”的方式，因为它更简洁，还能提前展示整个代码块的意图。不过在本教程中，我们会使用“边做边提交”的方式，因为它在演示时更灵活。**

DDL 指的是 SQL 中用于指示数据库创建、修改或删除表等模式级结构的子集。像“CREATE TABLE”这样的 DDL 语句应位于以 COMMIT 结尾的事务块中，因为许多数据库采用事务性 DDL，即事务提交后模式更改才会生效。不过，正如我们稍后会看到的，我们通常会让 SQLAlchemy 在更高级别的操作中执行 DDL 序列，此时一般无需操心 COMMIT 相关事宜。

## 语句执行基础

我们已经见过几个针对数据库执行 SQL 语句的示例，这些示例使用了名为 Connection.execute() 的方法，结合一个名为 text() 的对象，并返回一个名为 Result 的对象。在本节中，我们将更详细地说明这些组件的工作原理及交互方式。

### 获取行

我们将首先利用之前插入的行，通过对创建的表执行文本 SELECT 语句，来更详细地说明 Result 对象：

```python
with engine.connect() as conn:
    result = conn.execute(text("SELECT x, y FROM some_table"))
    for row in result:
        print(f"x: {row.x}  y: {row.y}")
```

*知晓SQL语句的读友不难知道上面的SQL语句是从some_table中选中所有的x&y列的组成的行整体返回*

上面，我们执行的“SELECT”语句从表中选择了所有行。返回的对象称为Result，它表示结果行的可迭代对象。

Result 有许多用于获取和转换行的方法，例如前面所示的 Result.all() 方法，该方法返回所有 Row 对象的列表。它还实现了 Python 迭代器接口，因此我们可以直接迭代 Row 对象集合。

**可以将result理解为一个包含很多元组的可迭代对象，每个元组由具体的sql语句查询出来的实例组成(每一行具体的数据)**

Row 对象本身旨在充当类似 Python 具名元组的角色。下面我们将演示访问行的多种方式。

- **元组赋值**——这是最具 Python 特色的写法，即按照接收到的顺序为每行的各个变量进行赋值：
```python
result = conn.execute(text("select x, y from some_table"))

for x, y in result:
    ...
```

- **整数索引** - 元组是 Python 序列，因此也支持常规的整数索引访问：
```python
result = conn.execute(text("select x, y from some_table"))

for row in result:
    x = row[0]
```

- **属性名称** - 由于这些是 Python 命名元组，元组拥有与各列名称匹配的动态属性名。这些名称通常是 SQL 语句为每行中的列分配的名称。尽管它们通常相当可预测，也可通过标签进行控制，但在定义不明确的情况下，它们可能会受特定数据库行为的影响：
```python
result = conn.execute(text("select x, y from some_table"))

for row in result:
    y = row.y

    # illustrate use with Python f-strings
    print(f"Row: {row.x} {y}")
```

- **映射访问** - 要以 Python 映射对象的形式获取行（本质上是 Python 面向普通 dict对象接口的只读版本），可通过 Result.mappings() 修饰符将 Result 对象 转换为 MappingResult 对象；这是一种结果对象，会生成类似字典的 RowMapping 对象，而非 Row 对象：
```python
result = conn.execute(text("select x, y from some_table"))

for dict_row in result.mappings():
    x = dict_row["x"]
    y = dict_row["y"]
```


### 传递参数

正如我们在之前的 INSERT 示例中所见，SQL 语句通常会伴随要与语句本身一同传递的数据。因此，Connection.execute() 方法也接受参数，这些参数被称为绑定参数。一个简单的例子是，我们希望将 SELECT 语句仅限制为满足特定条件的行，例如“y”值大于传入函数的某个值的行。

为了实现这一点，使 SQL 语句保持固定且驱动程序能够正确清理值，我们向语句中添加了一个 WHERE 条件，命名一个名为“y”的新参数；text() 构造使用冒号格式“:y”来接受这些参数。随后，“:y”的实际值以字典的形式作为第二个参数传递给 Connection.execute()：

```python
with engine.connect() as conn:
    result = conn.execute(text("SELECT x, y FROM some_table WHERE y > :y"), {"y": 2})
    for row in result:
        print(f"x: {row.x}  y: {row.y}")
```

在记录的 SQL 输出中，我们可以看到绑定参数 :y 在发送到 SQLite 数据库时被转换为了一个问号。这是因为 SQLite 数据库驱动使用一种名为“qmark 参数样式”的格式，这是 DBAPI 规范允许的六种不同格式之一。SQLAlchemy 将这些格式抽象为一种，即使用冒号的“命名”格式。

正如本节开头所提到的，文本 SQL 并非我们使用 SQLAlchemy 的常规方式。不过，在使用文本 SQL 时，Python 字面量值（即便是整数或日期这类非字符串类型）**绝不应直接把用户输入字符串化为 SQL 字符串**；**始终**应当使用参数。这一点最著名的作用是在处理不可信数据时避免 SQL 注入攻击。同时，它也能让 SQLAlchemy 的方言和/或 DBAPI 正确处理发往后端的输入数据。除了纯文本 SQL 的使用场景外，SQLAlchemy 的核心表达式 API 会在合适的情况下确保 Python 字面量值以绑定参数的形式传递。

### 传递多个参数

在提交更改的示例中，我们执行了一条 INSERT 语句，看起来能够一次性向数据库中插入多行数据。对于DML语句（如“INSERT”“UPDATE”和“DELETE”），我们可以向Connection.execute()方法传递字典列表而非单个字典，从而发送多个参数集，这表示该单条 SQL 语句会针对每个参数集被调用一次。这种执行方式被称为executemany：

```python
with engine.connect() as conn:
    conn.execute(
        text("INSERT INTO some_table (x, y) VALUES (:x, :y)"),
        [{"x": 11, "y": 12}, {"x": 13, "y": 14}],
    )
    conn.commit()
```

上述操作等同于针对每个参数集执行一次给定的 INSERT 语句，只不过该操作会经过优化，以提升处理大量行时的性能。

### 使用ORM会话执行

如前所述，上述大多数模式和示例也适用于 ORM，因此此处我们将介绍这种用法，以便在本教程后续内容中，我们能够结合 Core 和 ORM 的使用来阐释每种模式。

使用ORM时，基础的事务性/数据库交互对象被称为Session。在现代SQLAlchemy中，该对象的使用方式与Connection非常相似，实际上Session在使用时，其内部会引用一个Connection，并通过该连接来执行SQL语句。

当 Session 与非 ORM 结构一起使用时，它会传递我们提供的 SQL 语句，其整体操作方式与 Connection 直接执行时并无太大差异，因此我们可以结合已学过的简单文本 SQL 操作来在此处对其进行说明。

Session有几种不同的创建模式，但这里我们将说明最基本的一种，它与Connection的使用方式完全一致，即在上下文管理器中对其进行构造：

```python
from sqlalchemy.orm import Session

stmt = text("SELECT x, y FROM some_table WHERE y > :y ORDER BY x, y")
with Session(engine) as session:
    result = session.execute(stmt, {"y": 6})
    for row in result:
        print(f"x: {row.x}  y: {row.y}")
```

**注意Session接收的是engine，示例中省略了create_engine()的调用**

上面的示例可以与上一节发送参数中的示例进行对比——我们直接将对with engine.connect() as conn的调用替换为with Session(engine) as session，然后像使用Connection.execute()方法一样使用Session.execute()方法。

上面的示例可以与上一节发送参数中的示例进行对比——我们直接将对with engine.connect() as conn的调用替换为with Session(engine) as session，然后像使用Connection.execute()方法一样使用Session.execute()方法。

此外，与Connection类似，Session通过Session.commit()方法具备“随提交执行”的特性，下文将通过一条文本形式的 UPDATE 语句来修改我们的部分数据：
```python
with Session(engine) as session:
    result = session.execute(
        text("UPDATE some_table SET y=:y WHERE x=:x"),
        [{"x": 9, "y": 11}, {"x": 13, "y": 15}],
    )
    session.commit()
```

以上，我们使用在发送多个参数中介绍的带参数的“executemany”执行方式调用了一条 UPDATE 语句，并以“随提交随提交”的方式结束了该代码块。

**Session在结束事务后，实际上并不会持有Connection对象。下次需要针对数据库执行SQL语句时，它会从Engine获取一个新的Connection。**

**不过，Session 显然还有更多的功能。但只要了解它有一个 Session.execute() 方法，且其使用方式与 Connection.execute() 相同，就能为我们后续的示例讲解打下基础。**

