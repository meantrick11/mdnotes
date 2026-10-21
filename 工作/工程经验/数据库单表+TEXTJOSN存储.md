# 复杂对象怎么存？TEXT + Fastjson2 手动序列化，和其他方案全对比

## 一、从一个让人头疼的场景说起

假设你在做一个用户系统，每个用户有一个“收货地址列表”，里面可能存了好几个地址，每个地址包含省份、城市、街道。

如果老老实实建一张 `user_address` 表，用外键关联用户，当然是最规范的做法。但很多业务场景下，这些地址只是**附属信息**：你从来不会单独查“所有住在朝阳路的人”，只会跟着用户一起查出来展示。

这时候专门建一张表、写一堆 JOIN，反而让代码变得臃肿。

有没有一种办法，能把一个复杂的 Java 对象（比如 `List<Address>`）**直接塞进一个数据库字段**里？

有。这就是 **TEXT 列存 JSON** 要解决的问题。

---

## 二、TEXT 列 + Fastjson2 手动序列化：像搬家打包一样

### 1. 核心思路

想象你要搬家，不可能把整个衣柜直接搬走，而是把衣服叠好放进箱子，到了新家再拿出来挂好。

TEXT 列存 JSON 就是同样的逻辑：

- **存的时候**：把 Java 对象（比如 `List<Address>`）用 Fastjson2 转成一串 JSON 文本，比如 `[{"city":"北京","street":"朝阳路"}]`，然后把这个字符串存进数据库的 TEXT 列。
- **取的时候**：从数据库读出这串 JSON 文本，再用 Fastjson2 把它“还原”成原来的 Java 对象。

为什么用 TEXT 而不是 MySQL 的 JSON 类型？

因为 **TEXT 是所有数据库都有的通用类型**。不用纠结 MySQL 支不支持、PostgreSQL 的 jsonb 怎么配，兼容性最好。代价是数据库不会帮你校验 JSON 格式的合法性，正确性完全靠你的代码保证。

### 2. 准备依赖

在 `pom.xml` 里加入 Fastjson2：

```xml
<dependency>
    <groupId>com.alibaba.fastjson2</groupId>
    <artifactId>fastjson2</artifactId>
    <version>2.0.52</version>
</dependency>
```

### 3. 存：手动序列化写入 TEXT 列

假设实体类长这样：

```java
public class User {
    private Long id;
    private String name;
    private List<Address> addressList;  // 给业务代码用
    private String addressJson;         // 真正存到数据库 TEXT 列的字段
}
```

**关键原则**：`addressList` 是给业务代码用的，`addressJson` 是给数据库用的。写库之前，**手动**把 `addressList` 转成 JSON 字符串赋给 `addressJson`：

```java
import com.alibaba.fastjson2.JSON;

User user = new User();
user.setName("张三");
user.setAddressList(Arrays.asList(
    new Address("北京", "朝阳路"),
    new Address("上海", "南京路")
));

// 手动序列化：List<Address> → JSON 字符串
user.setAddressJson(JSON.toJSONString(user.getAddressList()));
```

然后 Mapper 里正常写 SQL，只处理 `addressJson` 字段：

```xml
<insert id="insertUser">
    INSERT INTO user (name, address_json)
    VALUES (#{name}, #{addressJson})
</insert>
```

**为什么叫“手动”？**

因为 MyBatis 默认不认识 `List<Address>` 这种复杂类型。你直接往 SQL 里传 `#{addressList}`，它只会调用 `toString()`，输出类似 `[Address@1a2b3c]` 的垃圾字符串，数据库直接报错。所以你**必须**自己先转好，再传字符串进去。

### 4. 取：手动反序列化还原对象

查出来的时候反过来操作。MyBatis 从 TEXT 列读出来的就是一个普通 `String`，你拿到这个字符串后，手动调 Fastjson2 还原：

```java
import com.alibaba.fastjson2.JSON;
import com.alibaba.fastjson2.TypeReference;

// MyBatis 查出来的 user 里，addressJson 是 "[{\"city\":\"北京\",...}]"
User user = userMapper.selectById(1L);

// 手动反序列化：JSON 字符串 → List<Address>
List<Address> list = JSON.parseObject(
    user.getAddressJson(),
    new TypeReference<List<Address>>() {}
);

user.setAddressList(list);
```

**注意**：这里用的是 `TypeReference`，而不是直接写 `List<Address>.class`。Java 的泛型在运行时会被擦除，`List<Address>` 这种带泛型的类型不能直接用 `Class` 对象传进去，`TypeReference` 就是为了解决这个问题。

### 5. 一个非常容易踩的坑：重复序列化

这是手动方案里最隐蔽的问题，很多新手都会中招。

**场景**：你的 `addressJson` 字段在某种情况下已经是 JSON 字符串了，你又手滑调了一次 `JSON.toJSONString()`，结果字符串被“套娃”了：

