# PostgreSQL BETWEEN 区间运算符
**要点**：本教程讲解 `BETWEEN` 运算符，用于判断数值、日期是否落在指定闭区间内。

## PostgreSQL BETWEEN 运算符简介
`BETWEEN` 用于校验一个值是否处于一段连续区间内。
基础语法：
```sql
值 BETWEEN 下限 AND 上限;
```
判定规则：
当 `值 >= 下限` **且** `值 <= 上限` 时返回 true，否则返回 false。
等价原生比较写法：
```sql
值 >= 下限 AND 值 <= 上限
```

### NOT BETWEEN 取反区间
若要判断数值不在区间内，使用 `NOT BETWEEN`：
```sql
值 NOT BETWEEN 下限 AND 上限
```
等价写法（小于下限 或 大于上限）：
```sql
值 < 下限 OR 值 > 上限
```

### 使用场景
`BETWEEN` 常配合 `WHERE` 子句使用，支持 `SELECT` / `INSERT` / `UPDATE` / `DELETE` 各类DML语句。

## PostgreSQL BETWEEN 运算符示例
基于示例库 `payment` 表演示。

### 1）数字区间匹配 BETWEEN
查询支付ID在 17503 ~ 17505 之间的记录：
```sql
SELECT
  payment_id,
  amount
FROM
  payment
WHERE
  payment_id BETWEEN 17503 AND 17505
ORDER BY payment_id;
```
输出：
```
payment_id | amount
------------+--------
17503      | 7.99
17504      | 1.99
17505      | 7.99
(3 行)
```

### 2）NOT BETWEEN 排除区间
查询支付ID不在 17503 ~ 17505 的数据：
```sql
SELECT
  payment_id,
  amount
FROM
  payment
WHERE
  payment_id NOT BETWEEN 17503 AND 17505
ORDER BY payment_id;
```
输出：
```
payment_id | amount
------------+--------
17506      | 2.99
17507      | 7.99
17508      | 5.99
17509      | 5.99
17510      | 5.99
...
```

### 3）日期区间匹配 BETWEEN
日期采用 ISO 8601 标准格式 `YYYY-MM-DD`。
需求：查询支付日期在 2007-02-15 ~ 2007-02-20，且金额大于10的订单：
```sql
SELECT
  customer_id,
  payment_id,
  amount,
  payment_date
FROM
  payment
WHERE
  payment_date BETWEEN '2007-02-15' AND '2007-02-20'
  AND amount > 10
ORDER BY payment_date;
```
输出：
```
customer_id | payment_id | amount | payment_date
-------------+------------+--------+----------------------------
33          | 18640      | 10.99  | 2007-02-15 08:14:59.996577
544         | 18272      | 10.99  | 2007-02-15 16:59:12.996577
516         | 18175      | 10.99  | 2007-02-16 13:20:28.996577
572         | 18367      | 10.99  | 2007-02-17 02:33:38.996577
260         | 19481      | 10.99  | 2007-02-17 16:37:30.996577
477         | 18035      | 10.99  | 2007-02-18 07:01:49.996577
221         | 19336      | 10.99  | 2007-02-19 09:18:28.996577
(7 行)
```

## 小结
- `BETWEEN` 判断值是否落在**闭区间**（包含上下限）；
- `NOT BETWEEN` 判断值不在指定区间。

