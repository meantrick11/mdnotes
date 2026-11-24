# CI/CD 持续集成与交付

## 解决的问题与位置

把构建、测试、制品与发布动作串成可追踪流程。它位于从源码到可持续服务的交付链中，与「应用构建与制品管理」直接衔接。

## 概念与技术边界

关键概念：触发、流水线、任务、门禁、凭据、发布。

Jenkins 为自托管自动化平台；GitLab CI/CD 和 GitHub Actions 与各自代码托管平台集成。

## 推荐学习顺序与掌握程度

CI/CD 原理 → 构建发布流程 → Jenkins 基础；其余按托管平台选用。开发工程师的建议目标：P1：CI/CD 与 Jenkins 基础到 L2；部署策略到 L3。

## 本域笔记

- [CI-CD核心概念](CI-CD%E6%A0%B8%E5%BF%83%E6%A6%82%E5%BF%B5.md) — 已完成
- Pipeline与Job — TODO：待按「背景、关系、机制、实践」撰写
- 构建与发布流程 — TODO：待按「背景、关系、机制、实践」撰写
- 流水线触发机制 — TODO：待按「背景、关系、机制、实践」撰写
- 部署与发布策略 — TODO：待按「背景、关系、机制、实践」撰写

## 技术专题

- [Jenkins](Jenkins/README.md)
- [GitLab CI-CD](GitLab%20CI-CD/README.md)
- [GitHub Actions](GitHub%20Actions/README.md)

## 与其他领域的关联

- [应用构建与制品管理](../02-%E5%BA%94%E7%94%A8%E6%9E%84%E5%BB%BA%E4%B8%8E%E5%88%B6%E5%93%81%E7%AE%A1%E7%90%86/README.md)
- [全景技术关系](../00-%E5%BA%94%E7%94%A8%E9%83%A8%E7%BD%B2%E4%B8%8E%E4%BA%A4%E4%BB%98%E5%85%A8%E6%99%AF/%E9%83%A8%E7%BD%B2%E6%9E%B6%E6%9E%84%E4%B8%8E%E6%8A%80%E6%9C%AF%E4%BD%93%E7%B3%BB.md)

## 已有知识引用

- [git](../../../%E8%AE%A1%E7%AE%97%E6%9C%BA%E9%80%9A%E7%94%A8/%E5%B8%B8%E7%94%A8%E5%B7%A5%E5%85%B7/git/git.md)
