# Go 语言类型转换

类型转换用于将一种数据类型的变量转换为另外一类型的变量

`type_name(expression)`

## 数值类型转换

```go
var a int = 10
var b float64 = float64(a)
```

## 字符串相关的转换

使用strconv库

## 字符串类型转换

```go
var str string = "10"
var num int
num, _ = strconv.Atoi(str)
```

上述代码将字符串变量str转为整形变量num，

注意，strconv.Atoi 函数返回两个值，第一个是转换后的整型值，第二个是可能发生的错误，我们可以使用空白标识符 _ 来忽略这个错误

### 整形转字符串
```go
    num := 123
    str := strconv.Itoa(num)
```

### 字符串转为浮点数
```go

str := "3.14"
num, err := strconv.ParseFloat(str, 64)
```


### 浮点数转为字符串

```go
num := 3.14
str := strconv.FormatFloat(num, 'f', 2, 64)
```

## 接口类型的转换

#### 类型断言
类型断言用于将接口类型转换为指定类型:
```go
value.(type) 
或者 
value.(T)
```
其中 value 是接口类型的变量，type 或 T 是要转换成的类型。

如果类型断言成功，它将返回转换后的值和一个布尔值，表示转换是否成功。

比如：
```go
package main

import "fmt"

func main() {
    var i interface{} = "Hello, World"
    str, ok := i.(string)
    if ok {
        fmt.Printf("'%s' is a string\n", str)
    } else {
        fmt.Println("conversion failed")
    }
}
```

#### 类型转换

类型转换用于将一个接口类型的值转换为另一个接口类型，语法如下：
```go
T(value)
```

T 是目标接口类型，value 是要转换的值。

在类型转换中，我们必须保证要转换的值和目标接口类型之间是兼容的，否则编译器会报错。

```go
package main

import "fmt"

// 定义一个接口 Writer
type Writer interface {
    Write([]byte) (int, error)
}

// 实现 Writer 接口的结构体 StringWriter
type StringWriter struct {
    str string
}

// 实现 Write 方法
func (sw *StringWriter) Write(data []byte) (int, error) {
    sw.str += string(data)
    return len(data), nil
}

func main() {
    // 创建一个 StringWriter 实例并赋值给 Writer 接口变量
    var w Writer = &StringWriter{}
    
    // 将 Writer 接口类型转换为 StringWriter 类型
    sw := w.(*StringWriter)
    
    // 修改 StringWriter 的字段
    sw.str = "Hello, World"
    
    // 打印 StringWriter 的字段值
    fmt.Println(sw.str)
}
```

### 空接口类型

空接口 interface{} 可以持有任何类型的值。在实际应用中，空接口经常被用来处理多种类型的值。

```go
package main

import (
    "fmt"
)

func printValue(v interface{}) {
    switch v := v.(type) {
    case int:
        fmt.Println("Integer:", v)
    case string:
        fmt.Println("String:", v)
    default:
        fmt.Println("Unknown type")
    }
}

func main() {
    printValue(42)
    printValue("hello")
    printValue(3.14)
}
```