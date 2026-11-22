# DL-001｜深度学习开发环境搭建：WSL2、uv、PyTorch CUDA 与 VS Code

> **学习日期：** 2026 年 11 月 11 日  
> **所属阶段：** 深度学习学习路线 · 第一阶段  
> **环境：** Windows + WSL2 Ubuntu 24.04 + Python 3.12 + uv + PyTorch CUDA  
> **目标：** 搭建可用于深度学习训练、模型微调和后续工程开发的 Linux 开发环境。

---

# 1. 环境搭建目标与整体架构

## 1.1 为什么需要这套环境？

后续的深度学习学习不仅涉及理论，还需要进行：

- Tensor 运算与自动微分
    
- 神经网络模型训练
    
- CNN、RNN、Transformer 实现
    
- 小型 GPT 模型训练
    
- Hugging Face 模型加载与微调
    
- LoRA / PEFT 微调
    
- GPU 计算、显存管理与模型推理
    
- 模型服务与 API 部署
    

因此需要一套相对稳定、可复现，并且能够使用 NVIDIA GPU 的 Python 开发环境。

本次采用以下技术组合：

|技术|核心职责|
|---|---|
|Windows|日常操作系统和图形界面|
|WSL2|在 Windows 上提供 Linux 运行环境|
|Ubuntu 24.04|Linux 开发与程序执行环境|
|VS Code|代码编辑、调试和 Notebook 操作|
|VS Code WSL 扩展|连接 Windows 编辑器与 WSL|
|VS Code Server|在 WSL 中提供远程开发支持|
|uv|管理 Python 版本、依赖、虚拟环境|
|PyTorch|深度学习框架|
|CUDA|NVIDIA GPU 加速计算生态|
|Jupyter / ipykernel|Notebook 交互式运行环境|

## 1.2 整体开发架构

```
Windows 11
│
├── VS Code
│   ├── 图形界面
│   ├── 代码编辑
│   └── 调试、Notebook 界面
│
└── WSL2
    │
    └── Ubuntu 24.04
        │
        ├── VS Code Server
        │
        ├── 开发工具
        │   ├── Git
        │   ├── uv
        │   └── Python 3.12
        │
        └── ~/projects/deep-learning/
            │
            ├── .venv/
            │   ├── PyTorch
            │   ├── CUDA 运行时依赖
            │   ├── NumPy
            │   ├── Matplotlib
            │   └── Jupyter
            │
            ├── pyproject.toml
            ├── uv.lock
            └── src/
```

核心思想：

**Windows 负责开发界面，WSL 负责 Linux 开发环境与代码执行，uv 负责 Python 项目管理，PyTorch 负责深度学习计算。**

---

# 2. 本机硬件与软件配置

## 2.1 硬件信息

|组件|配置|
|---|---|
|CPU|Intel Core i7-14650HX|
|RAM|32 GB|
|GPU|NVIDIA GeForce RTX 4060 Laptop GPU|
|VRAM|8 GiB|
|硬盘|NVMe SSD|
|NVIDIA Windows 驱动|616.56|

## 2.2 最终软件环境

|组件|版本或状态|
|---|---|
|WSL|WSL2|
|Ubuntu|24.04|
|Python|3.12|
|uv|0.13.7|
|PyTorch|2.11.0+cu128|
|PyTorch CUDA Runtime|12.8|
|NVIDIA 驱动报告的 CUDA UMD|13.4|
|ipykernel|7.4.0|
|VS Code WSL 连接|成功|
|GPU 自动微分|成功|

这些版本是本次配置时的实际环境记录，不代表未来安装其他项目时必须使用完全相同的版本。

---

# 3. WSL2：Linux 开发环境

## 3.1 什么是 WSL？

WSL 全称 Windows Subsystem for Linux。

WSL2 通过轻量虚拟化方式，在 Windows 上提供真正的 Linux 内核环境。

这样即使主机运行 Windows，也能够使用：

