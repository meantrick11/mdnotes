# 建立连接--Engine部分
每个连接数据库的 SQLAlchemy 应用都需要使用一个Engine。这一小节面向所有读者。

任何 SQLAlchemy 应用程序的起点都是一个名为Engine的对象。该对象充当与特定数据库连接的中央来源，既为这些数据库连接提供工厂，又提供一个称为连接池的存储空间。引擎通常是全局对象，针对特定的数据库服务器仅创建一次，并通过一个 URL 字符串进行配置，该字符串会说明其应如何连接到数据库主机或后端。

## 创建Engine
在本教程中，我们将仅使用内存中的 SQLite 数据库。这是一种无需预先设置实际数据库即可测试内容的简便方法。Engine 是通过 create_engine() 函数创建的：
```python
from sqlalchemy import create_engine
engine = create_engine("sqlite+pysqlite:///:memory:", echo=True)
```

传给create_engine的主要参数是一个字符串形式的URL，即上面传入的"sqlite+pysqlite:///:memory:"。该字符串向Engine传达了三个重要信息：
1. 我们正在与哪种数据库通信？这是上方的sqlite部分，它将 SQLAlchemy 连接到一个名为方言的对象。可以是sqlite，也可以是postgresql

2. 我们使用的是什么 DBAPI？Python 版 DBAPI 是 SQLAlchemy 用来与特定数据库交互的第三方驱动。在本例中，我们使用的名称是 pysqlite，在现代 Python 中，它是用于 SQLite 的 sqlite3 标准库接口。**如果省略该名称，SQLAlchemy 会为所选的特定数据库使用默认的 DBAPI。**

3. 我们该如何定位数据库？在本例中，我们的 URL 包含了/:memory:这一字符串，它是向sqlite3模块发出的一个标识，表明我们将使用一个仅内存型数据库。这种数据库非常适合进行实验，因为它既不需要任何服务器，也无需创建新文件。

**Engine 首次通过 create_engine() 返回时，实际上尚未尝试连接数据库；只有在首次被要求对数据库执行任务时，才会进行连接。这是一种被称为 延迟初始化 的软件设计模式。**

*我们还指定了一个参数 create_engine.echo，该参数将指示 Engine 将其生成的所有 SQL 日志记录到一个 Python 日志器中，该日志器会输出到标准输出。此标志是更正式地设置 Python 日志记录的简便方式，在脚本的实验阶段非常有用。许多 SQL 示例会在 [SQL] 链接下方包含此 SQL 日志输出，点击该链接即可查看完整的 SQL 交互过程。*



