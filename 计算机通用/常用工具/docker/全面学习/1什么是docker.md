# 什么是Docker?

Docker 是一款用于开发、交付和运行应用程序的开源平台。Docker 能让你将应用程序与基础设施分离开来，从而快速交付软件。借助 Docker，你可以用管理应用程序的方式来管理基础设施。利用 Docker 的代码交付、测试和部署方法，你能大幅缩短编写代码与在生产环境中运行代码之间的延迟。

## Docker 平台

Docker 支持将应用程序打包并运行在一种名为容器的松散隔离环境中。这种隔离性与安全性让你可以在同一台主机上同时运行多个容器。容器轻量且包含运行应用程序所需的全部内容，因此无需依赖主机上已安装的组件。你可以在工作中共享容器，并且能确保与你共享的所有人都获得功能完全一致的容器。

## Docker的运行平台

Docker（Linux 容器）不是普通软件，它深度依赖 Linux 内核能力：namespaces、cgroups、overlayfs 等内核特性。

1. **Linux系统：**宿主机本身就是 Linux 内核，直接安装 docker‑engine（dockerd 守护进程 + docker cli），容器就是宿主机内核上一组隔离进程，不需要虚拟机。

2. **Widows系统：**Windows：借助 WSL2 / Hyper‑V，自动拉起一个隐藏的轻量 Linux 虚拟机（WSL 发行版docker‑desktop），Docker Engine (dockerd) 真正运行在这个 Linux 虚拟机内部。如果不下载Docker Desktop，也可以在WSL2
中进行操作。

3. **macOS系统:**macOS：借助 HyperKit / Apple Virtualization Framework，后台跑一个微型 Linux 虚拟机，dockerd 跑在虚拟机里。

*同时提供：Windows/macOS 本地的docker.exe/docker客户端、端口转发、文件共享、网络代理、GUI 面板、compose、k8s 支持，把虚拟机的 API 暴露给本机命令行，你敲docker run，实际是远程调用虚拟机里面的 dockerd 服务。*

## Docker的优势

### 快速且稳定
Docker 让开发者能够通过本地容器在标准化环境中开展工作，这些容器为应用程序和服务提供支持，从而简化了开发生命周期。容器非常适用于持续集成和持续交付（CI/CD）工作流程。
### 部署简单且易扩展

Docker 基于容器的平台支持高度可移植的工作负载。Docker 容器可在开发者的本地笔记本电脑、数据中心的物理机或虚拟机、云服务提供商的服务器上运行，也可在混合环境中运行。

### 同一硬件可多实例

Docker 轻量且快速。它为基于管理程序的虚拟机提供了一种可行、高性价比的替代方案，让你能更充分地利用服务器容量来实现业务目标。Docker 非常适用于高密度环境，以及需要以更少资源完成更多任务的中小型部署场景。


## Docker架构

Docker 采用客户端-服务器架构。Docker 客户端与 Docker 守护进程进行通信，而守护进程负责构建、运行和分发 Docker 容器的核心工作。Docker 客户端和守护进程可以在同一系统上运行，也可以将 Docker 客户端连接到远程 Docker 守护进程。二者通过 REST API 进行通信，通信渠道可以是 UNIX 套接字或网络接口。另一个 Docker 客户端是 Docker Compose，它可用于处理由一组容器组成的应用程序。

### Docker 守护进程

Docker 守护进程（dockerd）监听 Docker API 请求并管理 Docker 对象，例如镜像、容器、网络和数据卷。一个守护进程还可以与其他守护进程通信以管理 Docker 服务。

### Docker 客户端

Docker 客户端（docker）是许多 Docker 用户与 Docker 交互的主要方式。当你使用 docker run 等命令时，客户端会将这些命令发送给 dockerd，由后者执行。docker 命令使用 Docker API。Docker 客户端可以与多个守护进程通信。

### Docker 桌面版

Docker Desktop 是一款适用于你的 Mac、Windows 或 Linux 环境的易安装应用程序，可让你构建和分享容器化应用程序与微服务。Docker Desktop 包含 Docker 守护进程（dockerd）、Docker 客户端（docker）、Docker Compose、Docker 内容信任、Kubernetes 以及凭证助手。如需了解，可以查看[https://docs.docker.com/desktop/](Docker Desktop)


### Docker 镜像仓库

Docker 注册表用于存储 Docker 镜像。Docker Hub 是一个任何人都可以使用的公共注册表，Docker 默认会在 Docker Hub 上查找镜像。你甚至可以运行自己的私有注册表。

当你使用 docker pull 或 docker run 命令时，Docker 会从你配置的镜像仓库拉取所需的镜像。当你使用 docker push 命令时，Docker 会将你的镜像推送到你配置的镜像仓库。

### Docker 对象

#### Images 镜像

镜像就是一份包含创建 Docker 容器指令的只读模板。通常情况下，一个镜像是基于另一个镜像并进行一些额外定制而成的。例如，你可以构建一个基于 Ubuntu 镜像的镜像，其中包含 Apache 网络服务器、你的应用程序，以及让应用程序运行所需的配置细节。

你可以创建自己的镜像，也可以仅使用他人创建并发布在镜像仓库中的镜像。要构建自己的镜像，你需要创建一个 Dockerfile，它采用简单的语法来定义创建镜像并运行镜像所需的步骤。Dockerfile 中的每条指令都会在镜像中创建一个层。当你修改 Dockerfile 并重新构建镜像时，仅会重建那些发生变更的层。这也是相较于其他虚拟化技术，镜像具备轻量、体积小、运行速度快的特点的原因之一。

#### Containers 容器

容器是镜像的可运行实例。你可以使用 Docker API 或 CLI 创建、启动、停止、移动或删除容器。你可以将容器连接到一个或多个网络，为其挂载存储，甚至基于其当前状态创建一个新的镜像。

默认情况下，容器与其他容器及其宿主机之间的隔离性相对较好。你可以控制容器的网络、存储或其他底层子系统与其他容器或宿主机的隔离程度。

容器由其镜像以及你在创建或启动容器时为其提供的任何配置选项所定义。当容器被删除时，其状态中未存储在持久存储中的任何更改都会消失。

## Docker 底层技术

Docker 由 Go 编程语言编写，并利用 Linux 内核的多项特性来实现其功能。Docker 采用一种名为 namespaces的技术来提供被称为容器的独立工作环境。当你运行一个容器时，Docker 会为该容器创建一组命名空间。

这些命名空间提供了一层隔离机制。容器的每个组件都在独立的命名空间中运行，且其访问权限被限制在该命名空间内。