- Linux 文件系统
    
- Bash Shell
    
- Linux 软件包管理器
    
- Linux Python 和编译工具
    
- Linux 开发与调试工具
    
- NVIDIA GPU 加速功能
    

WSL2 并不等于把 Windows 变成 Linux，而是提供一个可以与 Windows 协作的 Linux 环境。

## 3.2 WSL 常用命令

以下命令在 Windows PowerShell 中执行。

**查看 WSL 状态：**

```
wsl --status
```

用于查看 WSL 的基础状态和默认配置。

**查看已安装的 Linux 发行版：**

```
wsl -l -v
```

参数解释：

- `-l`：列出发行版。
    
- `-v`：显示详细版本信息。
    

其中 `VERSION = 2` 表示使用 WSL2。

**查看可安装的发行版：**

```
wsl --list --online
```

**安装 Ubuntu 24.04：**

```
wsl --install -d Ubuntu-24.04
```

`-d` 表示指定发行版。

本机已经存在 Ubuntu，不需要再次安装。

**进入 Ubuntu：**

```
wsl -d Ubuntu
```

**停止所有 WSL 实例：**

```
wsl --shutdown
```

用于完整停止 WSL 虚拟机。它不会删除 Ubuntu 文件或项目，但会中断正在运行的 Linux 程序。

## 3.3 WSL 文件系统和磁盘位置

WSL2 的 Linux 文件系统通常存放在一个虚拟磁盘文件中：

```
ext4.vhdx
```

本机通过 Windows 注册表确认 Ubuntu 的存储位置为：

```
D:\孔庶\Data\WSL\Ubuntu
```

因此，在 Ubuntu 的 Linux 文件系统中安装的软件、Python 依赖及项目文件，主要占用 D 盘空间。

**查询 WSL 发行版存储位置：**

在 PowerShell 中运行：

```
reg query "HKCU\Software\Microsoft\Windows\CurrentVersion\Lxss" /s
```

其中：

- `reg query`：查询 Windows 注册表。
    
- `/s`：递归查询所有子项。
    
- `DistributionName`：Linux 发行版名称。
    
- `BasePath`：发行版存储位置。
    
- `VhdFileName`：虚拟磁盘文件名。
    

需要区分 Linux 路径与 Windows 磁盘映射：

|Linux 路径|含义|
|---|---|
|`/home/musk`|Linux 用户主目录|
|`~/projects`|用户项目目录|
|`/mnt/c`|Windows C 盘|
|`/mnt/d`|Windows D 盘|
|`/usr`|Linux 系统软件目录|
|`/var`|系统运行数据与缓存等|
|`~/.cache`|当前 Linux 用户缓存目录|

特别注意：

```
cd /mnt/c/Users/lenovo
```

表示进入 Windows C 盘映射目录。

而：

```
cd /home/musk
```

表示进入 Ubuntu 自己的文件系统。

虽然 Ubuntu 虚拟磁盘放在 D 盘，但访问 `/mnt/c` 依然会使用 C 盘。

## 3.4 Linux 常用文件命令

```
pwd
```

Print Working Directory，输出当前工作目录。

```
ls
```

列出当前目录的文件。

```
ls -la
```

显示详细文件信息，包括隐藏文件。

```
cd ~
```

返回当前 Linux 用户的 Home 目录。

```
mkdir -p ~/projects/deep-learning
```

创建项目目录。

参数 `-p` 允许自动创建缺失的父目录，目录已经存在时通常不会报错。

```
df -h ~
```

查看该路径所在文件系统的空间使用情况。

```
du -sh ~/projects/deep-learning
```

查看项目目录占用空间。

参数：

- `-s`：汇总大小。
    
- `-h`：使用便于阅读的容量单位。
    

需要注意，WSL 的虚拟文件系统容量信息与 Windows D 盘真实剩余容量不一定完全一致。

---

# 4. Linux 软件包管理：apt

