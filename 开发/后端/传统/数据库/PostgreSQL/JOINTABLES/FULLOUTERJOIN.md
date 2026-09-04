
# PostgreSQL FULL OUTER JOIN

**摘要**：本教程将学习如何使用 PostgreSQL `FULL OUTER JOIN` 查询两张表的数据。

## PostgreSQL FULL OUTER JOIN 子句简介
`FULL OUTER JOIN` 合并两张表的数据，返回两张表的**所有行**，包含两边匹配成功的记录，以及两边各自无匹配的记录。

简单来说：`FULL OUTER JOIN` 等价于合并左连接与右连接的查询结果。

FULL OUTER JOIN 基础语法：
```sql
SELECT select_list 
FROM table1 
FULL OUTER JOIN table2  
ON table1.column_name = table2.column_name;
```
语法说明：
1. 在 `select_list` 中指定需要从 table1、table2 两张表取出的字段。
2. 在 `FROM` 子句中填写第一张表 table1。
3. 通过 `FULL OUTER JOIN` 关键字指定要关联的第二张表 table2。
4. 定义两表之间的匹配连接条件。

补充：`FULL OUTER JOIN` 可简写为 `FULL JOIN`，关键字 `OUTER` 可以省略。

### FULL OUTER JOIN 运行原理
**步骤1：初始化结果集**
FULL OUTER JOIN 从空的结果集开始。

**步骤2：匹配两边共有数据**
1. 找出 table1 和 table2 中关联列值相等的行；
2. 将这些匹配成功的行存入结果集。

**步骤3：加入两边无匹配的独有数据**
1. 先加入 table1 中在 table2 找不到匹配的行，该行来自 table2 的所有字段填充 NULL；
2. 再加入 table2 中在 table1 找不到匹配的行，该行来自 table1 的所有字段填充 NULL。

**步骤4：返回最终结果集**
最终结果包含两张表全部数据：
- 两边能匹配上的行，合并为一条完整记录；
- 单边无匹配的行，缺失一侧的字段全部填充 NULL。

文中附带维恩图直观展示全外连接逻辑。

## 准备演示示例表
先创建两张演示表：`departments` 部门表、`employees` 员工表
```sql
CREATE TABLE departments (
  department_id serial PRIMARY KEY,
  department_name VARCHAR (255) NOT NULL
);

CREATE TABLE employees (
  employee_id serial PRIMARY KEY,
  employee_name VARCHAR (255),
  department_id INTEGER
);
```
业务关系：一个部门可以有0名或多名员工；一名员工可以不属于任何部门，或仅归属一个部门。

向两张表插入测试数据：
```sql
INSERT INTO departments (department_name) VALUES
  ('Sales'),
  ('Marketing'),
  ('HR'),
  ('IT'),
  ('Production');

INSERT INTO employees (employee_name, department_id) VALUES
  ('Bette Nicholson', 1),
  ('Christian Gable', 1),
  ('Joe Swank', 2),
  ('Fred Costner', 3),
  ('Sandra Kilmer', 4),
  ('Julia Mcqueen', NULL);
```

查询部门表全部数据：
```sql
SELECT * FROM departments;
```
输出：
```
department_id | department_name
---------------+-----------------
  1            | Sales
  2            | Marketing
  3            | HR
  4            | IT
  5            | Production
(5 行记录)
```

查询员工表全部数据：
```sql
SELECT * FROM employees;
```
输出：
```
employee_id | employee_name   | department_id
-------------+-----------------+---------------
  1          | Bette Nicholson | 1
  2          | Christian Gable | 1
  3          | Joe Swank       | 2
  4          | Fred Costner    | 3
  5          | Sandra Kilmer   | 4
  6          | Julia Mcqueen   | null
(6 行记录)
```

## PostgreSQL FULL OUTER JOIN 使用示例
### 1）FULL OUTER JOIN 基础示例
下方语句使用全外连接关联员工表与部门表：
```sql
SELECT
  employee_name,
  department_name
FROM
  employees e
FULL OUTER JOIN departments d
  ON d.department_id = e.department_id;
```
输出：
```
employee_name   | department_name
-----------------+-----------------
Bette Nicholson  | Sales
Christian Gable  | Sales
Joe Swank        | Marketing
Fred Costner     | HR
Sandra Kilmer    | IT
Julia Mcqueen    | null
null             | Production
(7 行记录)
```

结果包含以下全部数据：
1. 所有有归属部门的员工、所有存在员工的部门；
2. 无归属部门的员工；
3. 没有任何员工的部门。

### 2）FULL OUTER JOIN 搭配 WHERE 过滤
示例1：筛选**没有任何员工**的部门
```sql
SELECT
  employee_name,
  department_name
FROM
  employees e
FULL OUTER JOIN departments d
  ON d.department_id = e.department_id
WHERE
  employee_name IS NULL;
```
输出：
```
employee_name | department_name
---------------+-----------------
null          | Production
(1 行记录)
```
结果说明：Production（生产部）没有任何员工。

示例2：筛选**不属于任何部门**的员工
```sql
SELECT
  employee_name,
  department_name
FROM
  employees e
FULL OUTER JOIN departments d
  ON d.department_id = e.department_id
WHERE
  department_name IS NULL;
```
输出：
```
employee_name | department_name
---------------+-----------------
Julia Mcqueen | null
(1 行记录)
```
结果说明：Julia Mcqueen 未归属任何部门。

## 小结
使用 PostgreSQL `FULL OUTER JOIN` 合并两张表数据，结果会包含：
1. 左右两表互相匹配成功的记录；
2. 左表独有、右表无匹配的记录；
3. 右表独有、左表无匹配的记录。

