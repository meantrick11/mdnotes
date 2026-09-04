# PostgreSQL 内连接（INNER JOIN）
**摘要**：本教程将学习如何使用 PostgreSQL 的 `INNER JOIN`（内连接）子句从多张表查询数据。

## PostgreSQL INNER JOIN 简介
在关系型数据库中，数据通常分散存储在多张表里。想要拿到完整的数据，往往需要同时查询多张表。

本教程重点讲解使用 `INNER JOIN` 从多张表获取数据。

下面是两张表做内连接的通用语法：
```sql
SELECT
  select_list
FROM
  table1
INNER JOIN table2
  ON table1.column_name = table2.column_name;
```

语法说明：
1. 在 `SELECT` 的选择列表中，指定需要从两张表取出的字段。
2. 在 `FROM` 子句中，指定主表 `table1`。
3. 使用 `INNER JOIN` 关键字指定要关联的第二张表 `table2`。
4. 定义连接条件：该条件指明两张表中哪一列的值需要互相匹配。

可以使用**表别名**来缩短 SQL：
```sql
SELECT
  select_list
FROM
  table1 t1
INNER JOIN table2 t2
  ON t1.column_name = t2.column_name;
```

这里把 `t1`、`t2` 分别作为 `table1`、`table2` 的别名，之后就可以用别名限定各个表的字段。

如果用于匹配的列名称完全相同，可以使用 `USING` 语法简写：
```sql
SELECT
  select_list
FROM
  table1 t1
INNER JOIN table2 t2
USING(column_name);
```

### INNER JOIN 的工作原理
针对 `table1` 的每一行，内连接拿该行的字段值，和 `table2` 每一行对应列的值做对比。

当两边的值相等时，内连接生成一条包含两张表全部字段的新行，并加入结果集。

如果值不相等，则丢弃这一组行对，继续下一行，重复匹配过程。

（文中维恩图示意内连接逻辑）

## PostgreSQL INNER JOIN 示例
下面来看几组 INNER JOIN 的使用示例。

### 1）使用 INNER JOIN 连接两张表
以示例库中的 `customer`（客户）表和 `payment`（支付）表为例。

业务关系：客户每产生一笔支付，就会在 `payment` 表插入一条记录。一个客户可以有 0 条或多条支付记录；每一笔支付只属于一个客户。`customer_id` 是两张表之间的关联字段。

下面语句使用 INNER JOIN 查询两张表的数据：
```sql
SELECT
  customer.customer_id,
  customer.first_name,
  customer.last_name,
  payment.amount,
  payment.payment_date
FROM
  customer
INNER JOIN payment
  ON payment.customer_id = customer.customer_id
ORDER BY
  payment.payment_date;
```

输出：
```
customer_id | first_name | last_name | amount | payment_date
-------------+------------+-----------+--------+----------------------------
  416       | Jeffery    | Pinson    | 2.99   | 2007-02-14 21:21:59.996577
  516       | Elmer      | Noe       | 4.99   | 2007-02-14 21:23:39.996577
  239       | Minnie     | Romero    | 4.99   | 2007-02-14 21:29:00.996577
  592       | Terrance   | Roush     | 6.99   | 2007-02-14 21:41:12.996577
  49        | Joyce      | Edwards   | 0.99   | 2007-02-14 21:44:52.996577
 ...
```

使用表别名简化写法：
```sql
SELECT
  c.customer_id,
  c.first_name,
  c.last_name,
  p.amount,
  p.payment_date
FROM
  customer c
INNER JOIN payment p
  ON p.customer_id = c.customer_id
ORDER BY
  p.payment_date;
```

两张表的关联列都叫 `customer_id`，可以使用 `USING`：
```sql
SELECT
  customer_id,
  first_name,
  last_name,
  amount,
  payment_date
FROM
  customer
INNER JOIN payment
USING(customer_id)
ORDER BY
  payment_date;
```

### 2）使用 INNER JOIN 连接三张表
下面的关系图展示三张表：`staff`（员工）、`payment`（支付）、`customer`（客户）。

一名员工可以处理 0 笔或多笔支付，每一笔支付由且仅由一名员工处理。
同理，一个客户可以产生 0 笔或多笔支付，每一笔支付只对应一位客户。

下面示例通过多次 INNER JOIN 查询三张表的数据：
```sql
SELECT
  c.customer_id,
  c.first_name || ' ' || c.last_name customer_name,
  s.first_name || ' ' || s.last_name staff_name,
  p.amount,
  p.payment_date
FROM
  customer c
INNER JOIN payment p USING(customer_id)
INNER JOIN staff s USING(staff_id)
ORDER BY payment_date;
```

输出：
```
customer_id | customer_name   | staff_name   | amount | payment_date
-------------+-----------------+--------------+--------+----------------------------
  416       | Jeffery Pinson  | Jon Stephens | 2.99   | 2007-02-14 21:21:59.996577
  516       | Elmer Noe       | Jon Stephens | 4.99   | 2007-02-14 21:23:39.996577
  239       | Minnie Romero   | Mike Hillyer | 4.99   | 2007-02-14 21:29:00.996577
  592       | Terrance Roush  | Jon Stephens | 6.99   | 2007-02-14 21:41:12.996577
  49        | Joyce Edwards   | Mike Hillyer | 0.99   | 2007-02-14 21:44:52.996577
 ...
```

## 小结
- 使用 `INNER JOIN` 从多张关联表中查询数据。
- `INNER JOIN` 仅返回**两边都能匹配成功**的行。
- 可使用表别名缩短语句；关联列同名时可以用 `USING(列名)` 替代 `ON`。
