# PostgreSQL 表连接（Joins）

**概要**：本教程将学习PostgreSQL各类连接，包括内连接、左连接、右连接、全外连接。

PostgreSQL 的连接用于基于多张关联表之间的公共列，合并一张（自连接）或多张表的列数据。公共列通常是第一张表的主键，和第二张表的外键。

PostgreSQL 支持：内连接、左连接、右连接、全外连接、交叉连接、自然连接，还有一种特殊的连接叫做自连接。

## 准备示例表
假设有两张表：`teams`（队伍）和`players`（球员）
```sql
CREATE TABLE teams (
  id INT PRIMARY KEY,
  team VARCHAR (100) NOT NULL,
  city VARCHAR (100) NOT NULL
);

CREATE TABLE players (
  id INT PRIMARY KEY,
  team_id INT REFERENCES teams (id),
  player VARCHAR (100) NOT NULL,
  role VARCHAR (100) NOT NULL
);

INSERT INTO teams (id, team, city) VALUES
  (1, 'Lions', 'Rome'),
  (2, 'Owls', 'Oslo'),
  (3, 'Bears', 'Bern'),
  (4, 'Sharks', 'Lima');

INSERT INTO players (id, team_id, player, role) VALUES
  (1, 1, 'Ava', 'Guard'),
  (2, 1, 'Noah', 'Wing'),
  (3, 2, 'Emma', 'Back'),
  (4, NULL, 'Liam', 'Guard'),
  (5, NULL, 'Mia', 'Wing');
```

一支队伍可以拥有多名球员。部分球员尚未归属任何队伍，他们的`team_id`为`NULL`。

查询`teams`表全部数据：
```sql
SELECT * FROM teams;
```
输出结果：
```
id | team   | city
----+--------+------
  1 | Lions  | Rome
  2 | Owls   | Oslo
  3 | Bears  | Bern
  4 | Sharks | Lima
(4 行记录)
```

查询`players`表全部数据：
```sql
SELECT * FROM players;
```
输出结果：
```
id | team_id | player | role
----+---------+--------+-------
  1 | 1       | Ava    | Guard
  2 | 1       | Noah   | Wing
  3 | 2       | Emma   | Back
  4 | null    | Liam   | Guard
  5 | null    | Mia    | Wing
(5 行记录)
```

## PostgreSQL 内连接 INNER JOIN
下面语句将`teams`表与`players`表，根据`id`与`team_id`列的值进行连接：
```sql
SELECT
  teams.id AS team_id,
  team,
  city,
  players.id AS player_id,
  player,
  role
FROM
  teams
INNER JOIN players
  ON teams.id = players.team_id;
```

> 注意：`JOIN` 是 `INNER JOIN` 的简写，二者等价。

输出：
```
team_id | team  | city | player_id | player | role
---------+-------+------+-----------+--------+-------
       1 | Lions | Rome |         1 | Ava    | Guard
       1 | Lions | Rome |         2 | Noah   | Wing
       2 | Owls  | Oslo |         3 | Emma   | Back
(3 行记录)
```

内连接遍历左表`teams`每一行，拿`id`的值与右表`players`每一行的`team_id`对比。
如果两边值相等，就生成一行包含两张表字段的结果，加入结果集。

## PostgreSQL 左连接 LEFT JOIN
下面语句使用左连接关联`teams`与`players`。在左连接定义中，写在左边的表称为**左表**，右边的称为**右表**。
```sql
SELECT
  teams.id AS team_id,
  team,
  city,
  players.id AS player_id,
  player,
  role
FROM
  teams
LEFT JOIN players
  ON teams.id = players.team_id;
```

输出：
```
team_id | team   | city | player_id | player | role
---------+--------+------+-----------+--------+-------
       1 | Lions  | Rome |         1 | Ava    | Guard
       1 | Lions  | Rome |         2 | Noah   | Wing
       2 | Owls   | Oslo |         3 | Emma   | Back
       3 | Bears  | Bern |      null | null   | null
       4 | Sharks | Lima |      null | null   | null
(5 行记录)
```

左连接**以左表为基准读取全部数据**，拿左表`id`和右表`players`的`team_id`做匹配。
1. 如果匹配成功：拼接两张表字段，生成结果行（结果前3行）。
2. 如果匹配不到：依然保留左表行，**右表全部字段填充为NULL**（结果最后两行）。

