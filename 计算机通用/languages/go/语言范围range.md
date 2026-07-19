# Go 语言范围(range)

Go 语言中 range 关键字用于 for 循环中迭代数组(array)、切片(slice)、通道(channel)或集合(map)的元素。在数组和切片中它返回元素的索引和索引对应的值，在集合中返回 key-value 对。

for 循环的 range 格式可以对 slice、map、数组、字符串等进行迭代循环。格式如下：
```go
for key, value := range oldMap {
    newMap[key] = value
}
```
以上代码中的 key 和 value 是可以省略。

如果只想读取 key，格式如下：

`for key := range oldMap`
或者这样：

`for key, _ := range oldMap`

如果只想读取 value，格式如下：

`for _, value := range oldMap`

## 数组和切片

返回索引和具体的value

```go
package main

import "fmt"

// 声明一个包含 2 的幂次方的切片
var pow = []int{1, 2, 4, 8, 16, 32, 64, 128}

func main() {
   // 遍历 pow 切片，i 是索引，v 是值
   for i, v := range pow {
      // 打印 2 的 i 次方等于 v
      fmt.Printf("2**%d = %d\n", i, v)
   }
}
```

## 字符串

return key & value

```go
package main

import "fmt"

func main() {
    for i, c := range "hello" {
        fmt.Printf("index: %d, char: %c\n", i, c)
    }
}
```

## 映射

同样是key&value
```go
package main

import "fmt"

func main() {
    // 创建一个空的 map，key 是 int 类型，value 是 float32 类型
    map1 := make(map[int]float32)
    
    // 向 map1 中添加 key-value 对
    map1[1] = 1.0
    map1[2] = 2.0
    map1[3] = 3.0
    map1[4] = 4.0
   
    // 遍历 map1，读取 key 和 value
    for key, value := range map1 {
        // 打印 key 和 value
        fmt.Printf("key is: %d - value is: %f\n", key, value)
    }

    // 遍历 map1，只读取 key
    for key := range map1 {
        // 打印 key
        fmt.Printf("key is: %d\n", key)
    }

    // 遍历 map1，只读取 value
    for _, value := range map1 {
        // 打印 value
        fmt.Printf("value is: %f\n", value)
    }
}
```

## 通道

range遍历从通道接收的值，知道通道关闭

```go
package main

import "fmt"

func main() {
    ch := make(chan int, 2)
    ch <- 1
    ch <- 2
    close(ch)
    
    for v := range ch {
        fmt.Println(v)
    }
}
```

### 忽略值

虽然被赋予值，但是不需要使用(可以省略)

```go
package main

import "fmt"

func main() {
    nums := []int{2, 3, 4}
    
    // 忽略索引
    for _, num := range nums {
        fmt.Println("value:", num)
    }
    
    // 忽略值
    for i := range nums {
        fmt.Println("index:", i)
    }
}
```