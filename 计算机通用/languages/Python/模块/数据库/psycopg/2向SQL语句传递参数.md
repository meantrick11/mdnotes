# 向SQL语句中传递参数

## excute() 参数

通过`with conn.cursor() as cur`建立的操作对象之后通过执行对应的execute函数中的第二个位置进行传参

```python
cur.execute("""
    INSERT INTO some_table (id,created_at,last_name)
""",    #注意，这里是",'进行传参，而不是%
(10,datetime.date(2026,8,24),"O'Reilly"))
```
以上python代码会被传递给PostgreSQL程序中如以下：

```sql
INSERT INTO some_table (id,created_at,last_name)
VALUES (10,"2026-8-24",'o''Reilly')
```

或者使用关键字传参，不过需要在execute的语句中指定对应的字段名：

```python
cur.execute("""
    INSERT INTO some_table (id, created_at, updated_at, last_name)
    VALUES (%(id)s, %(created)s, %(created)s, %(name)s);
    """,
    {'id': 10, 'name': "O'Reilly", 'created': datetime.date(2020, 11, 18)})
```
SQL语句中的单双引号的功能作用不同，单引号表示的是单个字符/字符串/文本值，而双引号用来包裹特殊表名/列名/字段名等

以下展示一些错误和正确的用法：
1. 不要使用%进行值传递
```python
cur.execute("INSERT INTO numbers VALUES (%s, %s)" % (10, 20)) # WRONG
cur.execute("INSERT INTO numbers VALUES (%s, %s)", (10, 20))  # correct
```
2. 对于可变参数传递必须是一个序列
```python
cur.execute("INSERT INTO foo VALUES (%s)", "bar")    # WRONG
cur.execute("INSERT INTO foo VALUES (%s)", ("bar"))  # WRONG
cur.execute("INSERT INTO foo VALUES (%s)", ("bar",)) # correct
cur.execute("INSERT INTO foo VALUES (%s)", ["bar"])  # correct
```
3. 占位符不能被引号包裹
```python
cur.execute("INSERT INTO numbers VALUES ('%s')", ("Hello",)) # WRONG
cur.execute("INSERT INTO numbers VALUES (%s)", ("Hello",))   # correct
```
4. 可变变量传递的占位符必须是%s，不能是%d等
```python
cur.execute("INSERT INTO numbers VALUES (%d)", (10,))   # WRONG
cur.execute("INSERT INTO numbers VALUES (%s)", (10,))   # correct
```
5. 占位符只能代替数值占位，而不能是表等占位符

```python
cur.execute("INSERT INTO %s VALUES (%s)", ('numbers', 10))  # WRONG
cur.execute(                                                # correct
    SQL("INSERT INTO {} VALUES (%s)").format(Identifier('numbers')),
    (10,))
```

## SQL注入

SQL语句中的单引号表示当前str的字符串边界，但Python之中str可以是单双引号，如果外部使用双引号包裹，而内部直接拼接字符串参数，那么将会导致SQL语句中的字符串提前闭合，**永远不要直接进行使用%或者+进行拼接SQL字符串等操作，而应该使用占位符及其驱动进行处理**

```python
SQL = "INSERT INTO authors (name) VALUES ('%s')" # NEVER DO THIS
data = ("O'Reilly", )
cur.execute(SQL % data) # THIS WILL FAIL MISERABLY
# SyntaxError: syntax error at or near "Reilly"
```
而是应该使用：
```python
SQL = "INSERT INTO authors (name) VALUES (%s)"  # Note: no quotes
data = ("O'Reilly", )
cur.execute(SQL, data)  # Note: no % operator
```
- %s只是给 psycopg 驱动看的标记：“这个位置后面填参数”。
- SQL 模板和数据分开传递，数据库驱动内部处理：自动转义单引号、自动给字符串加单引号，不会在 Python 层拼接字符串。
- 最终数据库收到：模板 SQL + 独立参数，不会发生注入。