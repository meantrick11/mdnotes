# PostgreSQL LIKE 模糊匹配运算符
**要点**：本教程学习使用 `LIKE` 运算符，通过通配符模式模糊匹配查询字符串数据。

## PostgreSQL LIKE 运算符简介
业务场景：记不清完整姓名，只记得开头是 Jen，需要模糊查询客户。
使用 `LIKE` 搭配通配符即可实现模糊检索，示例：
```sql
SELECT
  first_name,
  last_name
FROM
  customer
WHERE
  first_name LIKE 'Jen%';
```
输出：
```
first_name | last_name
------------+-----------
Jennifer    | Davis
Jennie      | Terry
Jenny       | Castro
(3 行)
```
### 通配符说明
PostgreSQL 提供两种通配符：
1. `%`：匹配任意长度（0个或多个）任意字符
2. `_`：匹配**单个**任意字符

基础语法：
```sql
字段 LIKE 匹配模板
```
- 匹配成功返回 true；
- 取反使用 `NOT LIKE`，匹配失败时返回 true；
- 模板不含任何通配符时，`LIKE` 等价于等于运算符 `=`。

## PostgreSQL LIKE 运算符示例
### 1）基础单值匹配测试
无通配符，等价等于判断：
```sql
SELECT 'Apple' LIKE 'Apple' AS result;
```
输出：
```
result
--------
t
(1 行)
```
匹配以 A 开头的字符串：
```sql
SELECT 'Apple' LIKE 'A%' AS result;
```
输出：
```
result
--------
t
(1 行)
```

### 2）表数据模糊匹配（包含指定字符串）
查询名字中包含 `er` 的客户：
```sql
SELECT
  first_name,
  last_name
FROM
  customer
WHERE
  first_name LIKE '%er%'
ORDER BY first_name;
```
输出：
```
first_name | last_name
-------------+-------------
Albert      | Crouse
Alberto     | Henning
Alexander   | Fennell
Amber       | Dixon
Bernard     | Colby
...
```

### 3）同时使用 % 和 _ 通配符
模板 `_her%` 规则：
- 第1位：任意单个字符
- 紧接着固定字符串 `her`
- 后面允许任意长度字符
```sql
SELECT
  first_name,
  last_name
FROM
  customer
WHERE
  first_name LIKE '_her%'
ORDER BY first_name;
```
输出：
```
first_name | last_name
------------+-----------
Cheryl      | Murphy
Sherri      | Rhodes
Sherry      | Marshall
Theresa     | Watson
(4 行)
```

### 4）NOT LIKE 排除匹配
查询名字不以 Jen 开头的所有客户：
```sql
SELECT
  first_name,
  last_name
FROM
  customer
WHERE
  first_name NOT LIKE 'Jen%'
ORDER BY first_name;
```
输出：
```
first_name | last_name
-------------+--------------
Aaron       | Selby
Adam        | Gooch
Adrian      | Clary
Agnes       | Bishop
...
```

## PostgreSQL LIKE 扩展语法
### ILIKE 大小写不敏感匹配
`LIKE` 区分大小写；`ILIKE` 忽略大小写。
示例匹配以 BAR/Bar/bar 开头的姓名：
```sql
SELECT
  first_name,
  last_name
FROM
  customer
WHERE
  first_name ILIKE 'BAR%';
```
输出：
```
first_name | last_name
------------+-----------
Barbara     | Jones
Barry       | Lovelace
(2 行)
```
若改用 `LIKE 'BAR%'` 严格大写匹配，无任何结果。

### 等价简写运算符
| 简写运算符 | 等价写法 |
|---|---|
| ~~ | LIKE |
| ~~* | ILIKE |
| !~~ | NOT LIKE |
| !~~* | NOT ILIKE |

示例使用 `~~` 替代 LIKE：
```sql
SELECT
  first_name,
  last_name
FROM
  customer
WHERE
  first_name ~~ 'Dar%'
ORDER BY first_name;
```
输出：
```
first_name | last_name
------------+-----------
Darlene     | Rose
Darrell     | Power
Darren      | Windham
Darryl      | Ashcraft
Daryl       | Larue
(5 行)
```

## ESCAPE 转义通配符
如果原始数据包含 `%`、`_` 符号，需要用 `ESCAPE` 指定转义字符，让通配符恢复普通字符含义。
语法：
```sql
字符串 LIKE 匹配模板 ESCAPE 转义符;
```

### 演示示例
1. 创建测试表并插入带特殊符号数据
```sql
CREATE TABLE t(
  message text
);

INSERT INTO t(message)
VALUES
('The rents are now 10% higher than last month'),
('The new film will have _ in the title');

SELECT message FROM t;
```
2. 转义匹配包含 `10%` 的记录，使用 `$` 作为转义符
```sql
SELECT * FROM t WHERE message LIKE '%10$%%' ESCAPE '$';
```
模板解析：`$%` 代表普通百分号，前后两个 `%` 是通配符。
输出：
```
message
----------------------------------------------
The rents are now 10% higher than last month
(1 行)
```

## 小结
1. `LIKE`：区分大小写模糊匹配；`NOT LIKE` 取反匹配；
2. 通配符：`%` 匹配任意多字符，`_` 匹配单个字符；
3. `ILIKE`：大小写无关模糊查询；
4. `ESCAPE`：自定义转义字符，匹配原生包含 %/_ 的文本；
5. 简写运算符 `~~ / ~~* / !~~ / !~~*` 等价于 LIKE/ILIKE 系列。

