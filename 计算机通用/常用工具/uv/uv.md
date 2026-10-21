# uv 全套常用命令（分场景，适配你 Windows + LangChain 项目）
## 一、基础信息 & 安装卸载uv本体
```powershell
# 查看uv版本
uv --version
uv -V

# 查看全局帮助
uv --help

# Windows 安装uv（PowerShell）
irm https://astral.sh/uv/install.ps1 | iex

# 更新uv自身
uv self update

# 卸载uv
uv self uninstall
```

## 二、项目初始化（标准uv工程，生成pyproject.toml+uv.lock）
```powershell
# 在当前目录初始化uv项目
uv init

# 指定项目名新建文件夹并初始化
uv init NoteAgent

# 导入旧requirements.txt批量添加依赖
uv add -r requirements.txt
```

## 三、虚拟环境管理（uv venv）
### 1. 新建环境
```powershell
# 默认生成 ./.venv
uv venv

# 指定Python版本创建
uv venv --python 3.11

# 指定自定义环境目录（比如沿用你现有的.venv）
uv venv .\.venv
```
### 2. 环境信息 & 激活（Windows PowerShell）
```powershell
# 查看当前虚拟环境信息
uv venv info

# 手动激活（和原生venv一致）
.\.venv\Scripts\Activate.ps1

# 退出环境直接输入 deactivate
deactivate
```
### 3. 复用你现有 python -m venv 创建的 .venv
所有 `uv pip` / `uv run` 加参数 `--venv .\.venv` 即可绑定旧环境

## 四、标准项目依赖管理（uv add / remove / sync / lock，推荐工程化使用）
### 1. 添加依赖（写入pyproject.toml）
```powershell
# 安装最新版
uv add langchain

# 指定固定版本（你当前1.3.9）
uv add langchain==1.3.9

# 版本范围约束
uv add "langchain>=1.3,<1.4"

# 一次性多个包
uv add langchain langchain-openai langgraph

# 开发依赖（仅本地测试，打包不携带）
uv add --dev pytest ruff black
```
### 2. 移除依赖
```powershell
uv remove langchain-openai
uv remove --dev pytest
```
### 3. 同步环境（最核心命令）
```powershell
# 根据pyproject.toml + uv.lock 安装/删除包，对齐环境
uv sync

# 同步时不安装开发依赖（生产部署）
uv sync --no-dev

# 同步到你旧的 .venv 不新建
uv sync --venv .\.venv
```
### 4. 锁文件 & 更新依赖
```powershell
# 仅生成/更新uv.lock，不安装包
uv lock

# 升级所有依赖到最新兼容版本
uv lock --upgrade
uv sync --upgrade
```
### 5. 依赖查看 & 导出
```powershell
# 树形展示依赖树（看清langchain子依赖）
uv tree

# 导出锁文件为 requirements.txt（给pip兼容环境）
uv export --format requirements-txt > requirements.txt
```

## 五、uv pip 兼容模式（操作现有原生venv，替代pip，速度更快）
适合你现在**不想迁移工程、只想用uv加速旧.venv**的场景
```powershell
# 安装包到现有 .venv
uv pip install langchain==1.3.9 --venv .\.venv

# 批量导入requirements.txt
uv pip install -r requirements.txt --venv .\.venv

# 卸载
uv pip uninstall langchain --venv .\.venv

# 列出已安装包
uv pip list --venv .\.venv

# 导出当前环境包列表
uv pip freeze --venv .\.venv > requirements.txt

# 同步环境严格匹配requirements.txt（多余包自动删除）
uv pip sync requirements.txt --venv .\.venv
```

## 六、运行代码（uv run，无需手动激活环境）
### 1. 标准uv项目（自带.venv）
```powershell
# 运行主脚本
uv run python main.py

# 直接执行脚本简写
uv run main.py

# 运行工具命令（pytest/ruff等）
uv run pytest
uv run ruff check .
```
### 2. 绑定你旧的 python venv（你当前场景）
```powershell
# 指定旧.venv运行代码
uv run --venv .\.venv python main.py
```

## 七、Python版本管理（替代pyenv，多版本切换）
```powershell
# 列出本地已安装Python
uv python list

# 安装指定Python版本
uv python install 3.11

# 设置项目默认Python
uv python pin 3.11
```

## 八、全局工具（uv tool，全局安装CLI工具，不污染系统Python）
```powershell
# 全局安装工具包（如ruff、httpie）
uv tool install ruff

# 运行全局工具
uv tool run ruff check .

# 更新全局工具
uv tool upgrade ruff
```

## 九、缓存清理（磁盘空间不足时用）
```powershell
# 查看全局缓存大小
uv cache dir
uv cache prune --dry-run

# 清理过期缓存文件
uv cache prune

# 清空全部缓存
uv cache clean
```

# 针对你 LangChain Agent 项目高频速查（最常用几条）
1. 用旧venv跑代码：`uv run --venv .\.venv main.py`
2. 给旧venv装langchain：`uv pip install langchain==1.3.9 --venv .\.venv`
3. 迁移成uv标准工程：`uv init && uv add -r requirements.txt && uv sync`
4. 查看langchain依赖树：`uv tree`
5. 开发调试自动跑测试：`uv run pytest`