筛选「左表中，在右表没有匹配记录」的数据，可以左连接搭配WHERE条件：
```sql
SELECT
  teams.id AS team_id,
  team,
  city,
  players.id AS player_id,
  player,
  role
FROM
  teams
LEFT JOIN players
  ON teams.id = players.team_id
WHERE players.id IS NULL;
```

输出：
```
team_id | team   | city | player_id | player | role
---------+--------+------+-----------+--------+------
       3 | Bears  | Bern |      null | null   | null
       4 | Sharks | Lima |      null | null   | null
(2 行记录)
```

> `LEFT JOIN` 和 `LEFT OUTER JOIN` 完全等价，可以互换使用。
> **左反连接**：只取出左表、右表无匹配的数据。

## PostgreSQL 右连接 RIGHT JOIN
右连接等价于把左右表互换的左连接。以**右表作为基准**读取全部数据，拿右表的`team_id`与左表`teams`的`id`对比。
1. 匹配成功：拼接两张表字段；
2. 匹配失败：保留右表行，**左表字段全部填充NULL**。

```sql
SELECT
  teams.id AS team_id,
  team,
  city,
  players.id AS player_id,
  player,
  role
FROM
  teams
RIGHT JOIN players
  ON teams.id = players.team_id;
```

输出：
```
team_id | team  | city | player_id | player | role
---------+-------+------+-----------+--------+-------
       1 | Lions | Rome |         1 | Ava    | Guard
       1 | Lions | Rome |         2 | Noah   | Wing
       2 | Owls  | Oslo |         3 | Emma   | Back
    null | null  | null |         4 | Liam   | Guard
    null | null  | null |         5 | Mia    | Wing
(5 行记录)
```

筛选「右表中，左表没有匹配」的数据，加上WHERE判断左表主键为NULL：
```sql
SELECT
  teams.id AS team_id,
  team,
  city,
  players.id AS player_id,
  player,
  role
FROM
  teams
RIGHT JOIN players
  ON teams.id = players.team_id
WHERE teams.id IS NULL;
```

输出：
```
team_id | team | city | player_id | player | role
---------+------+------+-----------+--------+-------
    null | null | null |         4 | Liam   | Guard
    null | null | null |         5 | Mia    | Wing
(2 行记录)
```

> `RIGHT JOIN` 与 `RIGHT OUTER JOIN` 等价，可以互换。
> **右反连接**：只取出右表、左表无匹配的数据。

## PostgreSQL 全外连接 FULL OUTER JOIN
全外连接（全连接）返回**左右两张表全部行**。两边匹配上就拼接全部字段；没有匹配的一侧字段填充NULL。
```sql
SELECT
  teams.id AS team_id,
  team,
  city,
  players.id AS player_id,
  player,
  role
FROM
  teams
FULL OUTER JOIN players
  ON teams.id = players.team_id;
```

输出：
```
team_id | team   | city | player_id | player | role
---------+--------+------+-----------+--------+-------
       1 | Lions  | Rome |         1 | Ava    | Guard
       1 | Lions  | Rome |         2 | Noah   | Wing
       2 | Owls   | Oslo |         3 | Emma   | Back
       3 | Bears  | Bern |      null | null   | null
       4 | Sharks | Lima |      null | null   | null
    null | null   | null |         4 | Liam   | Guard
    null | null   | null |         5 | Mia    | Wing
(7 行记录)
```

只取出两边**互不匹配**的数据（两边独有的数据，去掉两边匹配成功的交集）：
```sql
SELECT
  teams.id AS team_id,
  team,
  city,
  players.id AS player_id,
  player,
  role
FROM
  teams
FULL JOIN players
  ON teams.id = players.team_id
WHERE teams.id IS NULL OR players.id IS NULL;
```

输出结果：
```
team_id | team   | city | player_id | player | role
---------+--------+------+-----------+--------+-------
       3 | Bears  | Bern |      null | null   | null
       4 | Sharks | Lima |      null | null   | null
    null | null   | null |         4 | Liam   | Guard
    null | null   | null |         5 | Mia    | Wing
(4 行记录)
```

