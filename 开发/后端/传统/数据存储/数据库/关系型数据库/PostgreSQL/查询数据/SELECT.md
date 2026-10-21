# PostgreSQL SELECT

**概述**：本教程将带你学习基础的 **PostgreSQL SELECT** 语句，实现从数据表中查询数据。

如果你尚不了解如何使用 **psql 命令行工具** 或 **pgAdmin 图形化工具** 连接 PostgreSQL 执行查询，可以先阅读 PostgreSQL 数据库连接教程。

操作数据库最常用的任务，就是通过 `SELECT` 从数据表中读取数据。

`SELECT` 是 PostgreSQL 语法最复杂的语句之一，包含大量子句，支持灵活构建各类查询。

由于内容复杂，我们将它拆分为多篇简短易懂的教程，方便你逐个掌握各个子句：
- 使用 `DISTINCT` 运算符对查询结果去重
- 使用 `ORDER BY` 子句对结果排序
- 使用 `WHERE` 子句过滤数据行
- 使用 `LIMIT` 或 `FETCH` 子句获取表中的部分数据
- 使用 `GROUP BY` 子句将数据分组
- 使用 `HAVING` 子句过滤分组结果
- 使用各类连接语法关联其他表：`INNER JOIN`、`LEFT JOIN`、`FULL OUTER JOIN`、`CROSS JOIN`
- 使用集合运算：`UNION`、`INTERSECT`、`EXCEPT`

本篇教程重点讲解 `SELECT` 和 `FROM` 基础子句。

## PostgreSQL SELECT 语句语法
我们先学习单表查询场景下基础形式的 `SELECT`。

基础语法格式：
```sql
SELECT 选择列表
FROM 表名;
```

语法说明：
1. 首先指定**选择列表**：可以是表中的一列，或是多列。多列之间使用逗号 `,` 分隔。如果你希望查询表内**所有列**，可以直接使用星号 `*` 简写，不用手动列出全部字段。选择列表同样支持表达式、常量字面量。
2. 在 `FROM` 关键字之后，指定需要查询的数据表名称。

`FROM` 子句是可选的。当你不需要从任何数据表读取数据时，可以直接省略 `FROM`。

执行顺序规则：PostgreSQL **先执行 FROM 子句，再执行 SELECT 子句**。

> 注意：SQL 关键字大小写不敏感。`SELECT`、`select`、`Select` 效果完全一致。行业惯例通常将 SQL 关键字大写，提升可读性。

## PostgreSQL SELECT 使用示例
下面演示 SELECT 的各类用法。
示例基于 `dvdrental` 示例数据库中的 `customer`（客户）表。
*之前的快速开始一节有讲如何将教学示例的dvdrental导入PostgreSQL数据库中。*
第一步：使用 postgres 用户连接 PostgreSQL 服务
```bash
psql -U postgres
```
系统会提示输入 postgres 用户密码，密码验证通过后进入终端：
```
postgres=#
```

第二步：切换到 dvdrental 数据库
```sql
\c dvdrental
```

完成后，即可执行下面所有示例 SQL。

### 1）查询单列数据
示例：从 customer 表查询所有客户的名字
```sql
SELECT first_name FROM customer;
```

部分输出结果：
```
first_name
-------------
Jared
Mary
Patricia
Linda
Barbara
...
```

补充说明：SQL 语句末尾的分号 `;` 不属于 SQL 语法本身，它是 psql 的结束标识，通知数据库本条语句输入完成；同时分号也用于分隔多条独立 SQL。

### 2）查询多列数据
查询客户名字、姓氏、邮箱字段：
```sql
SELECT first_name, last_name, email FROM customer;
```

部分输出：
```
first_name | last_name | email
-------------+--------------+------------------------------------------
Jared       | Ely          | jared.ely@example.com
Mary        | Smith        | mary.smith@example.com
Patricia    | Johnson      | patricia.johnson@example.com
...
```

返回结果会依次展示 first_name、last_name、email 三列。

### 3）查询整张表全部列
使用 `SELECT *` 查询表中所有字段：
```sql
SELECT * FROM customer;
```
`*` 是表内所有列的简写。

> ⚠️ 生产环境开发规范：不推荐随意使用 `SELECT *`，建议显式写出业务真正需要的字段，减少网络数据传输开销。

### 4）使用表达式查询
`SELECT` 后的选择列表不仅可以写字段名，还支持表达式。

示例：拼接客户姓名
```sql
SELECT first_name || ' ' || last_name FROM customer;
```

输出示例：
```
?column?
-----------------
Jared Ely
Mary Smith
Patricia Johnson
...
```

你可以看到结果列标题自动命名为 `?column?`，可读性较差。我们可以使用别名优化。

### 5）使用列别名（AS）
使用 `AS` 为列设置别名，方便阅读结果：
```sql
SELECT first_name || ' ' || last_name AS full_name
FROM customer;
```

输出：
```
full_name
-----------------
Jared Ely
Mary Smith
Patricia Johnson
...
```

关键字 `AS` 可以省略，下面写法等价：
```sql
SELECT first_name || ' ' || last_name full_name
FROM customer;
```

如果别名包含空格、特殊字符，可以用双引号包裹：
```sql
SELECT first_name || ' ' || last_name AS "Full Name"
FROM customer;
```

### 6）查询常量字面量
SELECT 可以不带 FROM，直接输出常量：
```sql
SELECT 'Hello PostgreSQL' AS message, 2026 AS year;
```

输出：
```
message            | year
-------------------+------
Hello PostgreSQL   | 2026
```

## 小结
- `SELECT 字段列表 FROM 表名` 是最基础的查询模板；
- `*` 查询全部字段，生产环境尽量避免；
- SELECT 支持字段、表达式、常量；
- 使用 `AS` 设置列别名，优化结果可读性；
- FROM 子句可省略，用于直接计算常量表达式。

