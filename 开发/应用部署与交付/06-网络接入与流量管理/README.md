# 网络接入与流量管理

## 解决的问题与位置

让请求安全、稳定地到达应用实例。它位于从源码到可持续服务的交付链中，与「可观测性与运行维护」直接衔接。

## 概念与技术边界

关键概念：域名、DNS、TLS、代理、负载均衡、网关。

Nginx 可做反向代理和负载均衡；API Gateway 还可集中处理鉴权、路由与策略。

## 推荐学习顺序与掌握程度

HTTP/DNS/TLS → 反向代理 → Nginx → 网关。开发工程师的建议目标：P1：Nginx 反向代理到 L2；高级流量策略按需。

## 本域笔记

- 正向代理与反向代理 — TODO：待按「背景、关系、机制、实践」撰写
- 负载均衡 — TODO：待按「背景、关系、机制、实践」撰写
- 域名与DNS — TODO：待按「背景、关系、机制、实践」撰写
- HTTPS与TLS — TODO：待按「背景、关系、机制、实践」撰写

## 技术专题

- [Nginx](Nginx/README.md)
- [API Gateway](API%20Gateway/README.md)

## 与其他领域的关联

- [可观测性与运行维护](../07-%E5%8F%AF%E8%A7%82%E6%B5%8B%E6%80%A7%E4%B8%8E%E8%BF%90%E8%A1%8C%E7%BB%B4%E6%8A%A4/README.md)
- [全景技术关系](../00-%E5%BA%94%E7%94%A8%E9%83%A8%E7%BD%B2%E4%B8%8E%E4%BA%A4%E4%BB%98%E5%85%A8%E6%99%AF/%E9%83%A8%E7%BD%B2%E6%9E%B6%E6%9E%84%E4%B8%8E%E6%8A%80%E6%9C%AF%E4%BD%93%E7%B3%BB.md)

## 已有知识引用

- [HTTP协议](../../../%E8%AE%A1%E7%AE%97%E6%9C%BA%E9%80%9A%E7%94%A8/%E8%AE%A1%E7%AE%97%E6%9C%BA%E7%BD%91%E7%BB%9C/%E5%BA%94%E7%94%A8%E5%B1%82/HTTP%E5%8D%8F%E8%AE%AE.md)
- [HTTPS协议](../../../%E8%AE%A1%E7%AE%97%E6%9C%BA%E9%80%9A%E7%94%A8/%E8%AE%A1%E7%AE%97%E6%9C%BA%E7%BD%91%E7%BB%9C/%E5%BA%94%E7%94%A8%E5%B1%82/HTTPS%E5%8D%8F%E8%AE%AE.md)
- [Web服务器](../../web%E5%BC%80%E5%8F%91/Web%E6%9C%8D%E5%8A%A1%E5%99%A8.md)
