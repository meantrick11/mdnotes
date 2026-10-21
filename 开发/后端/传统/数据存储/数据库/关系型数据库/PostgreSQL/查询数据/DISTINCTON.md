# PostgreSQL DISTINCT ON

**概述**：本教程讲解 PostgreSQL `DISTINCT ON` 子句，实现根据指定字段获取不重复的数据行。

## PostgreSQL DISTINCT ON 子句简介
`DISTINCT ON` 可以基于指定字段提取唯一行。**`DISTINCT则是完全每列都要相等才会去重**，基础语法：
```sql
SELECT DISTINCT ON (column1, column2,...) column1, column2, ...
FROM table_name
ORDER BY column1, column2, ...;
```
作用：对每组指定字段/字段组合，仅保留结果集中**第一条**匹配数据。

决定最终保留哪一行的核心：`ORDER BY` 子句中的排序字段。

技术上可以不写 `ORDER BY` 直接使用 `DISTINCT ON`，但数据表原生行顺序无固定规则，不排序时“第一条”数据不可预测，结果随机。

重要规范：`DISTINCT ON()` 括号内的字段，必须与 `ORDER BY` 最左侧排序字段保持一致。

## PostgreSQL DISTINCT ON 使用示例
新建学生成绩表 student_scores 演示用法：
### 1）创建表
```sql
CREATE TABLE student_scores (
  id SERIAL PRIMARY KEY,
  name VARCHAR(50) NOT NULL,
  subject VARCHAR(50) NOT NULL,
  score INTEGER NOT NULL
);
```
### 2）插入测试数据
```sql
INSERT INTO student_scores (name, subject, score) VALUES
('Alice', 'Math', 90),
('Bob', 'Math', 85),
('Alice', 'Physics', 92),
('Bob', 'Physics', 88),
('Charlie', 'Math', 95),
('Charlie', 'Physics', 90);
```
表中每个学生都有数学、物理两门成绩。

### 3）查询：获取每位学生最高单科成绩
```sql
SELECT DISTINCT ON (name) name, subject, score
FROM student_scores
ORDER BY name, score DESC;
```
输出：
```
name    | subject | score
---------+---------+-------
Alice   | Physics | 92
Bob     | Physics | 88
Charlie | Math    | 95
(3 rows)
```
结果为每位学生各自最高分记录。
`ORDER BY name, score DESC` 先按姓名分组，组内成绩倒序，因此 `DISTINCT ON(name)` 会保留每组分数最高的第一条数据。

## 小结
1. `DISTINCT ON` 功能：对指定字段分组，每组仅保留第一条唯一记录；
2. 必须搭配 `ORDER BY`，否则每组保留的数据随机、不可控；
3. `DISTINCT ON(分组字段)` 必须与 ORDER BY 最前置字段匹配。

