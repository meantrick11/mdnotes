# PostgreSQL SELECT DISTINCT

**概述**：在本教程中，你将学习如何使用 PostgreSQL `SELECT DISTINCT` 子句，从查询返回的结果集中去除重复行。

## PostgreSQL SELECT DISTINCT 子句简介
`SELECT DISTINCT` 用于从结果集中移除重复数据行，对于每一组重复数据，仅保留其中一行。

`SELECT DISTINCT` 可以作用于 `SELECT` 语句查询列表中的一列或多列。

以下是 `DISTINCT` 子句的语法：
```sql
SELECT DISTINCT column1 FROM table_name;
```
在该语法中，`SELECT DISTINCT` 会依据 `column1` 字段的值判断数据是否重复。

如果你指定多列，`SELECT DISTINCT` 会根据这些列的值组合判断重复。示例如下：
```sql
SELECT DISTINCT column1, column2 FROM table_name;
```
该语法下，`SELECT DISTINCT` 将同时结合 `column1` 和 `column2` 两列的值来判定重复行。

注意：PostgreSQL 还提供 `DISTINCT ON` 子句，用于保留结果集中单列或多列组合的第一条唯一记录。

如果你需要查询整张表所有列的不重复数据，可以使用 `SELECT DISTINCT *`：
```sql
SELECT DISTINCT * FROM table_name;
```
星号 `*` 代表数据表的全部字段。

## PostgreSQL SELECT DISTINCT 使用示例
我们新建一张数据表来练习 `SELECT DISTINCT` 子句。

注意：你会在后续教程中学习建表和插入数据的完整语法。本教程仅需在 psql 或 pgAdmin 中执行下方语句即可。

第一步：执行 `CREATE TABLE` 语句创建 `colors` 表，包含 id、bcolor、fcolor 三列：
```sql
CREATE TABLE colors(
  id SERIAL PRIMARY KEY,
  bcolor VARCHAR,
  fcolor VARCHAR
);
```

第二步：向 `colors` 表插入若干数据行：
```sql
INSERT INTO colors (bcolor, fcolor) VALUES
('red', 'red'),
('red', 'red'),
('red', NULL),
(NULL, 'red'),
(NULL, NULL),
('green', 'green'),
('blue', 'blue'),
('blue', 'blue');
```

第三步：执行 `SELECT` 查询 `colors` 表全部数据：
```sql
SELECT id, bcolor, fcolor FROM colors;
```

输出结果：
```
id | bcolor | fcolor
----+--------+--------
  1 | red    | red
  2 | red    | red
  3 | red    | null
  4 | null   | red
  5 | null   | null
  6 | green  | green
  7 | blue   | blue
  8 | blue   | blue
(8 rows)
```

### 1）单列使用 SELECT DISTINCT 示例
以下语句查询 `colors` 表 `bcolor` 列的唯一值，并使用 `ORDER BY` 子句按字母顺序对结果集排序。
```sql
SELECT DISTINCT bcolor FROM colors ORDER BY bcolor;
```

输出结果：
```
bcolor
--------
blue
green
red
null
(4 rows)
```

`bcolor` 字段包含3个red、2个null、1个green、2个blue。`SELECT DISTINCT` 自动剔除了2个red、1个null和1个blue重复项。

注意：PostgreSQL 将所有 NULL 值视为重复，当使用 `SELECT DISTINCT` 时，所有 NULL 仅保留一行。

### 2）多列使用 SELECT DISTINCT
以下语句对 `bcolor` 和 `fcolor` 两列同时使用 `SELECT DISTINCT`：
```sql
SELECT DISTINCT bcolor, fcolor FROM colors ORDER BY bcolor, fcolor;
```

输出结果：
```
bcolor | fcolor
--------+--------
blue   | blue
green  | green
red    | red
red    | null
null   | red
null   | null
(6 rows)
```

本示例中，查询会同时结合 `bcolor` 和 `fcolor` 两列的值判断行是否唯一。

### 3）SELECT DISTINCT 实际业务用法
在实际开发场景中，经常使用 `SELECT DISTINCT` 分析字段内存在哪些不重复的值。

举例：你需要查询 film 表中所有影片的租赁费率。
可以在 `SELECT DISTINCT` 中指定 rental_rate 字段实现需求：
```sql
SELECT DISTINCT rental_rate FROM film ORDER BY rental_rate;
```

输出结果：
```
rental_rate
-------------
        0.99
        2.99
        4.99
(3 rows)
```

输出表明表中仅有三种不同的租赁费率：0.99、2.99、4.99。

注意：执行以上查询需要将当前数据库切换至 dvdrental，切换语句如下：
```sql
\c dvdrental
```

## 小结
- 使用 `SELECT DISTINCT` 可以移除查询结果集中的重复数据行。

