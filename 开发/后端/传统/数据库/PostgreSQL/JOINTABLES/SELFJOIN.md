# PostgreSQL Self-Join

**摘要**：本教程将学习 PostgreSQL 自连接用法，实现同一张表内多行数据对比查询。

## PostgreSQL 自连接简介
自连接就是普通连接，只是将一张表和它自身做关联。实际开发中，自连接常用来查询层级结构数据，或是对同一张表内部多行数据做对比。

实现自连接的核心：**同一张表书写两次，并分配不同的表别名**，再在 ON 后书写连接条件。

下面使用内连接实现表自关联的语法：
```sql
SELECT select_list 
FROM table_name t1 
INNER JOIN table_name t2 
ON join_predicate;
```
该语法中，使用 `INNER JOIN` 让一张表自身完成关联。

也可以改用左连接、右连接实现自关联，示例如下：
```sql
SELECT select_list 
FROM table_name t1 
LEFT JOIN table_name t2 
ON join_predicate;
```

## PostgreSQL 自连接实战示例
下面通过两组案例演示自连接用法。

### 1）查询层级结构数据
先创建演示用的员工表，模拟组织上下级层级关系。

建表与插入测试数据语句：
```sql
CREATE TABLE employee (
  employee_id INT PRIMARY KEY,
  first_name VARCHAR (255) NOT NULL,
  last_name VARCHAR (255) NOT NULL,
  manager_id INT,
  FOREIGN KEY (manager_id) REFERENCES employee (employee_id) ON DELETE CASCADE
);

INSERT INTO employee (employee_id, first_name, last_name, manager_id) VALUES
  (1, 'Windy', 'Hays', NULL),
  (2, 'Ava', 'Christensen', 1),
  (3, 'Hassan', 'Conner', 1),
  (4, 'Anna', 'Reeves', 2),
  (5, 'Sau', 'Norman', 2),
  (6, 'Kelsie', 'Hays', 3),
  (7, 'Tory', 'Goff', 3),
  (8, 'Salley', 'Lester', 3);

SELECT * FROM employee;
```
查询结果：
```
employee_id | first_name | last_name   | manager_id
-------------+------------+-------------+------------
  1          | Windy      | Hays        | null
  2          | Ava        | Christensen | 1
  3          | Hassan     | Conner      | 1
  4          | Anna       | Reeves      | 2
  5          | Sau        | Norman      | 2
  6          | Kelsie     | Hays        | 3
  7          | Tory       | Goff        | 3
  8          | Salley     | Lester      | 3
(8 行记录)
```

在 `employee` 员工表中，`manager_id` 字段关联本表的 `employee_id`。
`manager_id` 代表员工的直属上级ID；
如果 `manager_id` 为 NULL，代表该员工没有上级，是最高管理者。

下面使用自连接，查询每位员工对应的直属上级：
```sql
SELECT
  e.first_name || ' ' || e.last_name employee,
  m.first_name || ' ' || m.last_name manager
FROM
  employee e
INNER JOIN employee m
ON m.employee_id = e.manager_id
ORDER BY manager;
```
输出结果：
```
employee          | manager
------------------+------------------
Sau Norman        | Ava Christensen
Anna Reeves       | Ava Christensen
Salley Lester     | Hassan Conner
Kelsie Hays       | Hassan Conner
Tory Goff         | Hassan Conner
Ava Christensen   | Windy Hays
Hassan Conner     | Windy Hays
(7 行记录)
```

这条查询将员工表引用两次：别名 e 代表普通员工，别名 m 代表上级管理者。
连接条件通过匹配 `employee_id` 和 `manager_id` 绑定员工与上级关系。

可以发现：最高管理者 Windy Hays 没有出现在结果中。

如果需要把最高管理者也展示在结果里，将 `INNER JOIN` 替换为 `LEFT JOIN`：
```sql
SELECT
  e.first_name || ' ' || e.last_name employee,
  m.first_name || ' ' || m.last_name manager
FROM
  employee e
LEFT JOIN employee m
ON m.employee_id = e.manager_id
ORDER BY manager;
```
输出结果：
```
employee          | manager
------------------+------------------
Anna Reeves       | Ava Christensen
Sau Norman        | Ava Christensen
Salley Lester     | Hassan Conner
Kelsie Hays       | Hassan Conner
Tory Goff         | Hassan Conner
Hassan Conner     | Windy Hays
Ava Christensen   | Windy Hays
Windy Hays        | null
(8 行记录)
```

### 2）同表多行数据对比
以DVD租赁库中的 film 影片表举例。
需求：找出所有片长相同的影片配对。
```sql
SELECT
  f1.title,
  f2.title,
  f1.length
FROM
  film f1
INNER JOIN film f2
ON f1.film_id > f2.film_id
AND f1.length = f2.length;
```
输出结果：
```
title               | title               | length
--------------------+---------------------+--------
Chamber Italian     | Affair Prejudice    | 117
Grosse Wonderful    | Doors President     | 49
Bright Encounters   | Bedazzled Married   | 73
Date Speed          | Crow Grease         | 104
Annie Identity      | Academy Dinosaur    | 86
Anything Savannah   | Alone Trip          | 82
Apache Divine       | Anaconda Confessions| 92
Arabia Dogma        | Airplane Sierra     | 62
Dying Maker         | Antitrust Tomatoes  | 168
...
```

连接条件说明：
1. `f1.film_id > f2.film_id`：保证两组影片不重复配对、不出现自身和自己匹配；
2. `f1.length = f2.length`：筛选片长完全一致的影片。

## 小结
1. PostgreSQL 自连接是普通连接的特殊用法，使用 INNER JOIN / LEFT JOIN 让一张表关联自身；
2. 自连接适合两种场景：查询层级树形数据、同一张表内多行数据互相对比。

