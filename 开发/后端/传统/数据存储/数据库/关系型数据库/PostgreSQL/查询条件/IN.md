# PostgreSQL IN 运算符
**要点**：本教程学习 PostgreSQL `IN` 运算符，用于判断字段值是否匹配列表内任意一项。

## PostgreSQL IN 运算符简介
`IN` 运算符用于校验某个值是否存在于给定值列表中。
基础语法：
```sql
值 IN (值1, 值2, ...)
```
规则：当左侧**值**等于列表中任意一项时，表达式返回 true。
列表支持数字、字符串字面量；同时也支持子查询返回的结果集（子查询用法将在后续教程讲解）。

逻辑等价：`IN` 等同于多个条件用 `OR` 拼接，可读性与执行效率更高：
```sql
值 = 值1 OR 值 = 值2 OR ...
```

## PostgreSQL IN 运算符示例
示例使用官方示例库中的 `film`、`actor`、`payment` 三张表演示。

### 1）数字列表搭配 IN
查询影片 ID 为 1、2、3 的影片信息：
```sql
SELECT
  film_id,
  title
FROM
  film
WHERE
  film_id IN (1, 2, 3);
```
输出：
```
film_id | title
---------+------------------
1       | Academy Dinosaur
2       | Ace Goldfinger
3       | Adaptation Holes
(3 行)
```
等价写法（多 OR 拼接，代码更长、性能更差）：
```sql
SELECT
  film_id,
  title
FROM
  film
WHERE
  film_id = 1 OR film_id = 2 OR film_id = 3;
```

### 2）字符串列表搭配 IN
基于 `actor` 表，查询姓氏为 Allen、Chase、Davis 的演员：
```sql
SELECT
  first_name,
  last_name
FROM
  actor
WHERE
  last_name IN ('Allen', 'Chase', 'Davis')
ORDER BY last_name;
```
输出：
```
first_name | last_name
------------+-----------
Meryl      | Allen
Cuba       | Allen
Kim        | Allen
Jon        | Chase
Ed         | Chase
Susan      | Davis
Jennifer   | Davis
Susan      | Davis
(8 行)
```

### 3）日期列表搭配 IN
`payment` 表 `payment_date` 字段为 timestamp 类型（包含日期+时分秒），通过 `::date` 强制转换仅保留日期，匹配指定日期列表 `2007-02-15`、`2007-02-16`：
```sql
SELECT
  payment_id,
  amount,
  payment_date
FROM
  payment
WHERE
  payment_date::date IN ('2007-02-15', '2007-02-16');
```
输出：
```
payment_id | amount | payment_date
------------+--------+----------------------------
17503      | 7.99   | 2007-02-15 22:25:46.996577
17504      | 1.99   | 2007-02-16 17:23:14.996577
17505      | 7.99   | 2007-02-16 22:41:45.996577
17512      | 4.99   | 2007-02-16 00:10:50.996577
...
```
转换说明：`payment_date::date` 把完整时间戳截取为纯日期格式。

## PostgreSQL NOT IN 运算符
`NOT IN` 是 IN 的取反语法，判断值**不在**列表中。
基础语法：
```sql
值 NOT IN (值1, 值2, ...)
```
规则：左侧值和列表所有项都不相等时返回 true；存在任意一项相等则返回 false。
逻辑等价：多个不等条件用 `AND` 拼接：
```sql
值 <> 值1 AND 值 <> 值2 AND ...
```

### NOT IN 代码示例
查询 ID 不等于 1、2、3 的所有影片：
```sql
SELECT
  film_id,
  title
FROM
  film
WHERE
  film_id NOT IN (1, 2, 3)
ORDER BY film_id;
```
输出：
```
film_id | title
---------+-----------------------------
4       | Affair Prejudice
5       | African Egg
6       | Agent Truman
7       | Airplane Sierra
8       | Airport Pollock
...
```
等价写法（多 AND 拼接）：
```sql
SELECT
  film_id,
  title
FROM
  film
WHERE
  film_id <> 1 AND film_id <> 2 AND film_id <> 3
ORDER BY film_id;
```

## 小结
1. `IN`：判断字段值是否存在于指定列表，简化多 OR 条件；
2. `NOT IN`：判断字段值不在指定列表，简化多 AND 不等条件；
3. IN 支持数字、字符串、日期，也可配合子查询使用。