Ubuntu 使用 `apt` 管理系统软件。

它主要负责 Linux 层面的软件安装，而不是单个 Python 项目的依赖隔离。

## 4.1 更新软件包索引

```
sudo apt update
```

解释：

- `sudo`：使用管理员权限。
    
- `apt`：Ubuntu 软件包管理工具。
    
- `update`：更新可安装软件包的索引信息。
    

这里的 `update` 不代表升级所有已安装的软件。

## 4.2 安装基础开发工具

```
sudo apt install -y \
  python3 \
  python3-pip \
  python3-venv \
  git \
  build-essential \
  curl \
  ca-certificates
```

其中：

|软件包|功能|
|---|---|
|python3|Ubuntu 系统 Python|
|python3-pip|Python 包安装工具|
|python3-venv|Python 标准虚拟环境工具|
|git|版本控制|
|build-essential|常用 C/C++ 编译工具|
|curl|HTTP 下载与请求工具|
|ca-certificates|HTTPS 证书验证支持|

命令中的：

```
\
```

表示 Shell 命令续行。

它让一条长命令分多行书写，方便阅读。

参数 `-y` 表示自动同意安装确认。

## 4.3 apt 与 uv 的职责划分

**apt：管理 Linux 系统软件。**

**uv：管理 Python 版本、虚拟环境与项目依赖。**

未来使用时遵循这条原则，避免把 Python 项目依赖大量安装进系统 Python。

---

# 5. uv：Python 项目和依赖管理

## 5.1 为什么使用 uv？

传统 Python 开发可能同时使用：

- pyenv：Python 版本管理
    
- venv：虚拟环境
    
- pip：依赖安装
    
- pip-tools：依赖锁定
    

uv 可以统一承担其中多项职责。

它是基于 Rust 开发的 Python 项目与包管理工具，通常具有较快的依赖解析和缓存复用能力。

## 5.2 安装 uv

```
curl -LsSf https://astral.sh/uv/install.sh | sh
```

命令拆解：

- `curl`：通过 HTTP(S) 获取内容。
    
- `-L`：跟随重定向。
    
- `-s`：静默模式。
    
- `-S`：发生错误时显示错误信息。
    
- `-f`：HTTP 请求失败时返回错误。
    
- `| sh`：把获取的安装脚本交给 Shell 执行。
    

这是官方提供的安装方式。执行远程安装脚本时应确认来源可信。

安装后加载环境配置：

```
source ~/.local/bin/env
```

`source` 会在当前 Shell 中执行指定文件，使新增 PATH 等环境设置立即生效。

验证：

```
uv --version
which uv
```

其中：

```
which uv
```

用于查看当前 Shell 使用的 uv 可执行文件路径。

本机实际版本：

```
uv 0.13.7
```

## 5.3 使用 uv 管理 Python

安装 Python 3.12：

```
uv python install 3.12
```

查看已安装 Python：

```
uv python list --only-installed
```

这样可以让 uv 管理 Python，而不必修改系统默认 Python。

## 5.4 初始化项目

创建目录：

```
mkdir -p ~/projects/deep-learning
cd ~/projects/deep-learning
```

初始化：

```
uv init --name deep-learning --python 3.12
```

解释：

- `uv init`：初始化项目。
    
- `--name`：指定项目名称。
    
- `--python`：指定 Python 版本要求。
    

固定本地 Python：

```
uv python pin 3.12
```

同步环境：

```
uv sync
```

`uv sync` 会按照项目配置及锁文件同步依赖，并创建或更新项目虚拟环境。

## 5.5 项目结构

本机使用的是包含 `src/` 的项目结构：

```
deep-learning/
├── .venv/
├── .python-version
├── pyproject.toml
├── uv.lock
├── README.md
└── src/
    └── deep_learning/
        └── __init__.py
```

其中：

