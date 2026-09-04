# PostgreSQL ORDER BY

**概述**：本教程讲解 `ORDER BY` 子句，实现按单个/多个条件对查询结果行排序。

## 一、ORDER BY 子句基础介绍
直接执行 `SELECT` 返回的数据行顺序是不确定的，想要自定义排序，必须搭配 `ORDER BY`。
`ORDER BY` 可根据排序表达式，将结果升序或降序排列。

### 语法格式
```sql
SELECT
    字段列表
FROM
    表名
ORDER BY
    排序表达式1 [ASC | DESC],
    排序表达式2 [ASC | DESC],
    ...;
```
语法说明：
1. 排序表达式：可以是字段、计算表达式；多条件排序用逗号分隔，**先按前面的条件排，相同再按后面条件排**。
2. `ASC`：升序；`DESC`：降序；**不写则默认 ASC 升序**。

### SQL 执行顺序
PostgreSQL 执行顺序：`FROM` → `SELECT` → `ORDER BY`
正因先执行 SELECT，所以 SELECT 中定义的**列别名**可以直接在 ORDER BY 里使用。

## 二、ORDER BY 实操示例
示例基于 dvdrental 示例库 `customer` 客户表。

### 1）单字段升序排序
按名字 first_name 升序，ASC 可省略：
```sql
SELECT first_name, last_name
FROM customer
ORDER BY first_name ASC;
-- 简写（默认升序）
SELECT first_name, last_name
FROM customer
ORDER BY first_name;
```
结果：
```sql
dvdrental=# SELECT
dvdrental-#   first_name,
dvdrental-#   last_name
dvdrental-# FROM
dvdrental-#   customer
dvdrental-# ORDER BY
dvdrental-#   first_name ASC;
 first_name  |  last_name
-------------+--------------
 Aaron       | Selby
 Adam        | Gooch
 Adrian      | Clary
 Agnes       | Bishop
 Alan        | Kahn
 Albert      | Crouse
 Alberto     | Henning
 Alex        | Gresham
 Alexander   | Fennell
 Alfred      | Casillas
 Alfredo     | Mcadams
 Alice       | Stewart
 Alicia      | Mills
 Allan       | Cornish
 Allen       | Butterfield
 Allison     | Stanley
 Alma        | Austin
 Alvin       | Deloach
 Amanda      | Carter
 Amber       | Dixon
 Amy         | Lopez
 Ana         | Bradley
 Andre       | Rapp
 Andrea      | Henderson
 Andrew      | Purdy
 ...
 ```

### 2）单字段降序排序
按姓氏 last_name 倒序：
```sql
SELECT first_name, last_name
FROM customer
ORDER BY last_name DESC;
```
结果：
```sql
dvdrental=# SELECT
dvdrental-#   first_name,
dvdrental-#   last_name
dvdrental-# FROM
dvdrental-#   customer
dvdrental-# ORDER BY
dvdrental-#   last_name DESC;
 first_name  |  last_name
-------------+--------------
 Cynthia     | Young
 Marvin      | Yee
 Luis        | Yanez
 Brian       | Wyman
 Brenda      | Wright
 Tyler       | Wren
 Florence    | Woods
 Lori        | Wood
 Virgil      | Wofford
 Darren      | Windham
 Susan       | Wilson
 Bernice     | Willis
 Gina        | Williamson
 Linda       | Williams
 Jon         | Wiles
 Roy         | Whiting
 Betty       | White
 Lucy        | Wheeler
 Fred        | Wheat
 Mitchell    | Westmoreland
 Edna        | West
 Sheila      | Wells
 Marlene     | Welch
 Ronald      | Weiner
 Ethel       | Webb
 ...
 ```