- 正常应该是：`[{"city":"北京"}]`
- 重复序列化后变成：`"[{\"city\":\"北京\"}]"`（外层多了转义和引号）

数据库里存的是这个被转义过的字符串，下次取出来 `JSON.parseObject` 直接报格式错误。

**怎么防？** 序列化之前先判断一下：

```java
public static String ensureJson(Object obj) {
    if (obj instanceof String) {
        String s = (String) obj;
        try {
            JSON.parse(s);      // 能解析说明已经是合法 JSON
            return s;           // 直接返回，不再序列化
        } catch (Exception ignored) {
            // 不是合法 JSON，继续往下走
        }
    }
    return JSON.toJSONString(obj);  // 非字符串或非 JSON，正常序列化
}
```

核心原则就是：**同一个对象只序列化一次，序列化前先确认它是不是已经序列化过了。**

### 6. 这个方案的优缺点

**优点**：

- 兼容性最强，任何数据库都有 TEXT 类型，不依赖数据库的 JSON 特性。
- 写入性能通常优于数据库原生 JSON 类型。有研究显示，TEXT 方式的插入速度比 JSON 类型快约 15%。
- 磁盘存储更紧凑。

**缺点**：

- 数据库完全不校验 JSON 合法性，脏数据可以畅通无阻地进库。
- 无法使用数据库提供的 JSON 函数进行查询、排序、聚合。
- 每次更新都要整体重写，哪怕只改一个字段。
- 正确性完全依赖代码，容易踩“重复序列化”之类的坑。

**适用场景**：只需要整存整取、不做 JSON 内部查询、数据库版本较老或需要跨多种数据库兼容的场景。

---

## 三、其他方式对比

TEXT + 手动序列化只是众多方案中的一种。下面我们把其他几种主流方案拉出来，做一个横向对比。

### 方案二：数据库原生 JSON 类型

代表：MySQL JSON、PostgreSQL JSONB。

**核心逻辑**：让数据库“认识”JSON，写入时自动校验合法性，内部用二进制格式存储，支持专门的查询函数和索引。

MySQL JSON 类型做了什么 TEXT 做不到的事：

| 能力 | TEXT 存 JSON | MySQL JSON 类型 |
|---|---|---|
| 写入校验 | 无，非法 JSON 照单全收 | 自动校验，非法 JSON 直接报错 |
| 内部存储 | 原始字符串 | 二进制格式，读取字段无需解析全文 |
| 部分更新 | 整体重写 | 支持部分更新优化 |
| 查询能力 | 无 | 支持 JSON_EXTRACT、JSON_SET 等函数 |
| 索引支持 | 无 | 支持虚拟列索引、多值索引 |

PostgreSQL 的 JSONB 更进一步。PG 提供两种 JSON 类型：`json` 存原始文本副本，每次操作都要重新解析；`jsonb` 存分解后的二进制格式，写入稍慢但查询快得多，且支持 GIN 索引，可以高效地对 JSON 内部字段做包含、存在性等查询。

**优点**：

- 数据合法性由数据库保证。
- 可以对 JSON 内部字段做条件查询、排序、聚合。
- 部分更新减少 I/O 和 Binlog 体积。

**缺点**：

- 写入性能略低于 TEXT。
- 不同数据库的 JSON 实现差异大，SQL 语法不通用。
- 迁移数据库时可能遇到兼容性问题。

**适用场景**：需要对 JSON 内部字段做查询筛选、但又不值得拆成独立关联表的场景。

### 方案三：ORM 框架自动映射

代表：MyBatis-Plus 的 TypeHandler、JPA 的 AttributeConverter。

**核心逻辑**：用框架提供的扩展点，在 Java 对象和数据库字符串之间自动完成序列化，业务代码中不再出现 `JSON.toJSONString()` 这样的手动调用。

MyBatis-Plus 内置了多种 JSON 类型处理器，包括 `Fastjson2TypeHandler`、`JacksonTypeHandler`、`GsonTypeHandler` 等，只需在实体类字段上加注解：

```java
@TableField(typeHandler = Fastjson2TypeHandler.class)
private List<Address> addressList;
```

JPA 2.1 则提供 `AttributeConverter` 接口，定义一个转换器类，实现从 Java 对象到数据库列的双向转换，然后在实体字段上用 `@Converter` 注解引用。

**优点**：

- 业务代码最干净，完全不需要手动调用序列化 API。
- 类型安全，转换逻辑集中在一处。
- 与 ORM 框架无缝集成，减少人为失误。

**缺点**：

- 只解决了“Java 对象 ↔ 字符串”的转换问题，**并没有改变底层存储方式**。你依然要选择底层是 TEXT 还是原生 JSON 类型。
- TypeHandler 是框架绑定级的方案，换 ORM 框架时需要重写。
- 调试时如果序列化出问题，排查链路比手动调用更长。

