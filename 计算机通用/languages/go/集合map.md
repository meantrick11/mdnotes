# Go 语言Map(集合)

Map 是一种无序的键值对的集合。

Map 最重要的一点是通过 key 来快速检索数据，key 类似于索引，指向数据的值。

Map 是一种集合，所以我们可以像迭代数组和切片那样迭代它。不过，Map 是无序的，遍历 Map 时返回的键值对的顺序是不确定的。

在获取 Map 的值时，如果键不存在，返回该类型的零值，例如 int 类型的零值是 0，string 类型的零值是 ""。

Map 是引用类型，如果将一个 Map 传递给一个函数或赋值给另一个变量，它们都指向同一个底层数据结构，因此对 Map 的修改会影响到所有引用它的变量。

**类似于Python的字典，但是Map有键值的具体的类型要求**

## 定义Map

### 使用字面量

#### 先定义再赋值

```go
// 1. 声明
var person map[string]string

// 2. 必须 make 初始化！
person = make(map[string]string)

// 3. 现在才能赋值 ✅
person["name"] = "小明"
```
#### 直接赋值，底层自动初始化

```go
// 使用字面量创建 Map
m := map[string]int{
    "apple": 1,
    "banana": 2,
    "orange": 3,
}
```
#### 使用make创建，同方法一


可以使用内建函数 make 或使用 map 关键字来定义 Map:

/* 使用 make 函数 */
map_variable := make(map[KeyType]ValueType, initialCapacity)
其中 KeyType 是键的类型，ValueType 是值的类型，initialCapacity 是可选的参数，用于指定 Map 的初始容量。Map 的容量是指 Map 中可以保存的键值对的数量，当 Map 中的键值对数量达到容量时，Map 会自动扩容。如果不指定 initialCapacity，Go 语言会根据实际情况选择一个合适的值。

```go
// 创建一个空的 Map
m := make(map[string]int)

// 创建一个初始容量为 10 的 Map
m := make(map[string]int, 10)
```

### 获取元素

```go
// 获取键值对
v1 := m["apple"]
v2, ok := m["pear"]  // 如果键不存在，ok 的值为 false，v2 的值为该类型的零值
```

### 修改元素

修改键值对
`m["apple"] = 5`

### 获取map的长度

`length := len(m)`

### 遍历Map(使用range)

```go
// 遍历 Map
for k, v := range m {
    fmt.Printf("key=%s, value=%d\n", k, v)
}
```
### 删除元素

// 删除键值对
`delete(m, "banana")`


