# PostgreSQL FETCH 子句

**要点**：本教程学习使用 PostgreSQL `FETCH` 子句截取查询结果中的指定行数。

## PostgreSQL FETCH 子句简介
日常分页截取数据常使用 `LIMIT`，MySQL、H2 等数据库都支持 LIMIT，但它**不属于标准 SQL**。
为兼容 SQL 标准（SQL:2008 引入），PostgreSQL 提供了 `FETCH` 语法实现跳过行、限定返回行数的分页能力。

### 标准语法
```sql
OFFSET 跳过行数 { ROW | ROWS }
FETCH { FIRST | NEXT } [ 返回行数 ] { ROW | ROWS } ONLY
```
语法说明：
1. `OFFSET row_to_skip`：指定需要跳过的行数，必须为大于等于 0 的整数，默认值 0（不跳过任何行）；若跳过行数大于表总行数，查询返回空结果。
2. `FETCH ... ONLY`：指定需要取出的行数，数值必须 ≥1，省略数字时默认取 1 行。
3. 关键字等价替换：
   - `ROW` 与 `ROWS` 完全通用；
   - `FIRST` 与 `NEXT` 完全通用，可随意互换。

### 重要注意事项
1. 数据表原生行无固定排序，使用 FETCH 时**必须搭配 ORDER BY**，保证分页结果稳定；
2. SQL:2008 标准规定 OFFSET 必须写在 FETCH 前面，但 PostgreSQL 放宽限制，二者顺序可任意调换。

### FETCH 对比 LIMIT
两者功能完全等价，区别在于规范：
- `LIMIT`：非标准 SQL，MySQL 等数据库专用；
- `FETCH`：SQL 2008 标准语法，跨数据库兼容性更好，多数据库适配项目推荐使用。

## PostgreSQL FETCH 示例
基于示例库 `film` 表演示。

### 1）只取排序后的第 1 条数据
按影片名称升序，取第一条影片：
```sql
SELECT
  film_id,
  title
FROM
  film
ORDER BY
  title
FETCH FIRST ROW ONLY;
```
输出：
```
film_id | title
---------+------------------
1       | Academy Dinosaur
(1 行)
```
等价写法（显式指定取 1 行）：
```sql
SELECT
  film_id,
  title
FROM
  film
ORDER BY
  title
FETCH FIRST 1 ROW ONLY;
```

### 2）取排序后前 5 条数据
按影片名称升序，取前 5 部影片：
```sql
SELECT
  film_id,
  title
FROM
  film
ORDER BY
  title
FETCH FIRST 5 ROW ONLY;
```
输出：
```
film_id | title
---------+------------------
1       | Academy Dinosaur
2       | Ace Goldfinger
3       | Adaptation Holes
4       | Affair Prejudice
5       | African Egg
(5 行)
```

### 3）OFFSET + FETCH 分页（跳过前5条，再取5条）
跳过前5条影片，读取接下来5条：
```sql
SELECT
  film_id,
  title
FROM
  film
ORDER BY
  title
OFFSET 5 ROWS
FETCH FIRST 5 ROW ONLY;
```
输出：
```
film_id | title
---------+------------------
6       | Agent Truman
7       | Airplane Sierra
8       | Airport Pollock
9       | Alabama Devil
10      | Aladdin Calendar
(5 行)
```

## 小结
- 使用 `OFFSET + FETCH` 实现分页，跳过指定行数并截取固定数量查询结果；
- 跨数据库项目优先选择标准 SQL 的 FETCH 语法。

