# WHERE

**注意：如果是字符串，那么应该使用""包括，而不是WHERE column_name = ?**

## 条件语句

SELECT column,anthor_column FROM MYTABLE WHERE condiction AND/OR another_condition;

SELECT * FORM MYTABLE WHERE condition;


## 判断语句
### 数值
#### 大于小于(不)等于

col_name != 4

#### BETWEEN ... AND...

col_name BETWEEN 1.5 AND 6

#### NOT BETWEEN ... AND ...

col_name NOT BETWEEN 1 AND 5

#### IN(...)

col_name IN (1,2,3)

#### NOT IN(...)

col_name NOT IN (4,5,6)

### LIMIT
限制前多少条？
LIMIT 5
### SQL 字符串条件运算符

| 运算符 | 含义（中文） | 示例 |
| -------- | --------- | ------- |
| = | 区分大小写的精确字符串匹配 | col_name = "abc" |
| != 或 <> | 区分大小写的精确字符串不匹配 | col_name != "abcd" |
| LIKE | 不区分大小写的精确字符串匹配 | col_name LIKE "ABC" |
| NOT LIKE | 不区分大小写的精确字符串不匹配 | col_name NOT LIKE "ABCD" |
| % | 匹配任意个字符（0个或多个，仅配合 LIKE 使用） | col_name LIKE "%AT%"<br>(匹配 "AT", "ATTIC", "CAT" 或 "BATS") |
| _ | 匹配单个任意字符（仅配合 LIKE 使用） | col_name LIKE "AN_"<br>(匹配 "AND", 不匹配 "AN") |
| IN (...) | 字符串在指定列表中存在 | col_name IN ("A", "B", "C") |
| NOT IN (...) | 字符串不在指定列表中 | col_name NOT IN ("D", "E", "F") |


SELECT * FROM movies WHERE title LIKE "WALL-_";