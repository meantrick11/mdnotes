# PostgreSQL 表别名与列别名（Alias）

> 别名（Alias）用于给表或者列设置临时名称，仅在当前这条 SQL 语句内生效。

**摘要**：本教程讲解如何使用 PostgreSQL 的别名，包含表别名（table alias）与列别名（column alias）。

在 PostgreSQL 中，可以使用别名，为表或者查询中的列赋予一个临时名称。别名只在当前 SQL 语句执行期间有效。

## 列别名 Column alias

### 基础语法
```sql
SELECT column_name AS alias_name
FROM table_name;
```

`AS` 关键字可以省略，直接写别名：
```sql
SELECT column_name alias_name
FROM table_name;
```

### 带空格的别名
如果别名包含空格，必须用双引号 `"` 包裹：
```sql
SELECT column_name AS "Alias With Space"
FROM table_name;
```

### 示例
沿用前面教程的 `teams` 表：
```sql
SELECT * FROM teams;
```
输出：
```
 id | team   | city
----+--------+------
  1 | Lions  | Rome
  2 | Owls   | Oslo
  3 | Bears  | Bern
  4 | Sharks | Lima
(4 rows)
```

下面示例给 `team` 列设置别名为 `team_name`，`city` 设置别名为 `city_name`：
```sql
SELECT
  team AS team_name,
  city AS city_name
FROM teams;
```

输出：
```
 team_name | city_name
-----------+-----------
 Lions     | Rome
 Owls      | Oslo
 Bears     | Bern
 Sharks    | Lima
(4 rows)
```

别名也可以省略 `AS`：
```sql
SELECT
  team team_name,
  city city_name
FROM teams;
```

别名带空格，使用双引号：
```sql
SELECT
  team AS "Team Name",
  city AS "City Name"
FROM teams;
```

输出：
```
 Team Name | City Name
-----------+-----------
 Lions     | Rome
 Owls      | Oslo
 Bears     | Bern
 Sharks    | Lima
(4 rows)
```

> ⚠️ 注意：不要混淆双引号 `"` 和单引号 `'`。
> - `"双引号"`：用于标识符（表名、列名、别名）
> - `'单引号'`：用于字符串字面量

## 表别名 Table alias

### 语法
```sql
SELECT column_list
FROM table_name AS alias_name;
```

`AS` 关键字对于表别名同样可以省略：
```sql
SELECT column_list
FROM table_name alias_name;
```

### 为什么要用表别名
1. 缩短长表名，SQL 更简洁；
2. 多表连接时，区分多张表的同名字段；
3. 自连接（一张表自己和自己做连接）必须使用表别名。

### 简单示例
给 `teams` 设置别名 `t`：
```sql
SELECT t.team, t.city
FROM teams t;
```

### 多表 JOIN 中使用表别名
结合之前的 `teams` 和 `players`，使用别名简化写法：
```sql
SELECT
  t.id team_id,
  t.team,
  t.city,
  p.id player_id,
  p.player,
  p.role
FROM teams t
INNER JOIN players p
  ON t.id = p.team_id;
```

输出：
```
 team_id | team  | city | player_id | player | role
---------+-------+------+-----------+--------+-------
       1 | Lions | Rome |         1 | Ava    | Guard
       1 | Lions | Rome |         2 | Noah   | Wing
       2 | Owls  | Oslo |         3 | Emma   | Back
(3 rows)
```

> 如果不写别名，就要完整写 `teams.id`、`players.team_id`，代码冗长。

### 自连接场景（表自己连接自己）
自连接时，**必须给同一张表设置两个不同别名**，否则数据库分不清引用的是哪一份。

示例表（employees 员工表，manager_id 指向另一位员工）：
```sql
CREATE TABLE employees (
  id INT PRIMARY KEY,
  name VARCHAR(100) NOT NULL,
  manager_id INT
);

INSERT INTO employees (id, name, manager_id) VALUES
  (1, 'Alice', NULL),
  (2, 'Bob', 1),
  (3, 'Charlie', 1),
  (4, 'David', 2);
```

需求：查询员工姓名，以及他的直属上级姓名。
```sql
SELECT
  e.name employee_name,
  m.name manager_name
FROM employees e
LEFT JOIN employees m
  ON e.manager_id = m.id;
```

输出：
```
 employee_name | manager_name
---------------+--------------
 Alice         | null
 Bob           | Alice
 Charlie       | Alice
 David         | Bob
(4 rows)
```

> 这里 `e` 代表员工，`m` 代表管理者，是同一张物理表，靠别名区分。

## 别名常见注意点
1. **列别名不能直接用在 `WHERE` 子句**
```sql
-- ❌错误，WHERE看不到SELECT里定义的列别名
SELECT team AS team_name FROM teams WHERE team_name = 'Lions';
```
原因：SQL执行顺序：`FROM → WHERE → SELECT`。WHERE执行的时候，SELECT的别名还没有生成。

解决办法：要么写原始列名，要么套一层子查询 / CTE。
```sql
-- ✅正确
SELECT team AS team_name FROM teams WHERE team = 'Lions';
```

> 但列别名**可以用于 ORDER BY**，ORDER BY 在 SELECT 之后执行。
```sql
SELECT team AS team_name FROM teams ORDER BY team_name;
```

