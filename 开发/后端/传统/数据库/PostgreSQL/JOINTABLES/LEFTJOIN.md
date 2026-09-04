# PostgreSQL LEFT JOIN

**摘要**：本教程将讲解如何使用 PostgreSQL 的 `LEFT JOIN` 子句从多张表中查询数据。

## PostgreSQL LEFT JOIN 子句简介
`LEFT JOIN` 会将**左表**与**右表**进行关联，返回左表的所有行，无论这些行在右表中是否存在匹配数据。

当你需要查询一张表里、在另一张表无对应匹配的数据时，LEFT JOIN 非常适用。

LEFT JOIN 基础语法：
```sql
SELECT select_list
FROM
 table1
LEFT JOIN table2  
ON table1.column_name = table2.column_name;
```
语法说明：
1. 在 SELECT 的查询字段列表（select_list）中，指定两张表需要查询的字段。
2. 在 FROM 子句中指定**左表 table1**，作为数据查询基准表。
3. 使用 LEFT JOIN 关键字指定需要关联的**右表 table2**。
4. 定义连接条件 `table1.column_name = table2.column_name`，该条件规定两张表依靠哪一列的值进行匹配。

### LEFT JOIN 运行原理
LEFT JOIN 从左表（table1）开始读取数据。遍历左表每一行时，会将该行关联列的值，与右表每一行对应列的值逐一对比。

1. 若两边列值相等：生成一条包含查询字段的新记录，加入结果集；
2. 若无匹配值：仍保留左表该行，**右表所有查询字段填充为 NULL**。

补充说明：`LEFT JOIN` 等价于 `LEFT OUTER JOIN`，二者可互换使用。

若两张表用于关联的列名称完全相同，可简写使用 `USING` 语法：
```sql
SELECT select_list 
FROM  table1  
LEFT JOIN table2 
USING (column_name);
```
文中附带维恩图，直观展示 LEFT JOIN 的数据匹配逻辑。

## PostgreSQL LEFT JOIN 使用示例
以示例库中的 `film`（影片表）和 `inventory`（库存表）举例：
- 一条影片记录，可能对应 0 条或多条库存记录；
- 每一条库存记录，必然对应唯一一条影片记录；
- 两张表通过 `film_id` 字段建立关联关系。

### 1）LEFT JOIN 基础示例
以下语句使用 LEFT JOIN 关联影片表与库存表：
```sql
SELECT  
 film.film_id,  
 film.title,  
 inventory.inventory_id 
FROM  film  
LEFT JOIN inventory 
ON inventory.film_id = film.film_id 
ORDER BY  film.title;
```
逻辑说明：若某部影片在库存表中无匹配记录，该行的 `inventory_id` 字段会显示为 `NULL`。

使用表别名简化 SQL：
```sql
SELECT  
 f.film_id,
 f.title,  
 i.inventory_id 
FROM  film f  
LEFT JOIN inventory i 
ON i.film_id = f.film_id 
ORDER BY  i.inventory_id;
```
两张表关联字段都叫 film_id，使用 USING 简写：
```sql
SELECT  f.film_id,  f.title,  i.inventory_id 
FROM  film f  
LEFT JOIN inventory i 
USING (film_id) 
ORDER BY  i.inventory_id;
```

### 2）LEFT JOIN 搭配 WHERE 条件筛选
下面示例关联两张表，并通过 WHERE 筛选**无库存的影片**：
```sql
SELECT  f.film_id,  f.title,  i.inventory_id 
FROM  film f  
LEFT JOIN inventory i 
USING (film_id) 
WHERE  i.inventory_id IS NULL 
ORDER BY  f.title;
```
输出结果：
```
film_id | title                  | inventory_id 
---------+------------------------+--------------
  14     | Alice Fantasia         | null
  33     | Apollo Teen            | null
  36     | Argonauts Town         | null
  38     | Ark Ridgemont          | null
  41     | Arsenic Independence   | null
 ...
```

## 小结
- 使用 PostgreSQL `LEFT JOIN` 可查询左表全部数据，包含在右表存在匹配、或无匹配的所有行。

