# PostgreSQL IS NULL 判空运算符
**要点**：本教程学习 `IS NULL` / `IS NOT NULL`，用于判断字段值是否为空（NULL）。

## NULL 基础概念
数据库中 `NULL` 代表**数据缺失/无适用值**，它本身不是一个具体数值：
1. 不能使用 `=`、`<>` 去对比 NULL；
2. NULL 和任意值对比，结果永远为 NULL；
3. NULL 不等于自身。

示例验证 NULL 不能相等判断：
```sql
SELECT null = null AS result;
```
输出：
```
result
--------
null
(1 行)
```

## IS NULL / IS NOT NULL 运算符
想要判断字段是否为空，不能用等于/不等于，必须使用专用判空运算符：

### IS NULL
语法：
```sql
字段 IS NULL
```
规则：字段值为 NULL 时返回 true，否则 false。

### IS NOT NULL（取反）
语法：
```sql
字段 IS NOT NULL
```
规则：字段不为 NULL 时返回 true，否则 false。

补充知识点：
1. 排序中 NULL 的处理方式可查看 ORDER BY 教程；
2. PostgreSQL 配套处理空值函数：`NULLIF`、`ISNULL`、`COALESCE`；
3. 建表约束 `NOT NULL` 可限制字段不允许存储 NULL。

## PostgreSQL IS NULL 代码示例
基于示例库 `address` 表演示。
补充 psql 小技巧：默认 NULL 显示为空行，执行 `\pset null null` 可让终端直接展示 `null` 字样。

### 1）IS NULL 查询 address2 为空的数据
查询 address2 字段值为 NULL 的地址：
```sql
SELECT
  address,
  address2
FROM
  address
WHERE
  address2 IS NULL;
```
输出：
```
address               | address2
----------------------+----------
47 MySakila Drive     | null
28 MySQL Boulevard    | null
23 Workhaven Lane     | null
1411 Lillydale Drive  | null
(4 行)
```

### 2）IS NOT NULL 查询非空数据
查询 address2 不为 NULL 的地址记录：
```sql
SELECT
  address,
  address2
FROM
  address
WHERE
  address2 IS NOT NULL;
```
输出：
```
address               | address2
----------------------------------------+----------
1913 Hanoi Way         |
1121 Loja Avenue       |
692 Joliet Street      |
1566 Inegl Manor       |
```
> 注意：此处 address2 显示空白代表**空字符串**，并非 NULL。同一字段混用空字符串和 NULL 属于不规范设计，可通过 UPDATE 将空字符串统一更新为 NULL。

## 小结
1. NULL 表示数据缺失，不能使用 `=`、`<>` 判断空值；
2. `IS NULL`：匹配字段为空的行；
3. `IS NOT NULL`：匹配字段不为空的行。

