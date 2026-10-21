# PostgreSQL RIGHT JOIN

**摘要**：本教程将学习如何使用 PostgreSQL `RIGHT JOIN` 关联两张表，返回右表的全部数据行，无论该行在左表中是否存在匹配记录。

## PostgreSQL RIGHT JOIN 子句简介
`RIGHT JOIN` 将右表与左表进行关联，返回右表所有行，这些行在左表中可能存在匹配数据，也可能不存在。

如果你需要筛选出**右表中有、左表无对应匹配**的数据，RIGHT JOIN 会非常实用。

RIGHT JOIN 基础语法：
```sql
SELECT
 select_list
FROM
 table1
RIGHT JOIN table2  
ON table1.column_name = table2.column_name;
```
语法拆解说明：
1. 在 SELECT 的查询字段列表 `select_list` 中，指定两张表需要查询的字段。
2. 在 FROM 子句填写**左表 table1**。
3. 通过 `RIGHT JOIN` 关键字指定**右表 table2**，作为关联基准表。
4. 定义两表连接条件 `table1.column_name = table2.column_name`，指明两张表依靠哪一列完成值匹配。

### RIGHT JOIN 的工作原理
RIGHT JOIN 会从**右表（table2）**开始读取数据。

遍历右表每一行时，会拿该行关联列的值，和左表（table1）每一行对应列的值逐一比对：
1. 若两边字段值相等：生成一条包含查询字段的新记录，加入结果集；
2. 若不存在相等匹配值：依然保留右表当前行，**左表所有查询字段填充为 NULL**，并将该行加入结果集。

简单概括：RIGHT JOIN 一定会返回右表全部行，不关心左表是否存在对应匹配数据。

文中附带维恩图展示 RIGHT JOIN 匹配逻辑。

补充：`RIGHT OUTER JOIN` 和 `RIGHT JOIN` 完全等价，关键字 `OUTER` 可省略。

### USING 简写语法
如果两张表用于关联的列名称完全相同，可以使用 USING 简化 ON 条件：
```sql
SELECT select_list 
FROM table1 
RIGHT JOIN table2 
USING (column_name);
```

## PostgreSQL RIGHT JOIN 使用示例
示例依旧采用示例库中的 `film`（影片表）与 `inventory`（库存表）。

### 1）RIGHT JOIN 基础示例
下方示例使用 RIGHT JOIN，查询影片表全部记录（右表），无论影片是否存在库存匹配：
```sql
SELECT  
 film.film_id,
 film.title,  
 inventory.inventory_id
FROM
 inventory 
RIGHT JOIN film  
ON film.film_id = inventory.film_id 
ORDER BY film.title;
```

使用表别名改写上述查询：
```sql
SELECT  
 f.film_id,  
 f.title,  
 i.inventory_id 
FROM
 inventory i 
RIGHT JOIN film f  
ON f.film_id = i.film_id 
ORDER BY f.title;
```

两张表关联字段均为 film_id，使用 USING 简写：
```sql
SELECT  f.film_id,  f.title,  i.inventory_id 
FROM  inventory i 
RIGHT JOIN film f 
USING(film_id) 
ORDER BY f.title;
```

### 2）RIGHT JOIN 搭配 WHERE 条件过滤
下面语句结合 WHERE，筛选出**没有任何库存记录**的影片：
```sql
SELECT  f.film_id,  f.title,  i.inventory_id 
FROM  inventory i 
RIGHT JOIN film f 
USING(film_id) 
WHERE i.inventory_id IS NULL 
ORDER BY f.title;
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
- 使用 PostgreSQL `RIGHT JOIN` 关联左右两张表，固定返回**右表全部行**，无论该行在左表是否存在匹配数据。
- `RIGHT JOIN` 也被称作 `RIGHT OUTER JOIN`，二者完全通用。