### 3）多字段组合排序
先按名字升序，名字相同再按姓氏降序：
```sql
SELECT first_name, last_name
FROM customer
ORDER BY first_name ASC, last_name DESC;
```
结果：
```sql
dvdrental=# SELECT
dvdrental-#   first_name,
dvdrental-#   last_name
dvdrental-# FROM
dvdrental-#   customer
dvdrental-# ORDER BY
dvdrental-#   last_name DESC;
 first_name  |  last_name
-------------+--------------
 Cynthia     | Young
 Marvin      | Yee
 Luis        | Yanez
 Brian       | Wyman
 Brenda      | Wright
 Tyler       | Wren
 Florence    | Woods
 Lori        | Wood
 Virgil      | Wofford
 Darren      | Windham
 Susan       | Wilson
 Bernice     | Willis
 Gina        | Williamson
 Linda       | Williams
 Jon         | Wiles
 Roy         | Whiting
 Betty       | White
 Lucy        | Wheeler
 Fred        | Wheat
 Mitchell    | Westmoreland
 Edna        | West
 Sheila      | Wells
 Marlene     | Welch
 Ronald      | Weiner
 Ethel       | Webb
dvdrental=# SELECT
dvdrental-#   first_name,
dvdrental-#   last_name
dvdrental-# FROM
dvdrental-#   customer
dvdrental-# ORDER BY
dvdrental-#   first_name ASC,
dvdrental-#   last_name DESC;
 first_name  |  last_name
-------------+--------------
 Aaron       | Selby
 Adam        | Gooch
 Adrian      | Clary
 Agnes       | Bishop
 Alan        | Kahn
 Albert      | Crouse
 Alberto     | Henning
 Alex        | Gresham
 Alexander   | Fennell
 Alfred      | Casillas
 Alfredo     | Mcadams
 Alice       | Stewart
 Alicia      | Mills
 Allan       | Cornish
 Allen       | Butterfield
 Allison     | Stanley
 Alma        | Austin
 Alvin       | Deloach
 Amanda      | Carter
 Amber       | Dixon
 Amy         | Lopez
 Ana         | Bradley
 Andre       | Rapp
 Andrea      | Henderson
 Andrew      | Purdy
 ...
 ```

### 4）按表达式/列别名排序
`LENGTH(字段)` 获取字符串长度；可直接使用SELECT中定义的别名排序：
```sql
SELECT first_name, LENGTH(first_name) len
FROM customer
ORDER BY len DESC;
```
结果：
```sql
dvdrental=# SELECT
dvdrental-#   first_name,
dvdrental-#   LENGTH(first_name) len
dvdrental-# FROM
dvdrental-#   customer
dvdrental-# ORDER BY
dvdrental-#   len DESC;
 first_name  | len
-------------+-----
 Christopher |  11
 Jacqueline  |  10
 Constance   |   9
 Katherine   |   9
 Nathaniel   |   9
 Catherine   |   9
 Christian   |   9
 Christine   |   9
 Charlotte   |   9
 Geraldine   |   9
 Elizabeth   |   9
 Priscilla   |   9
 Josephine   |   9
 Frederick   |   9
 Alexander   |   9
 Cassandra   |   9
 Gwendolyn   |   9
 Francisco   |   9
 Stephanie   |   9
 Christina   |   9
 Kristina    |   8
 Lorraine    |   8
 Jennifer    |   8
 Caroline    |   8
 Beatrice    |   8
 ...
 ```

## 三、ORDER BY 与 NULL 值排序规则
*以下知道NULLS FIRST&NULLS LAST什么作用就可以了，实际所用大多都是默认的NULLS LAST.*
NULL 代表数据缺失/未知，可通过 `NULLS FIRST` / `NULLS LAST` 控制 NULL 行的位置：
```sql
ORDER BY 排序表达式 [ASC | DESC] [NULLS FIRST | NULLS LAST]
```
- `NULLS FIRST`：NULL 值排在所有非空数据前面
- `NULLS LAST`：NULL 值排在所有非空数据后面

### 演示测试表
```sql
-- 创建测试表
CREATE TABLE sort_demo(num INT);
-- 插入测试数据
INSERT INTO sort_demo(num) VALUES (1), (2), (3), (null);
```
psql 执行 `\pset null null` 可把空白NULL显示为字符串null，方便查看。

### 1）默认升序 ASC
默认 `NULLS LAST`，NULL放末尾
```sql
SELECT num FROM sort_demo ORDER BY num;
-- 等价写法
SELECT num FROM sort_demo ORDER BY num NULLS LAST;
```
结果：1, 2, 3, null

### 2）升序强制 NULL 放最前
```sql
SELECT num FROM sort_demo ORDER BY num NULLS FIRST;
```
结果：null, 1, 2, 3

### 3）默认降序 DESC
默认 `NULLS FIRST`，NULL放最前
```sql
SELECT num FROM sort_demo ORDER BY num DESC;
```
结果：null, 3, 2, 1

### 4）降序强制 NULL 放末尾
```sql
SELECT num FROM sort_demo ORDER BY num DESC NULLS LAST;
```
结果：3, 2, 1, null

## 四、核心小结
1. `ORDER BY` 用于自定义 SELECT 查询结果的行顺序；
2. `ASC` 升序、`DESC` 降序，默认升序；支持多字段组合排序；
3. 执行顺序 FROM → SELECT → ORDER BY，ORDER BY 可直接使用查询内的列别名；
4. 通过 `NULLS FIRST` / `NULLS LAST` 手动控制 NULL 数据的排序位置；
5. ASC 默认 NULLS LAST，DESC 默认 NULLS FIRST。