|文件或目录|功能|
|---|---|
|`.venv/`|Python 虚拟环境|
|`.python-version`|本地 Python 版本选择|
|`pyproject.toml`|项目元数据及依赖声明|
|`uv.lock`|锁定依赖版本与来源|
|`src/`|项目 Python 源代码|
|`README.md`|项目说明|

注意：`pyproject.toml` 声明项目依赖，`uv.lock` 则保存解析后的具体依赖版本和来源，方便之后复现。

## 5.6 虚拟环境有什么用？

不同 Python 项目可能依赖不同版本的第三方库。

虚拟环境用于隔离依赖，降低相互冲突的风险。

例如：

```
projects/
├── deep-learning/
│   └── .venv/
│       └── torch
│
└── ai-agent/
    └── .venv/
        └── transformers
```

两个项目可以独立管理依赖。

uv 运行项目命令时不需要每次手动激活 `.venv`：

```
uv run python main.py
```

它会使用当前项目的虚拟环境运行代码。

## 5.7 uv 依赖与缓存

安装第三方 Python 包：

```
uv add numpy
```

删除依赖：

```
uv remove numpy
```

同步环境：

```
uv sync
```

查看依赖关系：

```
uv tree
```

查看缓存位置：

```
uv cache dir
```

uv 通常先将下载的包放在共享缓存中，再将依赖同步到项目虚拟环境。

本机常见路径：

```
/home/musk/.cache/uv
/home/musk/projects/deep-learning/.venv
```

两者的区别：

- `~/.cache/uv`：uv 用户级缓存，可跨项目复用。
    
- `.venv`：当前项目使用的独立 Python 环境。
    

uv 在条件允许时可以使用硬链接等机制复用缓存文件，因此不能简单把缓存和虚拟环境的显示大小相加作为真实磁盘占用。

---

# 6. PyTorch 与 NVIDIA CUDA

## 6.1 PyTorch 是什么？

PyTorch 是深度学习框架，提供：

- 张量与矩阵运算
    
- 自动微分
    
- 神经网络模块
    
- 损失函数
    
- 参数优化器
    
- GPU 加速计算
    
- 模型保存与加载
    
- 数据集与数据加载工具
    

后续学习的模型都将主要基于 PyTorch 实现。

## 6.2 PyTorch 和 CUDA 的关系

基本运行结构：

```
Python 模型代码
       ↓
PyTorch
       ↓
CUDA 计算库
       ↓
NVIDIA 驱动
       ↓
RTX 4060 GPU
```

重要 CUDA 组件包括：

|组件|作用|
|---|---|
|CUDA Runtime|CUDA 程序运行支持|
|cuBLAS|GPU 矩阵乘法及线性代数计算|
|cuDNN|深度学习高性能计算|
|cuFFT|快速傅里叶变换|
|cuRAND|GPU 随机数生成|
|cuSPARSE|GPU 稀疏矩阵计算|
|NCCL|多 GPU 通信|

这些 GPU 计算库使 PyTorch 能够利用 NVIDIA GPU 的高性能运算能力。

本次主要使用 PyTorch 官方预编译 CUDA 包，无需单独安装完整 CUDA Toolkit。

## 6.3 安装 PyTorch CUDA

本次使用：

```
uv add torch torchvision \
  --index pytorch-cu128=https://download.pytorch.org/whl/cu128
```

解释：

- `torch`：PyTorch 核心库。
    
- `torchvision`：图像数据集、预处理和视觉模型工具。
    
- `--index`：指定 uv 使用的软件包索引。
    
- `pytorch-cu128`：指定索引名称。
    
- `cu128`：CUDA 12.8 版本的 PyTorch 软件包索引。
    

安装完成后，uv 会将依赖记录在项目配置中。

当项目已经有 `pyproject.toml` 和 `uv.lock` 时，通常只需执行：

```
uv sync
```

即可同步环境，不必重新初始化项目。

## 6.4 CUDA Driver 与 Runtime 的区别

本机：

