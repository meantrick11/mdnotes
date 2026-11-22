# D2L 前置 02｜Installation 与 notebook 运行约定

> **原书：** Alexander J. Smola, *Dive into Deep Learning*, PyTorch Edition，PDF 标注日期 2026-11-11。  
> **范围：** *Installation*，书页 11–14（PDF 第 51–54 页）。  
> **关联：** 本机 WSL2、uv、PyTorch CUDA 的安装与验证见 [DL-001 环境搭建](DL-001%EF%BD%9CWindows%20%2B%20WSL2%20%2B%20uv%20%2B%20PyTorch%20CUDA%20%E6%B7%B1%E5%BA%A6%E5%AD%A6%E4%B9%A0%E7%8E%AF%E5%A2%83%E6%90%AD%E5%BB%BA.md)。  
> **原书：** [Smola - Dive into Deep Learning.pdf](books/Smola%20-%20Dive%20into%20Deep%20Learning.pdf)

## 选择与本书匹配的环境

本版提供 PyTorch、JAX、TensorFlow、MXNet 的 notebook 包；**PyTorch 是唯一覆盖全书章节的框架**。前几章用 CPU 即可，较大的卷积网络、Transformer 和生成模型才更需要 GPU。因此从 CPU 锁文件启动，是原书建议的起点。

下载的框架专属 ZIP 包含 notebook、已保存的输出、引用图片、相应的 `d2l` 源码，以及固定依赖的 CPU/GPU 环境。只阅读已保存输出不必安装框架；要执行 notebook 则需要 Python、Jupyter 和对应框架。书中另提及源码仓库，主要供查看当前源码和构建工具；单纯跟读时，ZIP 包更直接。

## 从 ZIP 到可运行 notebook

以下是原书的**包内运行流程**，命令须在解压后的框架目录中执行，该目录应包含 `pyproject.toml`、`d2l/` 和锁文件。它不是要求重建已有的 WSL 开发环境。

```powershell
uv --version
uv venv --python 3.12
uv pip sync pylock.cpu.toml
uv run --no-sync jupyter lab
```

其中 `uv venv` 创建隔离环境；`uv pip sync` 使已安装包与指定锁文件一致；`--no-sync` 防止随后启动 Jupyter 时 `uv run` 改动刚按 CPU/GPU 锁文件同步好的环境。**不要在已有项目目录随手执行 `uv pip sync`**：它按锁文件同步包，适合本书专用虚拟环境。

需要原书所给 GPU 环境时，先在 Linux x86-64、NVIDIA GPU 的目标机器上确认驱动可见，然后在解压目录使用：

```bash
nvidia-smi
uv venv --python 3.12
uv pip sync pylock.gpu.toml
uv run --no-sync jupyter lab
```

原书的 GPU 锁文件针对 **Linux x86-64 + NVIDIA GPU**，PyTorch 栈使用 CUDA 12.8 wheel。CPU/GPU 锁文件可同步进同一虚拟环境，切换时相关包会被替换；若硬件、驱动或操作系统不匹配，应以对应框架当前安装说明为准。已有本机环境是否兼容，还须以 notebook 实际运行结果检验；`DL-001` 的 GPU 可用性结果不自动证明它等同于本书锁定环境。

原书给出的 PyTorch 检查方式：

```bash
uv run --no-sync python -c "import torch; print(torch.__version__); print(torch.cuda.is_available())"
```

CPU 环境输出 `False` 是正常结果。GPU 环境若输出 `False`，再检查驱动、设备与安装包是否匹配。

## 标准导入与隐藏的运行上下文

每个下载的 PyTorch notebook 都有标准导入单元；PDF 只在本节展示一次：

```python
import math
import numpy as np
import torch
from torch import nn
from torch.nn import functional as F
from d2l import torch as d2l
```

`math` 用于标量函数；`np` 是 CPU 上的 NumPy 数组；`torch` 是 PyTorch；`nn` 多提供层和损失函数等类；`F` 多提供相应函数形式；`d2l` 是**本书自己的库**。不同框架的 `d2l` 在导入时也统一取别名 `d2l`。所以直接复制 PDF 中后续章节的一段代码，可能因缺少标准导入或包内 `d2l` 而报错。

## 数据与磁盘空间

ZIP 包有 notebook、已保存输出和图片，但**不包含全部数据集及预训练模型**。数据依赖的单元首次执行时可能下载并缓存资源，需要网络和额外磁盘空间。阅读已有输出与亲自重跑实验是两种不同需求；后续大模型例子还可能超出笔记本的时间或内存预算。

## 复习自测

1. `uv run --no-sync` 在原书流程中保护了什么？
2. CPU 环境中 `torch.cuda.is_available()` 返回 `False` 说明安装失败吗？
3. 为什么仅拿到 PDF，而未下载 ZIP 包时，后续代码不一定可以直接粘贴运行？
