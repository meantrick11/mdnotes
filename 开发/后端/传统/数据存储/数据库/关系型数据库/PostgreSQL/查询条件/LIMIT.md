# PostgreSQL LIMIT 子句
**要点**：本教程学习如何使用 PostgreSQL `LIMIT` 子句截取查询结果中的部分数据行。

## PostgreSQL LIMIT 子句简介
`LIMIT` 是 `SELECT` 语句的可选子句，用于限制查询返回的行数。

基础语法：
```sql
SELECT
  列列表
FROM
  表名
ORDER BY
  排序字段
LIMIT
  行数;
```
执行逻辑：返回指定 `行数` 条数据。
- 若行数填 0：返回空结果集；
- 若行数为 NULL：等价于不写 LIMIT，返回全部数据。

如果需要先跳过若干行，再截取指定条数，搭配 `OFFSET` 使用，写在 LIMIT 后方：
```sql
SELECT
  列列表
FROM
  表名
ORDER BY
  排序字段
LIMIT  行数 OFFSET
  跳过行数;
```
执行逻辑：先跳过 `跳过行数` 条数据，再取出 `行数` 条。
- 跳过行数为 0 时，效果等同于不写 OFFSET；
- 执行顺序：PostgreSQL 会先执行 OFFSET，再执行 LIMIT。

重要注意：PostgreSQL 表内数据行本身无固定排序，使用 LIMIT 时**必须搭配 ORDER BY** 固定行顺序；缺少 ORDER BY 会导致每次查询返回行顺序混乱、结果不可控。

## PostgreSQL LIMIT 子句示例
示例基于示例库中的 `film` 表演示。

### 1）使用 LIMIT 限制返回行数
按影片 ID 升序，查询前 5 条影片：
```sql
SELECT
  film_id,
  title,
  release_year
FROM
  film
ORDER BY film_id
LIMIT  5;
```
输出：
```
film_id | title            | release_year
---------+------------------+--------------
 1      | Academy Dinosaur | 2006
 2      | Ace Goldfinger   | 2006
 3      | Adaptation Holes | 2006
 4      | Affair Prejudice | 2006
 5      | African Egg      | 2006
(5 行)
```
执行流程：
1. 通过 `ORDER BY film_id` 将影片按 ID 升序排序；
2. `LIMIT 5` 截取排序后的前 5 条数据。

### 2）LIMIT 搭配 OFFSET 分页
按 film_id 排序，跳过前 3 条，取后续 4 条（从第 4 条开始取 4 条）：
```sql
SELECT
  film_id,
  title,
  release_year
FROM
  film
ORDER BY film_id
LIMIT 4 OFFSET 3;
```
输出：
```
film_id | title            | release_year
---------+------------------+--------------
 4      | Affair Prejudice | 2006
 5      | African Egg      | 2006
 6      | Agent Truman     | 2006
 7      | Airplane Sierra  | 2006
(4 行)
```
执行流程：
1. 按影片 ID 升序排序；
2. `OFFSET 3` 跳过前 3 行；
3. `LIMIT 4` 截取接下来 4 行数据。

### 3）LIMIT 获取最大/最小 Top N 数据
常用场景：取租赁费率最高的前 10 部影片：
```sql
SELECT
  film_id,
  title,
  rental_rate
FROM
  film
ORDER BY rental_rate DESC
LIMIT  10;
```
输出：
```
film_id | title               | rental_rate
---------+---------------------+-------------
 13     | Ali Forever         | 4.99
 20     | Amelie Hellfighters | 4.99
 7      | Airplane Sierra     | 4.99
 10     | Aladdin Calendar    | 4.99
 2      | Ace Goldfinger      | 4.99
 8      | Airport Pollock     | 4.99
 98     | Bright Encounters   | 4.99
 133    | Chamber Italian     | 4.99
 384    | Grosse Wonderful    | 4.99
 21     | American Circus     | 4.99
(10 行)
```
执行流程：
1. `ORDER BY rental_rate DESC` 将影片按租赁费率从高到低排序；
2. `LIMIT 10` 截取排序后前 10 条。

## 小结
- 使用 PostgreSQL `LIMIT` + `OFFSET` 可以截取查询结果的局部数据，常用于分页查询。