```
NVIDIA 驱动报告：CUDA 13.4
PyTorch Runtime：CUDA 12.8
```

这是正常的版本组合。

**CUDA Driver** 负责 GPU 驱动层支持。

**CUDA Runtime** 提供程序运行时需要的 CUDA 功能。

PyTorch 使用与其预编译构建相匹配的 CUDA 运行库；驱动不要求与 PyTorch Runtime 版本完全一致，只需要满足兼容性要求。

## 6.5 验证 CUDA

```
uv run python - <<'PY'
import torch

print("PyTorch:", torch.__version__)
print("CUDA Runtime:", torch.version.cuda)
print("CUDA Available:", torch.cuda.is_available())

if torch.cuda.is_available():
    print("GPU:", torch.cuda.get_device_name(0))
PY
```

本机实际结果：

```
PyTorch: 2.11.0+cu128
CUDA Runtime: 12.8
CUDA Available: True
GPU: NVIDIA GeForce RTX 4060 Laptop GPU
```

说明 PyTorch 能够识别并使用 CUDA。

## 6.6 GPU 前向计算和自动微分测试

```
import torch

device = "cuda"

x = torch.randn(
    1000,
    1000,
    device=device,
    requires_grad=True
)

# 前向计算
y = x @ x
loss = y.square().mean()

# 反向自动微分
loss.backward()

print("Device:", x.device)
print("Loss:", loss.item())
print("Gradient shape:", x.grad.shape)
print("Gradient calculated:", x.grad is not None)
```

实际结果：

```
Device: cuda:0
Loss: 1005.6356201171875
Gradient shape: torch.Size([1000, 1000])
Gradient calculated: True
```

### 代码解释

`**torch.randn(1000, 1000)**`

创建一个 1000 × 1000 的随机张量，元素来自标准正态分布。

`**device="cuda"**`

将张量创建在 CUDA GPU 上，而不是 CPU 内存中。

`**requires_grad=True**`

开启自动微分跟踪。

`**x @ x**`

进行二维矩阵乘法。

`**y.square().mean()**`

对矩阵元素平方，再计算平均值，得到一个标量 Loss。

`**loss.backward()**`

根据计算图，通过链式法则计算 Loss 关于张量 `x` 的梯度。

`**x.grad**`

保存计算得到的梯度。

这个测试已经验证了 GPU 计算与自动微分能力。

但它还不是完整的模型训练过程，因为尚未涉及神经网络可训练参数、优化器和参数更新。

---

# 7. VS Code 与 WSL 远程开发

## 7.1 为什么使用 Windows VS Code 开发 Linux 项目？

VS Code 支持将图形编辑界面与代码执行环境分离。

Windows 上运行 VS Code 图形界面，WSL 中运行 VS Code Server 和 Linux 开发工具。

这样不需要在 Windows 和 Linux 中分别安装一套 PyTorch 环境。

## 7.2 VS Code Remote WSL 架构

```
Windows
│
└── VS Code
    │
    │ WSL 远程连接
    ▼
WSL Ubuntu
│
├── VS Code Server
├── Git
├── uv
├── Python
├── PyTorch
└── deep-learning 项目
```

编辑器显示在 Windows 中，但项目代码、终端、Python 解释器和训练程序都运行在 WSL 环境中。

## 7.3 安装 WSL 扩展

在 Windows VS Code 的扩展市场搜索：

`WSL`

选择 Microsoft 官方发布的 WSL 扩展并安装。

它支持 VS Code 连接 WSL Linux 环境。

## 7.4 从 WSL 打开 VS Code

在 Ubuntu 终端执行：

```
cd ~/projects/deep-learning
code .
```

命令说明：

- `cd`：切换到目标项目目录。
    
- `code`：调用 VS Code 命令行入口。
    
- `.`：表示当前工作目录。
    

首次连接时，VS Code 会在 Ubuntu 内下载安装 VS Code Server。

其常见位置为：

