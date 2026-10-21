# JSON  编码器和解码器

## 什么是JSON?
**JSON = JavaScript Object Notation（JavaScript 对象表示法）**
它是一种**轻量级、纯文本、与编程语言无关的数据交换格式**。
- 人阅读、手写简单直观；
- 机器可以快速解析、生成；
- 语法取自 JavaScript 的 子集，但**不依赖 JavaScript**，Java/Python/Go/C++ 几乎所有语言都支持；
- 文件后缀：`.json`，网络传输类型：`application/json`。

JSON只有两种基础结构：
1. **对象 `{}`**：键值对集合（字典），`"key":value`，键必须是双引号字符串
2. **数组 `[]`**：有序的值列表

可存放的值类型：字符串、数字、`true`/`false`、`null`、对象、数组。

最简示例：
```json
{
  "name": "张三",
  "age": 20,
  "hobby": ["coding", "game"]
}
```

###  二、JSON 的作用
1. **前后端接口通信（最核心用途）**
浏览器/App发请求，服务器返回结构化数据，使用JSON传输（RESTful API标准格式）。
2. **配置文件**
项目配置、软件设置用 `.json` 保存（VSCode、npm、前端工程配置）。
3. **数据持久化/简单存储**
保存少量结构化数据，用于本地缓存。
4. **不同系统、不同编程语言之间交换数据**
一套格式，多语言互通，充当“通用数据翻译器”。

