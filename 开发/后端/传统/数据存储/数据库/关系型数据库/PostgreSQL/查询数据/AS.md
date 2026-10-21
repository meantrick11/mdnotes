# PostgreSQL 列别名

**概述**：在本教程中，你将学习 PostgreSQL 列别名，掌握如何为查询中的字段设置临时名称。

## PostgreSQL 列别名简介
列别名允许你为 `SELECT` 查询列表中的字段或表达式指定一个临时名称。列别名仅在这条查询执行期间生效。

使用列别名的语法如下：
```sql
SELECT 字段名 AS 别名 FROM 表名;
```
语法中，为 `字段名` 设置别名 `别名`。`AS` 关键字是可选的，你可以省略，简写形式：
```sql
SELECT 字段名 别名 FROM 表名;
```

下面的语法展示如何为 `SELECT` 子句里的表达式设置别名：
```sql
SELECT 表达式 AS 别名 FROM 表名;
```

使用列别名的主要目的，是让查询结果的表头名称更具备业务含义，方便阅读。

### PostgreSQL 列别名示例
我们将使用示例数据库中的 `customer` 客户表演示列别名用法。

### 1）为普通字段设置列别名
下面这条查询从 `customer` 表查询所有客户的名与姓：
```sql
SELECT first_name, last_name FROM customer;
```

如果你想修改 `last_name` 的表头名称，可以通过列别名实现：
```sql
SELECT first_name, last_name AS surname FROM customer;
```
这条语句将 `last_name` 的别名设置为 `surname`。

也可以去掉 `AS` 关键字简写，效果完全相同：
```sql
SELECT first_name, last_name surname FROM customer;
```

### 2）为表达式设置列别名
下面的查询拼接客户的名、空格、姓氏，输出所有人的完整姓名：
```sql
SELECT first_name || ' ' || last_name FROM customer;
```
> 补充：在 PostgreSQL 中，`||` 是字符串拼接运算符，可以将多个字符串合并为一个字符串。

你能发现，查询结果的表头自动显示为 `?column?`，不具备可读性。

解决办法：给拼接表达式设置别名，例如 `full_name`：
```sql
SELECT first_name || ' ' || last_name AS full_name FROM customer;
```

### 3）包含空格的列别名
如果列别名中包含空格，需要使用**双引号**把别名包裹起来：
```sql
字段名 AS "带空格的别名"
```

示例：
```sql
SELECT first_name || ' ' || last_name "full name" FROM customer;
```
*注意：SQL语法中的单引号表示字符串、文本值，而双引号则大多表示标识符，比如表名、列名、别名等需要带有关键字、空格的情况。*
## 小结
- 使用 `字段名 AS 别名` 或 `表达式 AS 别名` 为字段、表达式设置列别名；`AS` 关键字可以省略。
- 如果别名带有空格，必须使用双引号 `" "` 包裹别名。

上一节：PostgreSQL SELECT
下一节：PostgreSQL ORDER BY

本页目录
- PostgreSQL 列别名简介
  - PostgreSQL 列别名示例
    - 1）为普通字段设置列别名
    - 2）为表达式设置列别名
    - 3）包含空格的列别名
- 小结
