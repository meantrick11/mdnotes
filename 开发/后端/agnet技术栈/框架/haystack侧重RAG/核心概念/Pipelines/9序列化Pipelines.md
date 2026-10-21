# 序列化Pipelines

序列化指的是将Pipelines转换为一种可以保存到磁盘并在之后加载的格式。(广义的应该是将一些代码类转为可以传输或者存储等等的固化格式，比如python中的字典，序列化后应该为text的形式存储到磁盘中，或者序列化为JSON格式方便网络传输)

**Haystack 支持将 YAML 格式用于管道序列化。**


这里举例说明一下YAML&JSON的区别，同样的一份Python内容，不同序列化之后是：
```yaml
user:
  name: "张同学"
  age: 22
  tags:
    - rag
    - langgraph
  enable: true
```

```json
{
  "user": {
    "name": "张同学",
    "age": 22,
    "tags": ["rag", "langgraph"],
    "enable": true
  }
}
```

## 将Pipeline转换为YAML

使用 dumps() 方法将 Pipeline 对象转换为 YAML 格式：
```python
from haystack import Pipeline

pipe = Pipeline()
print(pipe.dumps())
# >> components: {}
# >> connections: []
# >> max_runs_per_component: 100
# >> metadata: {}
```

你也可以使用dump()方法将管道的YAML表示形式保存到文件中：

```python
with open("/content/test.yml", "w") as file:
    pipe.dump(file)
```


## 将Pipeline转回为Python

你可以将 YAML 管道转换回 Python。使用 loads() 方法可转换管道的字符串表示形式（str、bytes 或 bytearray），或使用 load() 方法将类文件对象中表示的管道转换为对应的 Python 对象。

两种加载方式均支持回调函数，可让你在反序列化过程中修改组件。反序列化受可信模块白名单限制，因此引用白名单之外类别的管道将无法加载，除非你扩展白名单。


```python
from haystack import Pipeline
from haystack.core.serialization import DeserializationCallbacks
from typing import Type, Dict, Any

# This is the YAML you want to convert to Python:
pipeline_yaml = """
components:
  cleaner:
    init_parameters:
      remove_empty_lines: true
      remove_extra_whitespaces: true
      remove_regex: null
      remove_repeated_substrings: false
      remove_substrings: null
    type: haystack.components.preprocessors.document_cleaner.DocumentCleaner
  converter:
    init_parameters:
      encoding: utf-8
    type: haystack.components.converters.txt.TextFileToDocument
connections:
- receiver: cleaner.documents
  sender: converter.documents
max_runs_per_component: 100
metadata: {}
"""


def component_pre_init_callback(
    component_name: str,
    component_cls: Type,
    init_params: Dict[str, Any],
):
    # This function gets called every time a component is deserialized.
    if component_name == "cleaner":
        assert "DocumentCleaner" in component_cls.__name__
        # Modify the init parameters. The modified parameters are passed to
        # the init method of the component during deserialization.
        init_params["remove_empty_lines"] = False
        print("Modified 'remove_empty_lines' to False in 'cleaner' component")
    else:
        print(f"Not modifying component {component_name} of class {component_cls}")


pipe = Pipeline.loads(
    pipeline_yaml,
    callbacks=DeserializationCallbacks(component_pre_init_callback),
)
```

## 反序列化安全

加载管道会实例化序列化数据中引用的类。为了防止特制的 YAML 文件导入并实例化任意类，Pipeline.load、Pipeline.loads 和 Pipeline.from_dict 拒绝从受信任模块白名单之外的模块导入类，否则会引发 DeserializationError。

默认情况下，白名单包含haystack、haystack_integrations、haystack_experimental、builtins、typing和collections。尽管builtins在白名单中，但像eval、exec、compile、__import__、open和getattr这类危险的内置函数仍被阻止使用。


## 自定义模块

在将其他包中的自定义组件或可调用对象作为引用的管道，只有将这些模块添加到允许列表后才能加载。你可以通过三种方式扩展该列表：