**适用场景**：已经使用 MyBatis-Plus 或 JPA，希望消除手动序列化代码、让数据访问层更整洁的项目。

### 方案四：关联表（规范化存储）

**核心逻辑**：老老实实建 `user_address` 子表，用外键关联，用 JOIN 查询。

**优点**：

- 数据完整性最好，外键约束保证不会出现“孤儿地址”。
- 可以对地址的每个字段做索引、查询、聚合。
- 部分更新天然支持，改一个地址只更新一行。
- 与关系型数据库的设计哲学一致，长期维护成本最低。

**缺点**：

- 表结构僵硬，字段变更需要 DDL 改表。
- 查询需要 JOIN，数据量大时连接开销显著。
- 对于“只是附带展示、从不做条件查询”的附属数据，建子表显得过度设计。

**适用场景**：数据有明确的实体关系、需要频繁查询筛选、对数据一致性要求高的核心业务数据。

### 方案五：文档数据库

代表：MongoDB。

**核心逻辑**：换一种数据库，原生以类 JSON 的 BSON 格式存储文档，整个对象直接作为一个文档存入，不需要序列化/反序列化那层转换。

**优点**：

- 与 Java 对象模型天然契合，嵌套结构直接映射。
- 模式灵活，字段可动态增减，不需要 DDL。
- 读写整个文档的性能好，没有 JOIN 开销。

**缺点**：

- 引入新的数据库技术栈，运维和团队学习成本高。
- 跨文档的关联查询性能远不如关系型 JOIN。
- 事务支持相对较弱。
- 如果原本就在用 MySQL/PG，为了存一个复杂对象而引入 MongoDB，通常是杀鸡用牛刀。

**适用场景**：数据本身就是文档型、模式频繁变化、整体以文档为单位读写、对强事务要求不高的场景。

### 方案六：序列化库的横向选择

无论你选哪种存储方案，序列化库的性能和特性都会影响整体表现。

| 维度 | Fastjson2 | Jackson | Gson |
|---|---|---|---|
| 序列化速度 | 极快 | 快 | 中等 |
| 生态成熟度 | 较新，但发展迅速 | 最成熟，Spring 生态默认 | 成熟但更新较慢 |
| 安全记录 | 历史上有过反序列化漏洞 | 较好 | 较好 |
| 功能丰富度 | 够用 | 最丰富，注解体系强大 | 基础够用 |

实测数据显示，在约 20 万条数据的序列化+反序列化场景中，Fastjson2 耗时约 110ms，Jackson 约 170ms，Gson 约 210ms。如果场景对序列化性能敏感，Fastjson2 是合理选择；如果项目已经深度使用 Spring 生态，Jackson 的集成度更好。

---

## 四、一张表帮你做决策

| 方案 | 开发便利性 | 查询能力 | 数据完整性 | 性能特征 | 最佳场景 |
|---|---|---|---|---|---|
| TEXT + 手动序列化 | 中（需手动调用） | 无（只能整取） | 低（靠代码保证） | 写入快、存储省 | 附属数据、跨库兼容、只整存整取 |
| 原生 JSON 类型 | 高 | 中（JSON 函数） | 中（数据库校验格式） | 查询优、写入略慢 | 需要 JSON 内部查询但不拆表 |
| ORM 自动映射 | 最高 | 取决于底层类型 | 取决于底层类型 | 取决于底层类型 | 已有 ORM，追求代码整洁 |
| 关联表 | 低（建表+JOIN） | 最强（任意 SQL） | 最高（外键约束） | JOIN 有开销 | 核心业务数据、频繁查询 |
| 文档数据库 | 高 | 中（文档内查询） | 中（弱事务） | 文档读写快、跨文档慢 | 文档型数据、模式频繁变化 |

---

## 五、选型口诀

**附属数据不查内部，TEXT 手动最省事；**

**要查 JSON 内部字段，原生 JSON 类型最合适；**

**核心关系数据，老老实实建关联表；**

**已有 ORM 嫌手写烦，TypeHandler 帮你自动化。**

---

## 六、总结

TEXT 列 + Fastjson2 手动序列化，本质是**用开发便利性换数据库规范性**的一种折中方案。它适合那些“只是跟着主对象一起存取、从不单独查询内部字段”的附属数据。

如果你的数据需要频繁筛选、排序、聚合，或者对一致性要求极高，那就应该考虑原生 JSON 类型或关联表。如果你已经在用 MyBatis-Plus、JPA，不想在手写序列化上浪费精力，TypeHandler / AttributeConverter 是更优雅的选择。

没有绝对最好的方案，只有最适合当前场景的方案。理解每种方式的边界，你就能在“建表”和“存 JSON”之间做出清醒的选择。