对比 XML：JSON体积更小、语法简洁、解析更快，是互联网主流数据格式。
### 相关链接
1. [JSON 原始官网（英文原版）](https://www.json.org/)
2. [JSON 中文官网](https://www.json.org.cn/)
3. [IETF 正式标准文档 RFC8259（互联网规范）](https://datatracker.ietf.org/doc/html/rfc8259)
4. [ECMA 国际标准 ECMA‑404](https://ecma-international.org/publications-and-standards/standards/ecma-404/)

**警告 在解析来自不受信任来源的 JSON 数据时要小心谨慎。 恶意的 JSON 字符串可能导致解码器消耗大量 CPU 和内存资源。 建议对要解析的数据大小进行限制。（因为有可能造成反序列化漏洞，有兴趣的读者可以去了解一下,研究安全也是一个不错的职业方向嗷~）**


JSON模块提供了和标准库marshal&pickle模块类似的操作，其中marshal 是给 Python 解释器自己用的底层序列化；pickle 是给程序员日常保存 Python 对象用的序列化工具。

>备注 JSON 是 YAML 1.2 的一个子集。 由该模块的默认设置所产生的 JSON（尤其是默认的 separators 值）也是 YAML 1.0 和 1.1 的一个子集。 因此该模块也能被用作 YAML 序列化器。
>YAML = YAML Ain’t Markup Language（不是标记语言）一种面向配置文件的人类可读序列化格式，缩进式语法，不用写大量引号、大括号，可读性极强。
>常用场景：1.配置文件首选：Docker‑Compose、K8s、GitHub Actions、Ansible、CI/CD 流水线、项目配置 2.保存结构化数据（字典、列表、嵌套对象）3.跨语言通用：Python / Go / Java / JS 都可以读写4.相比 JSON：支持注释、多行文本、日期、布尔简写、引用复用，写配置更舒服
>其他结构化模块:文本类:JSON(网络接口),YAML(配置领域王者),TOML,INI,XML(很少使用),CSV(表格)。二进制机器高效解析类:MessagePack(JSON二进制版本，小/快),Protocol Buffers(高性能RPC通信),Avro/Thrift(大数据/微服务),pickie/marshal(Python解析)

## 基础使用

### 序列化：

1. json.dump(obj, fp, *, skipkeys=False, ensure_ascii=True, check_circular=True, allow_nan=True, cls=None, indent=None, separators=None, default=None, sort_keys=False, **kw)
使用这个 Python 至 JSON 转换表 将 obj 序列化为输出到 fp (一个支持 .write() 的 file-like object) 的已格式化的 JSON 流。

备注 与 pickle 和 marshal 不同，JSON 不是一个具有框架的协议，所以尝试多次使用同一个 fp 调用 dump() 来序列化多个对象会产生一个不合规的 JSON 文件。
  参数:
  - obj (object) -- 要序列化的 Python 对象。

  - fp (file-like object) -- obj 将被序列化输出到的文件型对象。 json 模块总是产生 str 对象，而不是 bytes 对象，因此 fp.write() 必须支持 str 输入。

  - skipkeys (bool) -- 如为 True，则不为基本类型 (str, int, float, bool, None) 的键将被跳过而不会引发 TypeError。 默认为 False。

  - ensure_ascii (bool) -- 如果为 True (默认值)，输出将确保对所有输入的非 ASCII 和不可打印字符进行转义。 如果为 False，则所有字符将被原样输出，只有必须被转义的字符除外：引号、反斜线，以及 U+0000 至 U+001F 范围的控制字符。

  - check_circular (bool) -- 如为 False，则对容器类型的循环引用检查会被跳过并且循环引用将导致 RecursionError (或更糟的情况)。 默认为 True。

  - allow_nan (bool) -- 如为 False，则对超范围的 float 值 (nan, inf, -inf) 进行序列化将导致 ValueError，以严格遵循 JSON 规范。 如为 True (默认值)，则将使用它们的 JavaScript 等价形式 (NaN, Infinity, -Infinity)。

  - cls (a JSONEncoder subclass) -- 如果设置，则使用一个重写了 default() 方法的自定义 JSON 编码器，用以序列化为自定义的数据类型。 如为 None (默认值)，则使用 JSONEncoder。

  - indent (int | str | None) -- If a positive integer or string, JSON array elements and object members will be pretty-printed with that indent level. A positive integer indents that many spaces per level; a string (such as "\t") is used to indent each level. If zero, negative, or "" (the empty string), only newlines are inserted. If None (the default), no newlines are inserted.

  - separators (tuple | None) -- 一个二元组: (item_separator, key_separator)。 如为 None (默认值)，则默认当 indent 为 None 时 separators 为 (', ', ': ')，否则为 (',', ': ')。 要使用最紧凑形式的 JSON，可指定 (',', ':') 来去除空格。

  - default (callable | None) -- 当对象无法被序列化时将被调用的函数。 它应该返回一个可被 JSON 编码的版本或是引发 TypeError。 如为 None (默认值)，则会引发 TypeError。

  - sort_keys (bool) -- 如为 True，则字典输出将按键排序。 默认为 False。


#### 2. json.dumps(obj, *, skipkeys=False, ensure_ascii=True, check_circular=True, allow_nan=True, cls=None, indent=None, separators=None, default=None, sort_keys=False, **kw)
使用这个 转换表 将 obj 序列化为 JSON 格式的 str。 其参数的含义与 dump() 中的相同。

**备注 JSON 中的键-值对中的键永远是 str 类型的。当一个对象被转化为 JSON 时，字典中所有的键都会被强制转换为字符串。这所造成的结果是字典被转换为 JSON 然后转换回字典时可能和原来的不相等。换句话说，如果 x 具有非字符串的键，则有 loads(dumps(x)) != x。**


### 反序列化

3. json.load(fp, *, cls=None, object_hook=None, parse_float=None, parse_int=None, parse_constant=None, object_pairs_hook=None, **kw)
  使用 JSON 至 Python 转换表 将 fp 反序列化为 Python 对象。

  参数:
  - fp (file-like object) -- 一个包含要执行反序列化的 JSON 文档的支持 .read() 的 text file 或 binary file。

  - cls (a JSONDecoder subclass) -- 如果设定此参数，则使用自定义的JSON解码器。任何传递给 load() 方法的多余关键字参数都会被传递给 cls 的构造器。如果传入 None （默认值），则会使用 JSONDecoder 。

  - object_hook (callable | None) -- 如果设置，将是一个在调用时传入任意被解码的 JSON 对象字面值 (即 dict) 的函数。 该函数的返回值将代替该 dict 被使用。 此特性可被用于实现自定义解码器，例如 JSON-RPC 类提示。 默认值为 None。

  - object_pairs_hook (callable | None) -- 如果设置，将是一个在调用时传入以对照值的有序列表进行解码的任意 JSON 对象字面值的函数。 该函数的返回值将代替该 dict 被使用。 此特性可被用于实现自定义解码器。 如果还设置了 object_hook，则 object_pairs_hook 的优先级更高。 默认值为 None。

  - parse_float (callable | None) -- 如果设置，将是一个在调用时以代表每个要解码的 JSON 浮点数的字符串作为参数的函数。 如为 None (默认值)，则它等价于 float(num_str)。 这可被用于将 JSON 浮点数解析为自定义数据类型，例如 decimal.Decimal。

  - parse_int (callable | None) -- 如果设置，将是一个在调用时以代表每个要解码的 JSON 整数的字符串作为参数的函数。 如为 None (默认值)，则它等价于 int(num_str)。 这可被用于将 JSON 整数解析为自定义数据类型，例如 float。

  - parse_constant (callable | None) -- 如果设置，将是一个以下列字符串之一作为参数的函数: '-Infinity', 'Infinity' 或 'NaN'。 这可被用于在遇到无效的 JSON 数字时引发异常。 默认值为 None。

  抛出:
  - JSONDecodeError -- 当被反序列化的数据不是合法的 JSON 文档。

  - UnicodeDecodeError -- 当被反序列化的数据不包含 UTF-8, UTF-16 或 UTF-32 编码的数据。



#### 4. json.loads(s, *, cls=None, object_hook=None, parse_float=None, parse_int=None, parse_constant=None, object_pairs_hook=None, **kw)
类似于 load()，但不是针对文件型对象，而是使用这个 转换表 将 s (一个包含 JSON 的 str, bytes 或 bytearray 实例) 反序列化为 Python 对象。


其中2&4是用来处理python对象<->JSON字符串之间的转换的，1&3可以用来文件相关的操作，比如
```python
with open("data.json", "w", encoding="utf-8") as f:
    json.dump(data, f, ensure_ascii=False, indent=4)
```
不过可以先f.read()再json.loads()，json.dumps()再json.write()对直接使用json.load()和json.dump()进行替代。


**由于顶层json包直接暴露.loads()&.dumps()函数，其底层需要编码器和解码器，但是正常开发使用这两个函数已经能够覆盖绝大多数场景了**

## 编码器和解码器

1. *class* json.JSONDecoder(\*, object_hook=None, parse_float=None, parse_int=None, parse_constant=None, strict=True, object_pairs_hook=None)

简单的JSON解码器。

默认情况下，解码执行以下转换:

| JSON | Python |
|---|---|
| object -- 对象 | dict |
| array | list -- 列表 |
| string | str |
| number (int) | int |
| number (real) | float |
| true | True |
| false | False |
| null | None |

它还将 `NaN`、`Infinity` 和 `-Infinity` 理解为它们对应的 `float` 值，这超出了 JSON 规范。

*object_hook* 是一个可选的函数，它被调用时将传入每个要解码的 JSON 对象而其返回值将被用来替代给定的 `dict`。 它可被用于提供自定义的反序列化操作 (比如支持 JSON‑RPC 类提示)。

*object_pairs_hook* 是一个可选的函数，它被调用时将传入每个以一个对照值的有序列表进行解码的 JSON 对象的结果。 *object_pairs_hook* 的返回值将被用于代替原本的 `dict`。 此特性可被用于实现自定义的解码器。 如果还定义了 *object_hook*，则 *object_pairs_hook* 的优先级更高。

*parse_float* 是一个可选的函数，它被调用时将传入每个要解码的 JSON 浮点数。 默认情况下，这相当于 `float(num_str)`。 它可被用于 JSON 浮点数使用其他类型或解析器的情况 (比如 `decimal.Decimal`)。

*parse_int* 是一个可选的函数，它被调用时将传入每个要解码的 JSON 整数。 默认情况下，这相当于 `int(num_str)`。 它可被用于 JSON 整数使用其他数据类型或解析器的情况 (比如 `float`)。

*parse_constant* 是一个可选的函数，它被调用时将传入以下字符串之一: `'-Infinity'`, `'Infinity'`, `'NaN'`。 它可被用于当遇到无效的 JSON 数字时引发一个异常。

如果 *strict* 为 false （默认为 `True` ），那么控制字符将被允许在字符串内。在此上下文中的控制字符编码在范围 0--31 内的字符，包括 `'\t'` (制表符）， `'\n'` ， `'\r'` 和 `'\0'` 。

如果反序列化的数据不是有效 JSON 文档，引发 `JSONDecodeError` 错误。

decode(*s*)

返回 *s* 的 Python 表示形式（包含一个 JSON 文档的 `str` 实例）。

如果给定的 JSON 文档无效则将引发 `JSONDecodeError`。

raw_decode(*s*)

从 *s* 中解码出 JSON 文档（以 JSON 文档开头的一个 `str` 对象）并返回一个 Python 表示形式为 2 元组以及指明该文档在 *s* 中结束位置的序号。

这可以用于从一个字符串解码JSON文档，该字符串的末尾可能有无关的数据。


2. *class* json.JSONEncoder(\*, skipkeys=False, ensure_ascii=True, check_circular=True, allow_nan=True, sort_keys=False, indent=None, separators=None, default=None)

用于Python数据结构的可扩展JSON编码器。

默认支持以下对象和类型：

| Python | JSON |
|---|---|
| dict | object -- 对象 |
| list, tuple | array |
| str | string |
| int, float, int 和 float 派生的枚举 | number |
| True | true |
| False | false |
| None | null |

要将其扩展至识别其他对象，需要子类化并实现 `default()`，如果可能还要实现另一个返回 `o` 的可序列化对象的方法，否则它应当调用超类实现 (来引发 `TypeError`)。

如果 *skipkeys* 为假值（默认），则当尝试对不为 `str`, `int`, `float`, `bool` 或 `None` 的键进行编码时将会引发 `TypeError`。 如果 *skipkeys* 为真值，这样的条目将被直接跳过。

如果 *ensure_ascii* 为真值（默认值），输出将确保对所有输入的非 ASCII 和不可打印字符进行转义。 如果 *ensure_ascii* 为假值，则所有字符将被原样输出，只有必须被转义的字符除外：引号、反斜线，以及 U+0000 至 U+001F 范围的控制字符。

如果 *check_circular* 为真值（默认），那么列表、字典和自定义的已编码对象将在编码期间进行循环引用检查以防止无限递归 (无限递归会导致 `RecursionError`)。 在其他情况下，将不会进行这种检查。

如果 *allow_nan* 为真值（默认），那么 `NaN`、`Infinity` 和 `-Infinity` 将照常进行编码。此行为不符合 JSON 规范，但与大多数基于 JavaScript 的编码器和解码器一致。否则，编码这些浮点数将引发 `ValueError`。

如果 *sort_keys* 为真值 (默认为 `False`)，那么字典的输出将按照键排序；这对回归测试很有用，以确保可以在日常基础上对 JSON 序列化结果进行比较。

如果 *indent* 是一个非负整数或者字符串，那么 JSON 数组元素和对象成员会被美化输出为该值指定的缩进等级。 如果缩进等级为零、负数或者 `""`，则只会添加换行符。 `None` (默认值) 选择最紧凑的表达。 使用一个正整数会让每一层缩进同样数量的空格。 如果 *indent* 是一个字符串 (比如 `"\t"`)，那个字符串会被用于缩进每一层。

当被指定时，*separators* 应当是一个 `(item_separator, key_separator)` 元组。当 *indent* 为 `None` 时，默认值取 `(', ', ':')`，否则取 `(',', ': ')`。为了得到最紧凑的 JSON 表达式，你应该指定其为 `(',', ':')` 以消除空白字符。

当 *default* 被指定时，其应该是一个函数，每当某个对象无法被序列化时它会被调用。它应该返回该对象的一个可以被 JSON 编码的版本或者引发一个 `TypeError`。如果没有被指定，则会直接引发 `TypeError`。

default(*o*)

在子类中实现这种方法使其返回 *o* 的可序列化对象，或者调用基础实现（引发 `TypeError`）。

例如，要支持任意的迭代器，你可以这样实现 `default()`:
```python
def default(self, o):
    try:
        iterable = iter(o)
    except TypeError:
        pass
    else:
        return list(iterable)
    # 让基类的 default 方法引发 TypeError
    return super().default(o)
```

encode(*o*)

返回 Python *o* 数据结构的 JSON 字符串表达方式。例如:
```python
>>> json.JSONEncoder().encode({"foo": ["bar", "baz"]}) '{"foo": ["bar", "baz"]}'
```

iterencode(*o*)

编码给定对象 *o*，并产出每个可用的字符串表达方式。例如:
```python
for chunk in json.JSONEncoder().iterencode(bigobject):
    mysocket.write(chunk)
```


## 异常 
exception json.JSONDecodeError(msg, doc, pos)
拥有以下附加属性的 ValueError 的子类：

msg
未格式化的错误消息。

doc
正在解析的 JSON 文档。

pos
The start index of doc where parsing failed.

lineno
The line corresponding to pos.

colno
The column corresponding to pos.


## 字符编码
RFC 要求使用 UTF-8 ， UTF-16 ，或 UTF-32 之一来表示 JSON ，为了最大互通性推荐使用 UTF-8 。

正如 RFC 所允许（但不要求）的那样，此模块的序列化器默认设置 ensure_ascii=True，从而对输出进行转义，使结果字符串只包含可打印的 ASCII 字符。

除了 ensure_ascii 参数以外，此模块严格按照 Python 对象和 Unicode 字符串 之间的转换来定义，因此不会直接处理字符编码的问题。

RFC 禁止在 JSON 文本的开头添加字节顺序标记（BOM），此模块的序列化器不会在其输出中添加 BOM。RFC 允许但不要求 JSON 反序列化器忽略其输入中的初始 BOM。此模块的反序列化器在遇到初始 BOM 时会引发 ValueError。

RFC 并未明确禁止包含不对应有效 Unicode 字符的字节序列的 JSON 字符串（比如不成对的 UTF-16 代理项），但它确实指出这可能会导致互操作性问题。默认情况下，此模块接受并输出此类序列的代码点（当它们存在于原始 str 中时）。


## Infinite 和 NaN 数值
RFC 不允许表示无穷大或 NaN 数值。尽管如此，默认情况下，此模块接受并输出 Infinity、-Infinity 和 NaN，如同它们是有效的 JSON 数字字面值:
```python
# 这些调用均不会引发异常，但结果都不是合法的 JSON
json.dumps(float('-inf'))
#'-Infinity'
json.dumps(float('nan'))
#'NaN'
# 当反序列化时也一样
json.loads('-Infinity')
#-inf
json.loads('NaN')
#nan

```
序列化器中， allow_nan 参数可用于替代这个行为。反序列化器中， parse_constant 参数，可用于替代这个行为。

## 对象中的重复名称
RFC 规定 JSON 对象内的名称应当是唯一的，但没有规定如何处理 JSON 对象中的重复名称。默认情况下，此模块不会引发异常；而是对于给定名称忽略除最后一个名称-值对之外的所有对:
```python
weird_json = '{"x": 1, "x": 2, "x": 3}'
json.loads(weird_json)
#{'x': 3}
```
## 命令行选项

*可以使用`python -m json [infile] [outfile] [可选参数]`进行json模块的参数调用，不过都不是很常用，了解即可*

**infile**
要被验证或美化打印的 JSON 文件。
如果未指定 infile，则从 sys.stdin 读取。

**outfile**
将 infile 输出写入到给定的 outfile。 在其他情况下，将写入到 sys.stdout。

**--sort-keys**
将字典输出按照键的字母顺序排序。

**--no-ensure-ascii**
禁用非 ASCII 字符的转义，详情参见 json.dumps()。

**--json-lines**
将每个输入行解析为单独的 JSON 对象。

**--indent, --tab, --no-indent, --compact**
用于空白符控制的互斥选项。

**-h, --help**
显示帮助消息。

### 使用示例

####  基础美化文件
python -m json mp_films.json

#### 输出保存到文件
python -m json mp_films.json out.json

#### 按键排序格式化
python -m json --sort-keys mp_films.json

#### 保留中文不转义
echo '{"name":"张三"}' | python -m json --no-ensure-ascii

#### 指定2空格缩进
python -m json --indent 2 mp_films.json

#### tab缩进
python -m json --tab mp_films.json

#### 仅换行，无缩进
python -m json --no-indent mp_films.json

#### 压缩单行json
python -m json --compact mp_films.json

#### 解析逐行JSON
python -m json --json-lines data.json

#### 组合参数
python -m json --sort-keys --no-ensure-ascii mp_films.json target.json

#### 查看帮助
python -m json -h

## 总结

到此，恭喜你完成了JSON模块的学习，主要记住序列化使用dumps()方法，而反序列化使用loads()方法即可，其他方法、类、参数等等都不是很常用，接下来会是开发中常用的时间相关datetime模块的学习。