```py
from haystack import Pipeline

# 1. Per call: pass additional module patterns for this deserialization only
pipe = Pipeline.load(open("pipeline.yaml"), allowed_modules=["mypkg.*"])

# 2. Process-wide: extend the allowlist programmatically
from haystack.core.serialization import allow_deserialization_module

allow_deserialization_module("mypkg")
```
默认情况下，模式会作为前缀进行匹配（"mypkg" 会匹配 mypkg 及其所有子模块）；如果模式中除了末尾的 .* 外还包含 *、? 或 [，则会按 fnmatch 通配符模式匹配。末尾的 .* 会被视为前缀匹配，因此 "mypkg" 和 "mypkg.*" 的行为完全相同。

**如果序列化数据的来源完全可信，你可以使用 unsafe=True 完全绕过白名单：**

`pipe = Pipeline.load(open("pipeline.yaml"), unsafe=True)`

**只有在你完全信任序列化管道的来源时，才使用 `unsafe=True`——它还会解除对危险内置函数的限制。**

## 嵌套初始化参数验证

作为额外的安全措施，反序列化会在递归进入任何嵌套的{"type": "...", "init_parameters": {...}}字典之前，根据类的__init__签名验证init_parameters的键。如果嵌套字典的键不是可接受的参数名，会在导入嵌套类型之前通过DeserializationError予以拒绝，这能阻止将不可信的类偷偷传入未使用的参数槽的行为。构造函数包含**kwargs的类则不受此限制，因为其可接受的参数集无法静态确定。

此验证可能会暴露 YAML 文件中已存在的错误——例如拼写错误、重命名或删除参数后遗留的内容，或是旧版 Haystack 版本的无效快照。解决方法是更新 YAML 文件，使每个嵌套组件键都与父类的真实 __init__ 参数相匹配。


## 默认序列化行为

序列化系统使用 default_to_dict 和 default_from_dict 自动处理许多对象类型。通常情况下，你无需为以下内容实现自定义的 to_dict/from_dict：
- 密钥：自动序列化和反序列化，以避免敏感值以明文形式存储。
- 组件设备：设备配置已被自动检测并恢复。
- 拥有自身 to_dict/from_dict 方法的对象：任何类型定义了 to_dict() 方法的初始化参数都会通过调用该方法进行序列化；init_parameters中任何带有 type 键且该键指向拥有 from_dict() 方法的类的字典，都会自动进行反序列化。

要序列化或反序列化单个组件，你可以使用 component_to_dict 和 component_from_dict，它们来自 haystack.core.serialization。当组件未定义自定义的 to_dict/from_dict 时，它们会回退使用上述默认行为：

```python
from haystack import component
from haystack.core.serialization import component_from_dict, component_to_dict


@component
class Greeter:
    def __init__(self, message: str = "Hello"):
        self.message = message

    @component.output_types(greeting=str)
    def run(self, name: str):
        return {"greeting": f"{self.message}, {name}!"}


# Serialize a component instance to a dictionary
greeter = Greeter(message="Hi")
data = component_to_dict(greeter, "my_greeter")

# Deserialize back to a component instance
restored = component_from_dict(Greeter, data, "my_greeter")
assert restored.message == greeter.message
```

## 执行自定义序列化

Haystack 中的管道和组件可直接序列化简单组件（包括自定义组件）。如下代码即可直接运行：

```py
from haystack import component


@component
class RepeatWordComponent:
    def __init__(self, times: int):
        self.times = times

    @component.output_types(result=str)
    def run(self, word: str):
        return word * self.times
```

另一方面，如果最终格式为 JSON，这段代码就无法正常运行，因为set类型无法进行 JSON 序列化：

```python
from haystack import component


@component
class SetIntersector:
    def __init__(self, intersect_with: set):
        self.intersect_with = intersect_with

    @component.output_types(result=set)
    def run(self, data: set):
        return data.intersection(self.intersect_with)
```

在这种情况下，你可以为组件提供自定义的 from_dict 和 to_dict 实现：

```python
from haystack import component, default_from_dict, default_to_dict


class SetIntersector:
    def __init__(self, intersect_with: set):
        self.intersect_with = intersect_with

    @component.output_types(result=set)
    def run(self, data: set):
        return data.intersect(self.intersect_with)

    def to_dict(self):
        return default_to_dict(self, intersect_with=list(self.intersect_with))

    @classmethod
    def from_dict(cls, data):
        # convert the set into a list for the dict representation,
        # so it can be converted to JSON
        data["intersect_with"] = set(data["intersect_with"])
        return default_from_dict(cls, data)
```

## 将管道保存为自定义格式

一旦以字典格式提供了管道，序列化的最后一步就是将该字典转换为可存储或可通过网络传输的格式。Haystack 原生支持 YAML，但若需要其他格式，你可以编写自定义的 Marshaller。

Marshaller 是一种 Python 类，负责按照特定格式将文本转换为字典，以及将字典转换为文本。Marshaller 必须遵守协议，并提供 marshal 和 unmarshal 方法。

这是一段自定义 TOML 序列化器的代码，它依赖于 rtoml 库：

```python
# This code requires a `pip install rtoml`
from typing import Dict, Any, Union
import rtoml


class TomlMarshaller:
    def marshal(self, dict_: Dict[str, Any]) -> str:
        return rtoml.dumps(dict_)

    def unmarshal(self, data_: Union[str, bytes]) -> Dict[str, Any]:
        return dict(rtoml.loads(data_))
```

然后你可以将 Marshaller 实例传递给 dump、dumps、load 和 loads 方法：

```python
from haystack import Pipeline
from my_custom_marshallers import TomlMarshaller

pipe = Pipeline()
pipe.dumps(TomlMarshaller())
# >> 'max_runs_per_component = 100\nconnections = []\n\n[metadata]\n\n[components]\n'
```