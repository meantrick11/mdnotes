# import 详解

## import的作用

python中的import语句用于导入其他模块，被导入的模块可以是内置模块、第三方模块(pip或者其他方式安装的模块)、自定义模块(自己编写的.py文件),但他们实质都是.py文件，所以也称为模块文件。
**实质都会加载整个模块文件（执行模块的顶层代码），只是最终只把指定的成员(具体的整个文件、函数、类、变量)暴露给当前命名空间(赋值给当前文件中的定义的变量)。**，比如`import math`，就是将math.py文件所有成员加载到当前空间，然后将整个math.py赋值给当前这个文件中的变量math，再比如`from math import sqrt`,就是将math.py文件中的sqrt函数加载到当前空间，然后将sqrt赋值给当前这个文件中的变量sqrt。**由于是变量赋值，所以会有变量冲突的情况，自然就有给导入的模块起别名的需求，具体语法见下文**

## import的用法

1. import + 模块名
```python
import math#python内置数学模块
```
将内置的math.py文件加载到当前空间，然后将整个math.py赋值给当前这个文件中的变量math,具体调用比如`math.sqrt(4)`,就是调用当前变量math(已经有math.py文件的所有成员)中的sqrt函数,当然类和变量也可以这样引用。

2. from + 模块名 + import + class\fuction\variable

```python
from math import sqrt#导入math.py文件中的sqrt函数
```
将math.py文件中的sqrt函数加载到当前空间，然后将sqrt赋值给当前这个文件中的变量sqrt,具体调用比如`sqrt(4)`,就是调用当前变量sqrt(已经有math.py文件的所有成员)中的sqrt函数,不过这里要补充的一点就是，from后面的'模块名'可以是具体模块名、路径等
- 具体模块名就是当前环境变量中的路径里面有的模块名，如何查看当前环境变量中的路径呢？可以通过如下代码：
>```python
>import sys 
>print(sys.path)
>```
在上面代码中，导入模块都是在当前环境变量中去依次寻找是否含有该模块，有则导入，否则就会报错。其打印结果包含当前脚本所在目录，zip压缩文件路径、标准库路径，第三方库的路径等，当然还可以修改或添加路径，临时修改`sys.path.append('路径名')`，用永久修改，如linux中`export PYTHONPATH="/my/custom/path:$PYTHONPATH"`或windows中`PYTHONPATH=C:\my\custom\path;%PYTHONPATH%` 或者直接更改配置文件
- 路径就是当前环境变量中路径名，比如`from . import test`，就是从当前路径下寻找test.py文件，如果当前路径下没有test.py文件，则会报错。最好还是使用相对路径(..或者.)来调整导入的路径,但与终端路径不同，不能使用/来切换路径。而要使用.表示当前包名：`from mypackage.test import test_func`,python会自动将.理解为对应的目录或者文件以及类或者函数，而不能直接使用./../来表示模块路径


3. from + 模块名 + import + * (导入模块中的所有函数、类、变量)
```python
from math import *#导入math.py文件中的所有函数、类、变量
```
将math.py文件中的所有函数、类、变量加载到当前空间，然后将所有函数、类、变量赋值给当前这个文件中的变量，具体调用比如`sqrt(4)`,就是调用当前变量sqrt(已经有math.py文件的所有成员)中的sqrt函数,当然类和变量也可以这样引用。

## 导入模块的顺序

当多个模块导入同一个模块时，会按照导入的顺序来执行，比如：
```python
import a
import b
```
如果a.py中导入了b.py，则会先执行b.py中的代码，再执行a.py中的代码，如果a.py中没有导入b.py，则会先执行a.py中的代码，再执行b.py中的代码。  
如果a.py中导入了b.py，b.py中又导入了c.py，则会先执行c.py中的代码，再执行b.py中的代码，再执行a.py中的代码。

*自己导入的时候最好按照内置模块>第三方模块>自定义模块的顺序来导入，这样可以避免一些变量名冲突等问题，同时也可以提高代码的可读性和维护性。*

**以下补充最重要的相对导入和绝对导入的问题**：
1. 相对导入，就是相对于当前文件夹导入比如`from . import dull`表示当前文件|`from .. import parentdull`表示从文件夹中导入parentdull模块|而`from ... import gradeparentdull`就表示从爷（父父)文件夹导入gradeparentdull模块了
2. 绝对导入，就是从项目顶部文件很简单，直接从顶部文件出发如`from gradparent.parent import child`表示从gradeparent的子文件夹中导入child模块