```
~/.vscode-server/
```

由于 Ubuntu 存储在 D 盘，因此该 Server 也主要占用 D 盘对应的 Linux 文件系统空间。

## 7.5 VS Code 扩展也有环境之分

VS Code 可以在 Windows 本地和 WSL 环境中安装不同的扩展。

例如 Python 扩展需要安装到 WSL 中，才能完整支持 WSL 中的 Python 开发。

典型操作：

1. VS Code 左下角确认 `WSL: Ubuntu`。
    
2. 打开扩展市场。
    
3. 搜索 Microsoft Python 扩展。
    
4. 选择 `Install in WSL: Ubuntu`。
    
5. 打开 Python 文件。
    

## 7.6 选择项目 Python 解释器

打开命令面板：

`Ctrl + Shift + P`

执行：

`Python: Select Interpreter`

选择：

```
/home/musk/projects/deep-learning/.venv/bin/python
```

这样 VS Code 的 Python 运行和调试操作就可以使用当前项目的虚拟环境。

## 7.7 实际运行验证

```
import sys
import torch

print("Python:", sys.executable)
print("PyTorch:", torch.__version__)
print("CUDA:", torch.cuda.is_available())
```

本机实际结果：

```
Python: /home/musk/projects/deep-learning/.venv/bin/python
PyTorch: 2.11.0+cu128
CUDA: True
```

说明 VS Code 已成功使用 WSL 中的项目环境。

## 7.8 Windows 与 WSL 项目切换

VS Code 不会永久绑定 WSL。

可以同时打开两个窗口：

- Windows 本地窗口：开发 Windows 项目。
    
- WSL 远程窗口：开发 Linux 项目。
    

Windows 项目可以通过 `File → Open Folder` 打开。

WSL 项目可以通过 `code .` 或远程连接菜单打开。

未来也可以使用 Remote SSH 连接远程 Linux GPU 服务器。

---

# 8. Jupyter Notebook 与 Kernel

## 8.1 为什么学习深度学习要使用 Notebook？

Notebook 适合交互式实验。

例如：

- 运行一小段 Tensor 代码。
    
- 检查张量形状。
    
- 绘制函数曲线。
    
- 显示模型训练结果。
    
- 可视化 Loss 与 Accuracy。
    
- 在代码单元格之间加入 Markdown 笔记。
    

它非常适合前期学习和探索。

后期正式工程项目，则需要逐步掌握 `.py` 模块化代码组织。

## 8.2 Notebook 和 Kernel 的区别

**Notebook** 是包含代码、文字和输出的交互式文档。

**Kernel** 是实际执行 Notebook 代码的进程。

VS Code 负责显示 Notebook，Jupyter 扩展负责连接和管理 Kernel，Kernel 通过 Python 解释器执行代码。

```
VS Code Notebook
       ↓
Jupyter 扩展
       ↓
Python Kernel
       ↓
项目 .venv
       ↓
PyTorch / CUDA
```

## 8.3 安装 Jupyter 开发工具

```
uv add --dev jupyterlab ipykernel
```

解释：

- `--dev`：将依赖添加为开发依赖。
    
- `jupyterlab`：Notebook 开发和交互界面。
    
- `ipykernel`：让 Python 环境能够作为 Jupyter Kernel 运行。
    

可以将 Jupyter 等工具划分为开发依赖，因为正式部署模型服务时不一定需要它们。

## 8.4 验证 ipykernel

```
uv run python -c "import ipykernel; print(ipykernel.__version__)"
```

本机实际输出：

```
7.4.0
```

## 8.5 注册 Jupyter Kernel

```
uv run python -m ipykernel install \
  --user \
  --name deep-learning \
  --display-name "Python 3.12 (Deep Learning)"
```

逐项解释：

`**uv run python**`

使用项目虚拟环境中的 Python 运行命令。

`**-m ipykernel**`

以 Python 模块方式运行 `ipykernel`。

`**install**`

