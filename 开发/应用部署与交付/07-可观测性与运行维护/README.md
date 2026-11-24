# 可观测性与运行维护

## 解决的问题与位置

发现异常并解释服务为何不可用或变慢。它位于从源码到可持续服务的交付链中，与「应用部署与运行管理」直接衔接。

## 概念与技术边界

关键概念：日志、指标、链路、健康检查、告警。

Prometheus 采集和查询指标；Grafana 展示与告警；OpenTelemetry 提供遥测数据采集标准与工具。

## 推荐学习顺序与掌握程度

日志与健康检查 → 指标与告警 → 链路追踪。开发工程师的建议目标：P0：日志进程端口排障到 L2；P2：平台工具按需到 L2。

## 本域笔记

- 可观测性体系 — TODO：待按「背景、关系、机制、实践」撰写
- Logging-Metrics-Tracing — TODO：待按「背景、关系、机制、实践」撰写
- 健康检查与告警 — TODO：待按「背景、关系、机制、实践」撰写
- 应用故障排查方法论 — TODO：待按「背景、关系、机制、实践」撰写

## 技术专题

- [Prometheus](Prometheus/README.md)
- [Grafana](Grafana/README.md)
- [OpenTelemetry](OpenTelemetry/README.md)

## 与其他领域的关联

- [应用部署与运行管理](../03-%E5%BA%94%E7%94%A8%E9%83%A8%E7%BD%B2%E4%B8%8E%E8%BF%90%E8%A1%8C%E7%AE%A1%E7%90%86/README.md)
- [全景技术关系](../00-%E5%BA%94%E7%94%A8%E9%83%A8%E7%BD%B2%E4%B8%8E%E4%BA%A4%E4%BB%98%E5%85%A8%E6%99%AF/%E9%83%A8%E7%BD%B2%E6%9E%B6%E6%9E%84%E4%B8%8E%E6%8A%80%E6%9C%AF%E4%BD%93%E7%B3%BB.md)
