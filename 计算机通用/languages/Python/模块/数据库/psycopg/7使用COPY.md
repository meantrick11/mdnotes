# 使用 COPY TO 和 COPY FROM
Psycopg 支持 PostgreSQL 的 COPY 协议。COPY 是向数据库加载数据最高效的方式之一（配合一些 SQL 技巧，也可以用来修改数据）。

通过调用 `Cursor.copy()` 方法使用复制功能，传入形如 `COPY ... FROM STDIN` 或 `COPY ... TO STDOUT` 的查询语句，并在 `with` 代码块中操作返回的 `Copy` 对象：
```python
with cursor.copy("COPY table_name (col1, col2) FROM STDIN") as copy:
    # 使用 write()/write_row() 将数据传递给 copy 对象
```

你可以使用 `psycopg.sql` 模块中的对象动态拼接 COPY 语句：
```python
with cursor.copy(
    sql.SQL("COPY {} TO STDOUT").format(sql.Identifier("table_name")) ) as copy:
    # 使用 read()/read_row() 从 copy 对象读取数据
```

版本3.1变更：你也可以像 `execute()` 一样给 `copy()` 传递参数：
```python
with cur.copy("COPY (SELECT * FROM table_name LIMIT %s) TO STDOUT", (3,)) as copy:
    # 最多读取三条记录
```

连接遵循常规事务行为，因此，除非连接开启自动提交，COPY 操作结束后你仍需要提交待变更的数据，也仍然可以执行回滚。更多细节请参阅事务管理。

## 逐行写入数据
借助复制操作，你可以从任意 Python 可迭代对象（元组列表，或任意序列迭代器）向数据库导入数据：Python 值的类型适配逻辑与普通查询完全一致。
想要实现该功能，使用 `COPY ... FROM STDIN` 调用 `Cursor.copy()`，并在 `with` 块内对返回对象调用 `write_row()`。退出代码块时复制操作就会结束。
```python
records = [(10, 20, "hello"), (40, None, "world")]
with cursor.copy("COPY sample (col1, col2, col3) FROM STDIN") as copy:
    for record in records:
        copy.write_row(record)
```

如果代码块内部抛出异常，复制操作会中断，已经插入的记录会全部丢弃。

> 如果你要逐行读写 Copy 对象，**不能指定** `FORMAT CSV`、`DELIMITER`、`NULL` 这类 COPY 参数，请不要修改这些配置。

## 逐行读取数据
你也可以反向操作，通过遍历 `rows()` 从 `COPY ... TO STDOUT` 读取行数据。但通常不推荐这么做，普通查询往往会更简单好用。

目前 PostgreSQL 在 COPY TO 场景不会返回完整的类型信息，返回的行是未解析的原始字符串或字节，取决于数据格式。
```python
with cur.copy("COPY (VALUES (10::int, current_date)) TO STDOUT") as copy:
    for row in copy.rows():
        print(row)  # 返回未解析数据: ('10', '2046-12-24')
```

读取前调用 `set_types()` 可以优化结果，但需要你手动指定字段类型：
```python
with cur.copy("COPY (VALUES (10::int, current_date)) TO STDOUT") as copy:
    copy.set_types(["int4", "date"])
    for row in copy.rows():
        print(row)  # (10, datetime.date(2046, 12, 24))
```

## 分块复制
如果数据已经是 COPY 兼容格式（例如来自上一次 COPY TO 导出的文件），可以改用 `Copy.write()` 批量导入数据库。
```python
with open("data", "r") as f:
    with cursor.copy("COPY data FROM STDIN") as copy:
        while data := f.read(BLOCK_SIZE):
            copy.write(data)
```

这种方式可以使用任意 COPY 参数与格式，前提是输入数据和 copy() 的预期格式兼容。文本格式下可以传入字符串；字节格式同时支持文本与二进制复制模式。

想要生成 COPY 格式的数据，可以执行 `COPY ... TO STDOUT`，迭代 Copy 对象会得到字节流：
```python
with open("data.out", "wb") as f:
    with cursor.copy("COPY table_name TO STDOUT") as copy:
        for data in copy:
            f.write(data)
```

## 二进制复制
在 COPY 语句指定 `FORMAT BINARY` 即可开启二进制复制。
> 使用 `write_row()` 导入二进制数据时，传入数据库的所有类型都必须注册二进制转储器；如果使用分块 `write()` 方式复制，则无此要求。

⚠️ 警告
PostgreSQL 对二进制模式导入数据校验十分严格，**不会执行任何类型转换**。
举例：向 integer 列传入值 100 会报错，因为 Psycopg 会以 smallint 类型发送，服务端会因为字段长度不匹配拒绝写入。

可以调用 Copy 对象的 `set_types()`，仔细指定加载类型来回避该问题。
更多二进制查询相关内容，参阅二进制参数与返回结果。

## 异步复制支持
异步复制使用和同步版本相同的编码模式，只是对象来自 `AsyncConnection`。
示例：`f` 是拥有异步 `read()` 方法的对象，完整异步复制示例：
```python
async with cursor.copy("COPY data FROM STDIN") as copy:
    while data := await f.read():
        await copy.write(data)
```

`AsyncCopy` 对象文档描述了异步方法签名，以及与同步 Copy 对象的差异。
更多异步用法，参阅异步操作。

## 示例：跨服务器复制表
可以使用两个不同连接上的 COPY，从源库读取，写入目标库，实现表（或部分表）跨服务器迁移。
```python
with psycopg.connect(dsn_src) as conn1, psycopg.connect(dsn_tgt) as conn2:
    with conn1.cursor().copy("COPY src TO STDOUT (FORMAT BINARY)") as copy1:
        with conn2.cursor().copy("COPY tgt FROM STDIN (FORMAT BINARY)") as copy2:
            for data in copy1:
                copy2.write(data)
```

使用 `FORMAT BINARY` 通常性能更高，但**源表和目标表结构必须完全一致**。
如果两张表只是兼容（例如源 integer 写入目标 bigint），不要使用二进制，改用文本复制模式。详见二进制复制小节。

这套逻辑也可以改成异步对象实现异步复制。

---
> 原文链接：https://www.psycopg.org/psycopg3/docs/basic/copy.html