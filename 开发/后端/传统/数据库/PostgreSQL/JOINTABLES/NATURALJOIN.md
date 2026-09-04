# PostgreSQL NATURAL JOIN

**摘要**：本教程将学习使用 PostgreSQL `NATURAL JOIN` 从两张表中查询数据。

## PostgreSQL NATURAL JOIN 子句简介
自然连接是一种特殊连接，它依靠两张表中**同名的列**自动生成隐式匹配条件。

NATURAL JOIN 通用语法：
```sql
SELECT select_list 
FROM table1 NATURAL [INNER, LEFT, RIGHT] JOIN table2;
```
语法说明：
1. 在 SELECT 的查询字段列表 `select_list` 中，指定需要从两张表读取的字段。
2. FROM 子句填写主表 table1。
3. 通过 NATURAL JOIN 关键字指定需要关联的第二张表 table2。

**弊端：诞生的目的就是为了省去ON/USING，使用范围为唯一一组业务关联字段同名的时候，而可以不用使用ON 或者USING来显式指定，绝大多数业务中，连接查询使用INNER JOIN&LEFT JOIN&FULL OUTER JOIN就可以了，RIGHT JOIN都很少使用，因为可以被LEFT JOIN等效替代。**

自然连接可以搭配内连接、左连接、右连接；如果不显式指定连接类型，PostgreSQL 默认使用 `INNER JOIN`。

`NATURAL JOIN` 的优势是无需手动书写 ON 匹配条件，它会自动将两张表所有同名字段做等值匹配。

它等价于下面显式写匹配条件的普通连接：
```sql
SELECT select_list 
FROM table1 [INNER, LEFT, RIGHT] JOIN table2  
ON table1.column_name = table2.column_name;
```

### 自然内连接 NATURAL INNER JOIN
下面两条语句完全等价：
```sql
SELECT select_list FROM table1 NATURAL INNER JOIN table2;
```
等价于
```sql
SELECT select_list FROM table1 INNER JOIN table2 USING (column_name);
```

### 自然左连接 NATURAL LEFT JOIN
两条语句等价：
```sql
SELECT select_list FROM table1 NATURAL LEFT JOIN table2;
```
等价于
```sql
SELECT select_list FROM table1 LEFT JOIN table2 USING (column_name);
```

### 自然右连接 NATURAL RIGHT JOIN
两条语句等价：
```sql
SELECT select_list FROM table1 NATURAL RIGHT JOIN table2;
```
等价于
```sql
SELECT select_list FROM table1 RIGHT JOIN table2 USING (column_name);
```

## 搭建演示示例表
创建分类表 categories、商品表 products，并插入测试数据用于演示：
```sql
CREATE TABLE categories (
  category_id SERIAL PRIMARY KEY,
  category_name VARCHAR (255) NOT NULL
);

CREATE TABLE products (
  product_id serial PRIMARY KEY,
  product_name VARCHAR (255) NOT NULL,
  category_id INT NOT NULL,
  FOREIGN KEY (category_id) REFERENCES categories (category_id)
);

INSERT INTO categories (category_name) VALUES
  ('Smartphone'),
  ('Laptop'),
  ('Tablet'),
  ('VR') RETURNING *;

INSERT INTO products (product_name, category_id) VALUES
  ('iPhone', 1),
  ('Samsung Galaxy', 1),
  ('HP Elite', 2),
  ('Lenovo Thinkpad', 2),
  ('iPad', 3),
  ('Kindle Fire', 3) RETURNING *;
```

商品表 products 数据：
```
product_id | product_name    | category_id
------------+-----------------+-------------
  1         | iPhone          | 1
  2         | Samsung Galaxy  | 1
  3         | HP Elite       | 2
  4         | Lenovo Thinkpad | 2
  5         | iPad            | 3
  6         | Kindle Fire     | 3
(6 行记录)
```

分类表 categories 数据：
```
category_id | category_name
-------------+---------------
  1          | Smartphone
  2          | Laptop
  3          | Tablet
  4          | VR
(4 行记录)
```

## PostgreSQL NATURAL JOIN 使用示例
### 1）基础 NATURAL INNER JOIN 示例
使用自然连接关联商品表与分类表：
```sql
SELECT * FROM products NATURAL JOIN categories;
```
该语句会自动以 `category_id` 作为匹配列执行内连接。

输出结果：
```
category_id | product_id | product_name    | category_name
-------------+------------+-----------------+---------------
  1          | 1          | iPhone          | Smartphone
  1          | 2          | Samsung Galaxy  | Smartphone
  2          | 3          | HP Elite        | Laptop
  2          | 4          | Lenovo Thinkpad | Laptop
  3          | 5          | iPad            | Tablet
  3          | 6          | Kindle Fire     | Tablet
(6 行记录)
```

这条自然连接等价于下面使用 USING 的普通内连接：
```sql
SELECT	* FROM products INNER JOIN categories USING (category_id);
```

### 2）NATURAL LEFT JOIN 自然左连接示例
无需手动指定匹配字段，直接使用 NATURAL LEFT JOIN：
```sql
SELECT * FROM categories NATURAL LEFT JOIN products;
```
输出结果：
```
category_id | category_name | product_id | product_name
-------------+---------------+------------+-----------------
  1          | Smartphone    | 1          | iPhone
  1          | Smartphone    | 2          | Samsung Galaxy
  2          | Laptop        | 3          | HP Elite
  2          | Laptop        | 4          | Lenovo Thinkpad
  3          | Tablet        | 5          | iPad
  3          | Tablet        | 6          | Kindle Fire
  4          | VR            | null       | null
(7 行记录)
```

### 3）NATURAL JOIN 产生意外结果的坑点
实际开发中**尽量避免使用 NATURAL JOIN**，它很容易产生不符合预期的查询结果。

以示例库中的 city 城市表、country 国家表举例：
两张表除了 `country_id` 之外，还有一列同名字段 `last_update`。
当执行如下自然连接：
```sql
SELECT * FROM city NATURAL JOIN country;
```
查询会返回空结果集。

原因：NATURAL JOIN 会把**所有同名字段**同时作为匹配条件，两张表的 `last_update` 时间值无法匹配，最终没有符合条件的数据。

## 小结
使用 PostgreSQL `NATURAL JOIN` 关联多张存在同名字段的表，它会自动以所有同名列做等值匹配。

