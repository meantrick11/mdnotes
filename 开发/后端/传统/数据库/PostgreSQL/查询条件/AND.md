# PostgreSQL AND 逻辑运算符

**要点**：本教程讲解 PostgreSQL `AND` 逻辑运算符，学习如何用它组合多个布尔条件表达式。

## PostgreSQL AND 运算符简介
PostgreSQL 布尔类型有三种取值：`true`、`false`、`null`。

系统中代表真值的写法：`true`、`'t'`、`'true'`、`'y'`、`'yes'`、`'1'`
系统中代表假值的写法：`false`、`'f'`、`'false'`、`'n'`、`'no'`、`'0'`

布尔表达式指运算结果为布尔值的语句，例如 `1=1` 运算结果为 true：
```sql
SELECT 1 = 1 AS result;
```
输出：
```
result
--------
t
(1 行)
```
输出里的 `t` 代表 true（真）。

`AND` 是逻辑运算符，用于拼接两个布尔表达式。
基础语法：
```
表达式1 AND 表达式2
```
`表达式1` 和 `表达式2` 运算结果只能是 true / false / null。

AND 运算规则：
1. 两个表达式全为 true，结果才是 true；
2. 任意一个表达式为 false，结果就是 false；
3. 其余情况结果为 null。

真值表（表达式顺序不影响最终结果，如 `true AND null` 与 `null AND true` 结果一致）：

| 表达式1 | 表达式2 | 表达式1 AND 表达式2 |
|---|---|---|
| True | True | True |
| True | False | False |
| True | Null | Null |
| False | False | False |
| False | Null | False |
| Null | Null | Null |

实际开发中，`AND` 大多配合 `WHERE` 子句使用，只有所有条件同时成立，该行数据才会被查询出来。

## PostgreSQL AND 运算符示例
### 1）基础 AND 运算示例
两个真值相与，返回 true：
```sql
SELECT true AND true AS result;
```
输出：
```
result
--------
t
(1 行)
```

真值与假值相与，返回 false：
```sql
SELECT true AND false AS result;
```
输出：
```
result
--------
f
(1 行)
```

真值与 null 相与，返回 null：
```sql
SELECT true AND null AS result;
```
输出：
```
result
--------
null
(1 行)
```

两个假值相与，返回 false：
```sql
SELECT false AND false AS result;
```
输出：
```
result
--------
f
(1 行)
```

假值与 null 相与，返回 false：
```sql
SELECT false AND null AS result;
```
输出：
```
result
--------
f
(1 行)
```

两个 null 相与，返回 null：
```sql
SELECT null AND null AS result;
```
输出：
```
result
--------
null
(1 行)
```

### 2）在 WHERE 子句中使用 AND
示例使用示例库中的 `film` 表演示。
需求：查询影片时长大于180分钟，且租赁费率低于1的影片。
```sql
SELECT
  title,
  length,
  rental_rate
FROM
  film
WHERE
  length > 180
  AND rental_rate < 1;
```
输出：
```
title                | length | rental_rate
---------------------+--------+-------------
Catch Amistad        | 183    | 0.99
Haunting Pianist     | 181    | 0.99
Intrigue Worst       | 181    | 0.99
Love Suicides        | 181    | 0.99
Runaway Tenenbaums   | 181    | 0.99
Smoochy Control      | 184    | 0.99
Sorority Queen       | 184    | 0.99
Theory Mermaid       | 184    | 0.99
Wild Apollo          | 181    | 0.99
Young Language       | 183    | 0.99
(10 行)
```

## 小结
- 使用 `AND` 运算符可组合多个布尔条件表达式。

