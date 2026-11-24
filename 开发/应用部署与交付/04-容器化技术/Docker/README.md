# Docker 学习索引

## 定位与场景

Docker 管理容器镜像、容器、网络和卷，适用于跨环境交付可重复运行的应用。Docker Compose 在此基础上描述多服务应用，集群调度由 Kubernetes 专题讨论。

## 知识地图与学习优先级

先掌握 Image、Container、Registry、Volume、Network 的关系，再理解 Docker daemon、镜像构建、运行配置、数据与网络、资源和安全。建议 **P0 / L2**：能独立构建并运行 FastAPI 镜像，定位常见启动与网络问题。

## 详细教程规划

| 顺序 | 计划文件 | 状态 |
| --- | --- | --- |
| 1 | [核心概念与关系.md](核心概念与关系.md) | 已完成 |
| 2 | `02-架构与工作原理.md` | TODO：待学习与撰写 |
| 3 | `03-安装与环境配置.md` | TODO：待学习与撰写 |
| 4 | `04-镜像管理.md` | TODO：待学习与撰写 |
| 5 | `05-容器生命周期.md` | TODO：待学习与撰写 |
| 6 | `06-Dockerfile与镜像构建.md` | TODO：待学习与撰写 |
| 7 | `07-容器网络.md` | TODO：待学习与撰写 |
| 8 | `08-数据卷与持久化.md` | TODO：待学习与撰写 |
| 9 | `09-镜像仓库与分发.md` | TODO：待学习与撰写 |
| 10 | `10-资源限制与安全.md` | TODO：待学习与撰写 |
| 11 | `11-日志与故障排查.md` | TODO：待学习与撰写 |
| 12 | `12-FastAPI工程实践.md` | TODO：待学习与撰写 |

## 已有资料

- [原有 Docker 使用笔记](../../../../计算机通用/常用工具/docker/docker.md)：保留原文，命令和版本在实际练习前核验。
- [原有 Docker 全面学习目录](../../../../计算机通用/常用工具/docker/全面学习/1什么是docker.md)：可作为既有学习记录。
- [容器化技术总览](../README.md)
- [Docker Compose 学习索引](../Docker%20Compose/README.md)

## 官方资源

- [Docker Engine 文档](https://docs.docker.com/engine/)
- [Docker 新手学习路径](https://docs.docker.com/learn/paths/get-started-with-docker-containers/)

## 最小实践与验收

构建一个 FastAPI 镜像，运行两个实例并分别映射端口；为测试数据挂载卷，重建容器后确认数据保留；阅读日志并定位一次故意制造的启动失败。能解释镜像与容器、容器层与卷、内部网络与主机端口的区别即达到当前目标。
