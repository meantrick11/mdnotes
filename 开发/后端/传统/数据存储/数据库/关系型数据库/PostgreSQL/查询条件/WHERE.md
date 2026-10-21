# PostgreSQL WHERE 子句

**要点**：本教程学习如何使用 PostgreSQL `WHERE` 子句过滤 `SELECT` 返回的数据行。

## PostgreSQL WHERE 子句简介
`SELECT` 语句会返回表中一列或多列的全部行。如果只想获取满足指定条件的行，就需要使用 `WHERE` 子句。

PostgreSQL 的 WHERE 子句语法：
```sql
SELECT
  列列表
FROM
  表名
WHERE
  条件
ORDER BY
  排序表达式;
```
语法中，`WHERE` 子句写在 `FROM` 子句之后。

`WHERE` 使用条件过滤查询返回的行。

条件是布尔表达式，结果为 true（真）、false（假）或 unknown（未知）。

查询只会返回 WHERE 条件结果为 true 的行。

PostgreSQL 执行顺序：先执行 `FROM`，再执行 `WHERE`，之后才执行 `SELECT` 和 `ORDER BY`。

> 注意：SELECT 里定义的列别名，不能在 WHERE 中直接使用。

除了 `SELECT`，`UPDATE` 和 `DELETE` 语句同样可以使用 `WHERE`，用来指定需要更新或删除的行。

WHERE 的条件可以使用下面的比较运算符和逻辑运算符：

| 运算符 | 说明 |
|---|---|
| = | 等于 |
| > | 大于 |
| < | 小于 |
| >= | 大于等于 |
| <= | 小于等于 |
| <> 或 != | 不等于 |
| AND | 逻辑与 |
| OR | 逻辑或 |
| IN | 值匹配列表中任意一项则返回真 |
| BETWEEN | 值落在区间范围内返回真 |
| LIKE | 匹配字符串模式 |
| IS NULL | 判断值是否为 NULL |
| NOT | 取反其他运算结果 |

## PostgreSQL WHERE 子句示例
下面基于示例库的 `customer` 表演示。

### 1) 等于运算符 =
查询名字为 `Jamie` 的客户：
```sql
SELECT
  last_name,
  first_name
FROM
  customer
WHERE
  first_name = 'Jamie';
```

输出：
```
last_name | first_name
-----------+------------
 Rice      | Jamie
 Waugh     | Jamie
(2 行)
```

### 2) AND 逻辑与
查找名字是 Jamie，姓氏是 Rice 的客户：
```sql
SELECT
  last_name,
  first_name
FROM
  customer
WHERE
  first_name = 'Jamie'
  AND last_name = 'Rice';
```

输出：
```
last_name | first_name
-----------+------------
 Rice      | Jamie
(1 行)
```

### 3) OR 逻辑或
查找姓氏为 Rodriguez 或者名字为 Adam 的客户：
```sql
SELECT first_name, last_name FROM customer
WHERE
  last_name = 'Rodriguez' OR first_name = 'Adam';
```

输出：
```
first_name | last_name
------------+-----------
 Laura      | Rodriguez
 Adam       | Gooch
(2 行)
```

### 4) IN 列表匹配
查询名字为 Ann、Anne、Annie 的客户：
```sql
SELECT first_name, last_name FROM customer WHERE first_name IN ('Ann', 'Anne', 'Annie');
```

输出：
```
first_name | last_name
------------+-----------
 Ann        | Evans
 Anne       | Powell
 Annie      | Russell
(3 行)
```

### 5) LIKE 模糊匹配
查询名字以 Ann 开头的客户：
```sql
SELECT first_name, last_name FROM customer WHERE first_name LIKE 'Ann%';
```

输出：
```
first_name | last_name
------------+-----------
 Anna       | Hill
 Ann        | Evans
 Anne       | Powell
 Annie      | Russell
 Annette    | Olson
(5 行)
```
`%` 是通配符，可以匹配任意字符串，`'Ann%'` 匹配所有以 Ann 开头的字符串。

### 6) BETWEEN 区间范围
查找名字以 A 开头，名字长度 3‑5 个字符的客户：
```sql
SELECT first_name, LENGTH(first_name) name_length
FROM customer
WHERE first_name LIKE 'A%'
  AND LENGTH(first_name) BETWEEN 3 AND 5
ORDER BY name_length;
```

输出：
```
first_name | name_length
------------+-------------
 Amy        | 3
 Ann        | 3
 Ana        | 3
 Andy       | 4
 Anna       | 4
 Anne       | 4
 Alma       | 4
 Adam       | 4
 Alan       | 4
 Alex       | 4
 Angel      | 5
...
```
`LENGTH()` 函数用来获取字符串字符数量。

### 7) 不等于运算符 <>
查找名字以 Bra 开头，且姓氏不等于 Motley 的客户：
```sql
SELECT first_name, last_name
FROM customer
WHERE first_name LIKE 'Bra%'
  AND last_name <> 'Motley';
```

输出：
```
first_name | last_name
------------+-----------
 Brandy     | Graves
 Brandon    | Huey
 Brad       | Mccurdy
(3 行)
```
> `<>` 和 `!=` 效果完全等价，可以互换使用。

## 小结
- 在 SELECT 中使用 `WHERE`，可以根据一个或多个条件过滤查询行。