2. 表别名的作用域：只属于当前这条查询语句，语句结束自动消失，不修改数据库元数据。

## 小结
- **列别名**：给输出的查询列起临时名字，`AS` 可省略；带空格别名用双引号。
- **表别名**：给表起短代号，多用于多表JOIN、自连接。
- WHERE 不能引用 SELECT 的列别名；ORDER BY 可以使用列别名。
- 自连接：同一张表必须分配两个不同别名。

上一篇：PostgreSQL FULL OUTER JOIN
下一篇：PostgreSQL GROUP BY
```

---

### 中文翻译版
```markdown
# PostgreSQL 别名（Alias）

> 别名可以给表或者查询列设置临时名称，别名仅在当前SQL语句内生效。

**摘要**：本教程学习PostgreSQL别名，分为**列别名**与**表别名**。

在PostgreSQL中，可以使用别名，为表或者查询中的列指定临时名称。别名的生命周期仅限于当前这条SQL。

## 列别名
语法：
```sql
SELECT 列名 AS 别名
FROM 表名;
```

关键字 `AS` 可以直接省略：
```sql
SELECT 列名 别名
FROM 表名;
```

如果别名里面包含空格，必须用**双引号**把别名括起来：
```sql
SELECT 列名 AS "带空格的别名"
FROM 表名;
```

### 示例
沿用之前的 teams 队伍表：
```sql
SELECT * FROM teams;
```
输出：
```
 id | team   | city
----+--------+------
  1 | Lions  | Rome
  2 | Owls   | Oslo
  3 | Bears  | Bern
  4 | Sharks | Lima
(4 行记录)
```

给列设置别名：
```sql
SELECT
  team AS team_name,
  city AS city_name
FROM teams;
```

输出：
```
 team_name | city_name
-----------+-----------
 Lions     | Rome
 Owls      | Oslo
 Bears     | Bern
 Sharks    | Lima
(4 行记录)
```

省略 AS 的写法：
```sql
SELECT
  team team_name,
  city city_name
FROM teams;
```

别名带空格，双引号包裹：
```sql
SELECT
  team AS "队伍名称",
  city AS "城市名称"
FROM teams;
```

> ⚠️重要区分：
> - `"双引号"`：用于标识符（表、列、别名）
> - `'单引号'`：用于字符串常量

## 表别名
语法：
```sql
SELECT 字段列表
FROM 表名 AS 别名;
```

表别名的 `AS` 同样可以省略：
```sql
SELECT 字段列表
FROM 表名 别名;
```

### 使用表别名的场景
1. 简化冗长的表名，SQL更简短；
2. 多表连接时，区分多张表中重名的字段；
3. **自连接**：一张表和自身做连接，必须使用表别名。

简单示例，teams设置别名`t`：
```sql
SELECT t.team, t.city
FROM teams t;
```

### JOIN多表连接使用别名
```sql
SELECT
  t.id team_id,
  t.team,
  t.city,
  p.id player_id,
  p.player,
  p.role
FROM teams t
INNER JOIN players p
  ON t.id = p.team_id;
```

输出：
```
 team_id | team  | city | player_id | player | role
---------+-------+------+-----------+--------+-------
       1 | Lions | Rome |         1 | Ava    | Guard
       1 | Lions | Rome |         2 | Noah   | Wing
       2 | Owls  | Oslo |         3 | Emma   | Back
(3 行记录)
```

### 自连接示例
员工表，`manager_id` 引用本表另一个员工id：
```sql
CREATE TABLE employees (
  id INT PRIMARY KEY,
  name VARCHAR(100) NOT NULL,
  manager_id INT
);

INSERT INTO employees (id, name, manager_id) VALUES
  (1, 'Alice', NULL),
  (2, 'Bob', 1),
  (3, 'Charlie', 1),
  (4, 'David', 2);
```

查询员工以及对应的上级名字：
```sql
SELECT
  e.name employee_name,
  m.name manager_name
FROM employees e
LEFT JOIN employees m
  ON e.manager_id = m.id;
```

输出：
```
 employee_name | manager_name
---------------+--------------
 Alice         | null
 Bob           | Alice
 Charlie       | Alice
 David         | Bob
(4 行记录)
```

> `e`代表员工，`m`代表管理者，物理上是同一张表，靠别名区分。

## 别名重要坑点
1. **WHERE 子句不能使用 SELECT 中定义的列别名**
```sql
-- ❌错误写法
SELECT team AS team_name FROM teams WHERE team_name = 'Lions';
```

SQL执行顺序：`FROM → WHERE → SELECT`。WHERE执行时，SELECT的别名还未生成。

正确写法，直接使用原始列名：
```sql
-- ✅正确
SELECT team AS team_name FROM teams WHERE team = 'Lions';
```

> 但是！`ORDER BY` **可以使用列别名**，ORDER BY执行在SELECT之后。
```sql
SELECT team AS team_name FROM teams ORDER BY team_name;
```

2. 表别名只在当前查询语句生效，语句结束自动失效，不会改动数据库。

## 总结
- 列别名：给输出字段起临时名字，AS可省略；带空格别名用双引号。
- 表别名：简写表名，多用于多表JOIN、自连接。
- WHERE不能引用SELECT的列别名；ORDER BY可以。
- 自连接：同一张表必须分配两套不同别名。