*这里有注意点：
在vscode\pycharm这种IDE中如果右键直接运行，它的整体运行项目根目录就是当前运行文件即是__package__变量为''，当当前文件中有导入其他模块代码时，且其他模块有导入相对模块代码时，这个时候就会报错’
比如：
有以下目录结构的项目代码
```aiignore
llm-sec-v1/
├── __pycache__/
├── llm/
│   ├── __pycache__/
│   ├── llmclient.py
│   └── routes/
│       ├── __pycache__/
│       ├── __init__.py
│       ├── apiroute.py
│       ├── commanroute.py
│       ├── security/
│       └── templates/
├── .env
├── .gitignore
├── app.py
├── config.py
├── README.md
└── requirements.txt
```

在结构中分别有如下代码
```
#app.py
from flask import Flask
from routes import register_all_routes
from config import Config
```
```
#__init__.py
from commanroute import register_routes
from apiroute import register_api_route

```

```
#apiroute.py
from flask import request,jsonify,session
from ..llm.llmclient import ai_answer
```
*简单来讲就是，app.py要导入__init__.py，而__init__.py又要导入apirout.py,apiroute.py要导入相对目录中的llmclient模块*

如果直接在IDE中点击运行，会报错，因为
当你在 VSCode / PyCharm 中点击文件旁边的绿色三角形运行 `app.py` 时，IDE 实际上执行的是类似这样的命令：

```bash
python /绝对路径/llm_sec_v1/app.py
```
Python 解释器接收到的是一个**文件路径**，而不是一个模块路径。此时：

1. Python 将 `app.py` 所在目录（`llm_sec_v1/`）加入 `sys.path`。
2. 将 `app.py` 作为 `__main__` 模块执行，**不会为它建立任何包上下文**（`__package__ = None`）。
3. 当 `app.py` 内部执行 `from routes import apiroute` 时，`apiroute.py` 被加载，其 `__package__` 被推断为空字符串。
4. `apiroute.py` 中的 `from ..llm.llmclient` 试图从空包名中“上溯一级”，直接失败。。

这时，如果将apiroute.py改为：
```aiignore
#apiroute.py
from flask import request,jsonify,session
from llm.llmclient import ai_answer
```
那么在导入的时候，使用的是绝对路径，就可以结合sys.path找到整个llm模块，进而可以导入成功

如果不修改apiroute.py,如何才能运行成功呢？
这就需要直接终端运行整个项目的主文件夹`python -m llm-sec-v1.app`,为什么添加一个-m就能成功导入？
- `-m` 告诉 Python：“不要把这个当成文件路径，而是当成一个**模块全名**来查找并执行。”
1. Python 从 `sys.path` 中查找名为 `llm_sec_v1` 的包。
2. 找到后，会沿着包层级逐层加载 `__init__.py`，并正确设置每个模块的 `__package__`。
3. 执行 `llm_sec_v1.app` 时，它的 `__package__` 被设为 `'llm_sec_v1'`。
4. 后续 `routes.apiroute` 的 `__package__` 自然就是 `'llm_sec_v1.routes'`。
5. 相对导入 `..` 顺利解析为 `'llm_sec_v1'`。


最后，总结一下：
1. 绝对导入 = 地图导航
只要目标位置在导航地图里（sys.path），从哪出发都能到。
2. 相对导入 = 家族亲戚称呼
必须知道自己在家族中的辈分（__package__），才能叫出“二叔”（..）、“堂哥”（...）。
3. 直接运行脚本 = 孤儿闯世界
脚本顶层启动 → 没爹没娘（__package__ = None）→ 家族称呼全部失效 → 相对导入直接哑火。
| 对比项 | 绝对导入 `from llm.xx import yy` | 相对导入 `from ..llm import yy` |
| :--- | :--- | :--- |
| **查找方式** | 依赖 `sys.path` | 依赖 `__package__` |
| `python app.py` 运行 | ✅ 可用 | ❌ 不可用 |
| `python -m` 模块运行 | ✅ 可用 | ✅ 可用 |

*在大型项目中，最好使用相对导入的方式，然后采用`python -m ...`的方式运行，增加代码的可移植性。*