安装并注册 Kernel 的配置。

`**--user**`

注册到当前 Linux 用户的 Jupyter 配置目录，不需要系统级权限。

`**--name deep-learning**`

Kernel 的内部名称。

`**--display-name**`

显示在 VS Code 或 Jupyter 中的可读名称。

本机实际结果：

```
Installed kernelspec deep-learning in
/home/musk/.local/share/jupyter/kernels/deep-learning
```

它注册的是现有 Python 虚拟环境的 Kernel，不会重新安装一套 Python。

## 8.6 查看 Jupyter Kernel

```
uv run jupyter kernelspec list
```

该命令列出 Jupyter 能够找到的 Kernel 配置。

## 8.7 在 VS Code 中使用 Notebook

打开 `.ipynb` 文件，点击右上角：

`Select Kernel`

然后从以下位置选择：

- `Python 环境`
    
- `Jupyter Kernel`
    

本机使用项目虚拟环境：

```
/home/musk/projects/deep-learning/.venv/bin/python
```

也可以选择已注册的：

```
Python 3.12 (Deep Learning)
```

如果 VS Code 没有及时发现新增 Kernel，可以通过命令面板执行：

`Developer: Reload Window`

重新加载窗口后再选择。

## 8.8 Notebook 验证代码

```
import sys
import torch
import numpy as np
import matplotlib.pyplot as plt

print("Python:", sys.executable)
print("CUDA:", torch.cuda.is_available())

x = np.linspace(-5, 5, 100)
y = x ** 2

plt.plot(x, y)
plt.title("Deep Learning Environment Test")
plt.show()
```

该代码用于检查：

- Notebook 是否能运行 Python。
    
- Kernel 是否使用项目虚拟环境。
    
- PyTorch 是否能访问 CUDA。
    
- NumPy 与 Matplotlib 是否正常。
    
- 图像是否能够显示在 Notebook 中。
    

---

# 9. 科学计算与辅助工具

基础工具安装命令：

```
uv add numpy pandas matplotlib scikit-learn tqdm
```

各依赖用途：

|依赖|主要作用|
|---|---|
|NumPy|多维数组和数值计算|
|Pandas|表格数据读取和处理|
|Matplotlib|曲线与图表可视化|
|scikit-learn|传统机器学习与评估工具|
|tqdm|训练与数据处理进度条|

部分工具不是 PyTorch 的强制依赖，但在深度学习实验、数据分析和模型评估中会经常使用。

---

# 10. 日常开发常用命令速查

## 10.1 启动项目

打开 Ubuntu，运行：

```
cd ~/projects/deep-learning
code .
```

这将使用 VS Code 打开 WSL 项目。

## 10.2 运行 Python 脚本

```
uv run python main.py
```

或者指定具体文件：

```
uv run python src/example.py
```

## 10.3 启动 JupyterLab

```
uv run jupyter lab --no-browser
```

该命令启动 Jupyter 服务，但不自动启动浏览器。

可以使用终端显示的本地 URL，在 Windows 浏览器中访问。

如果在 VS Code 内使用 Notebook，也可以直接选择内核并运行，不一定需要手动启动 JupyterLab 服务。

## 10.4 安装新的 Python 依赖

```
uv add 包名
```

例如：

```
uv add transformers
```

后续学习 Hugging Face 时再执行，不需要提前安装。

## 10.5 同步项目依赖

```
uv sync
```

适合从 Git 拉取项目后，根据锁文件恢复依赖环境。

## 10.6 查看当前 Python

```
uv run python -c "import sys; print(sys.executable)"
```

## 10.7 查看 GPU

```
nvidia-smi
```

## 10.8 查看 PyTorch GPU 状态

```
uv run python -c "import torch; print(torch.cuda.is_available())"
```

## 10.9 查看 uv 缓存

```
uv cache dir
```

## 10.10 查看项目依赖

```
uv tree
```

---

# 11. 核心工程概念总结

