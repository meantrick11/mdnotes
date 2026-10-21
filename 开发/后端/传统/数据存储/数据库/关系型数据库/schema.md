# 数据库 Schema
> 一句话：**Schema（模式）是数据库里对数据结构、约束、表之间关系的一套定义；可以理解成数据库的“蓝图”**，不存真实数据，只描述数据长什么样。

## 1. 通俗类比
- 数据库 = 一栋大楼
- Schema = 大楼的图纸（房间划分、门牌号、墙、管线规则）
- 表 = 图纸里定义的房间
- 行数据 = 房间里实际住的人

## 2. 包含哪些内容
Schema 定义的元信息（metadata）：
1. 表名、字段名
2. 字段类型：`int` / `varchar` / `datetime`
3. 约束：主键、外键、非空、唯一、默认值
4. 索引、视图、存储过程
5. 表与表之间关联关系

> 真实存的数据，不属于 schema。

## 3. 两种常见理解（容易混淆）
### ① 逻辑定义（理论定义）
就是上面说的：数据结构的描述，**抽象蓝图**。

### ② 数据库产品里的物理对象（MySQL / PostgreSQL）
- **PostgreSQL**：`schema` 是数据库内部的命名空间。一个数据库可以有多个 schema，用来分组表，隔离权限。
  ```sql
  create schema shop;
  create table shop.order(...); -- shop就是schema名
  ```
- **MySQL**：MySQL 里 `schema` ≈ `database`，`CREATE SCHEMA` 和 `CREATE DATABASE` 效果几乎一样。

> 重点区分：PostgreSQL 的 schema 是库下的命名空间；MySQL 的 schema 等价数据库。

## 4. 常见衍生词
- **Schema Design**：数据库表结构设计
- **Schema Migration**：结构变更（加字段、改表，迁移脚本）
- **JSON Schema**：不是数据库，是用来校验 JSON 数据格式的规则
- **Schema-on-write**：写入时强制校验结构（传统关系型数据库）
- **Schema-on-read**：读的时候才解析结构（Hadoop、部分NoSQL）

## 5. 极简总结
1. 本质：**数据结构的元描述**
2. 关系库：定义表、字段、约束、关联
3. PG：schema = 库内命名空间；MySQL：schema ≈ database
4. Schema 存**结构定义**，不存业务数据

