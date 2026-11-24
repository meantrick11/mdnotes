# 容器编排与集群管理

## 解决的问题与位置

在多节点上声明并维持应用期望状态。它位于从源码到可持续服务的交付链中，与「容器化技术」直接衔接。

## 概念与技术边界

关键概念：Pod、控制器、Service、配置、存储、调度。

Kubernetes 管理集群工作负载；Helm 管理 Kubernetes 应用的模板与发布版本。

## 推荐学习顺序与掌握程度

先 Docker/Compose → Kubernetes 概念与实验 → Helm。开发工程师的建议目标：P2：先 L1，真实集群项目再到 L2/L3。

## 本域笔记

- 容器编排解决的问题 — TODO：待按「背景、关系、机制、实践」撰写

## 技术专题

- [Kubernetes](Kubernetes/README.md)
- [Helm](Helm/README.md)

## 与其他领域的关联

- [容器化技术](../04-%E5%AE%B9%E5%99%A8%E5%8C%96%E6%8A%80%E6%9C%AF/README.md)
- [全景技术关系](../00-%E5%BA%94%E7%94%A8%E9%83%A8%E7%BD%B2%E4%B8%8E%E4%BA%A4%E4%BB%98%E5%85%A8%E6%99%AF/%E9%83%A8%E7%BD%B2%E6%9E%B6%E6%9E%84%E4%B8%8E%E6%8A%80%E6%9C%AF%E4%BD%93%E7%B3%BB.md)