## 11.1 编辑器与运行环境可以分离

VS Code 图形界面在 Windows 中，不代表代码一定在 Windows 中执行。

通过 WSL 扩展，可以让编辑器连接 Linux 开发环境。

这也是远程 Linux 服务器开发的重要基础。

## 11.2 系统 Python 与项目 Python 应当区分

系统 Python 为 Ubuntu 或系统工具提供运行支持。

项目 Python 环境由 uv 管理，项目依赖应尽量安装到 `.venv` 中。

## 11.3 CUDA 驱动与运行库不是同一个东西

GPU 驱动提供 GPU 访问能力，PyTorch 使用相应 CUDA 运行时与计算库执行 GPU 操作。

不需要为了匹配 `nvidia-smi` 显示的版本而盲目安装完整 CUDA Toolkit。

## 11.4 GPU 可用不等于模型训练已经完成

GPU 能执行矩阵运算和自动微分，只说明训练环境具备基础能力。

完整训练还包括：

```
准备数据
   ↓
构建模型
   ↓
前向传播
   ↓
计算 Loss
   ↓
反向传播
   ↓
优化器更新参数
   ↓
验证和保存模型
```

这将是后续 PyTorch 学习的核心内容。

## 11.5 依赖隔离与项目复现

`pyproject.toml` 记录项目依赖需求。

`uv.lock` 记录解析后的具体版本和来源。

`.venv` 是项目实际使用的运行环境。

uv Cache 则负责跨项目缓存和复用包。

它们共同构成可复现 Python 工程环境的基础。

---

# 12. 本次环境搭建成果

目前已经实际验证：

- WSL2 Ubuntu 24.04 正常运行。
    
- Ubuntu 的主要 Linux 文件系统位于 D 盘。
    
- uv 已可管理 Python 3.12 项目环境。
    
- PyTorch 2.11.0+cu128 能识别 RTX 4060。
    
- GPU 矩阵运算成功。
    
- PyTorch 自动微分成功。
    
- Windows VS Code 成功连接 WSL。
    
- VS Code 已使用项目 `.venv` 运行 Python。
    
- Jupyter Kernel 已成功注册，并完成内核选择配置。
    

环境搭建阶段的主要目的已经实现，后续除非实际学习需要，不应持续增加不必要的软件依赖。

---

# 13. 后续学习安排

下一篇学习笔记：

**DL-002｜PyTorch Tensor 与数据操作**

对应教材：

《动手学深度学习》2.1 数据操作

主要内容：

1. Tensor 的概念与数据类型。
    
2. Tensor 的维度、Shape 和 Size。
    
3. Tensor 创建与索引。
    
4. Reshape 与广播机制。
    
5. 矩阵运算与维度变化。
    
6. Tensor 在 CPU 和 GPU 之间的管理。
    
7. 独立编程实验和知识验收。
    

后续继续学习：

**DL-003｜微积分、计算图与自动微分**

最终逐步连接到神经网络、优化器、Transformer 与模型微调工程。

---

# 参考资料

- WSL 官方文档：[https://learn.microsoft.com/windows/wsl/](https://learn.microsoft.com/windows/wsl/)
    
- VS Code WSL 开发：[https://code.visualstudio.com/docs/remote/wsl](https://code.visualstudio.com/docs/remote/wsl)
    
- uv 官方文档：[https://docs.astral.sh/uv/](https://docs.astral.sh/uv/)
    
- uv + PyTorch：[https://docs.astral.sh/uv/guides/integration/pytorch/](https://docs.astral.sh/uv/guides/integration/pytorch/)
    
- PyTorch 官方文档：[https://docs.pytorch.org/](https://docs.pytorch.org/)
    
- Jupyter 官方文档：[https://docs.jupyter.org/](https://docs.jupyter.org/)
    
- 《动手学深度学习》中文版：[https://zh.d2l.ai/](https://zh.d2l.ai/)