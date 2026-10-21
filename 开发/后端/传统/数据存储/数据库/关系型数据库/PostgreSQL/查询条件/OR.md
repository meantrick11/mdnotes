# PostgreSQL OR 逻辑运算符
**要点**：本教程讲解 PostgreSQL `OR` 逻辑运算符，学习如何使用它组合多个布尔条件表达式。

## PostgreSQL OR 运算符简介
PostgreSQL 的布尔类型存在三种取值：`true`、`false`、`null`。

代表真值的写法：`true`、`'t'`、`'true'`、`'y'`、`'yes'`、`'1'`
代表假值的写法：`false`、`'f'`、`'false'`、`'n'`、`'no'`、`'0'`

布尔表达式指运算结果为布尔值的语句，例如 `1<>1` 的运算结果为 false：
```sql
SELECT 1 <> 1 AS result;
```
输出：
```
result
--------
f
(1 行)
```
输出中的 `f` 代表 false（假）。

`OR` 是逻辑运算符，用于拼接多个布尔表达式，基础语法：
```
表达式1 OR 表达式2
```
表达式1、表达式2 的运算结果只能是 true / false / null。

OR 运算规则：
1. 任意一个表达式为 true，结果就为 true；
2. 两个表达式全为 false，结果才是 false；
3. 其余场景结果为 null。

真值表（表达式先后顺序不影响运算结果，例如 `false OR null` 和 `null OR false` 结果相同）：

| 表达式1 | 表达式2 | 表达式1 OR 表达式2 |
|---|---|---|
| True | True | True |
| True | False | True |
| True | Null | True |
| False | False | False |
| False | Null | Null |
| Null | Null | Null |

实际开发场景中，`OR` 常配合 `WHERE` 子句使用，只要任意一个条件成立，该行数据就会被查询出来。

## PostgreSQL OR 运算符示例
### 1）基础 OR 运算示例
真值或真值，返回 true：
```sql
SELECT true OR true AS result;
```
输出：
```
result
--------
t
(1 行)
```

真值或假值，返回 true：
```sql
SELECT true OR false AS result;
```
输出：
```
result
--------
t
(1 行)
```

真值或 null，返回 true：
```sql
SELECT true OR null AS result;
```
输出：
```
result
--------
t
(1 行)
```

假值或假值，返回 false：
```sql
SELECT false OR false AS result;
```
输出：
```
result
--------
f
(1 行)
```

假值或 null，返回 null：
```sql
SELECT false OR null AS result;
```
输出：
```
result
--------
null
(1 行)
```

null 或 null，返回 null：
```sql
SELECT null OR null AS result;
```
输出：
```
result
--------
null
(1 行)
```

### 2）在 WHERE 子句中使用 OR
示例基于示例库中的 `film` 表演示。
需求：查询租赁费率为 0.99 或 2.99 的影片。
```sql
SELECT
  title,
  rental_rate
FROM
  film
WHERE
  rental_rate = 0.99
  OR rental_rate = 2.99;
```
输出：
```
title               | rental_rate
--------------------+-------------
Academy Dinosaur    | 0.99
Adaptation Holes    | 2.99
Affair Prejudice    | 2.99
African Egg         | 2.99
...
```

## 小结
- 使用 `OR` 运算符可以组合多个布尔条件表达式。

