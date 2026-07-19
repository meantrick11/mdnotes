# FastAPI

## 简介

FastAPI 是一个现代、快速（高性能）的 Web 框架，用于基于标准 Python 类型提示构建 API。

主要特性包括：

- 快速：性能极高，与 NodeJS 和 Go 不相上下（得益于 Starlette 和 Pydantic）。它是现有的最快 Python 框架之一。
- 快速编码：将开发速度提高约 200% 到 300%。 *
- 更少 Bug：减少约 40% 的人为（开发者）导致错误。 *
- 直观：出色的编辑器支持。随处可见的代码补全。减少调试时间。
- 简单：设计易于使用和学习。减少阅读文档的时间。
- 简短：最大限度地减少代码重复。从每个参数声明中获取多个特性。减少 Bug。
- 健壮：获取生产就绪的代码。并带有自动交互式文档。
- 基于标准：基于（并完全兼容）API 的开放标准：OpenAPI（原名 Swagger）和 JSON SchemaFastAPI是基于Python的高性能Web框架，专门用于快速构建API接口服务

官网地址：https://fastapi.org.cn


## 安装

1. 首先，创建虚拟环境进行隔离

```python
python -m venv .venv
uv venv .venv
```

2. 下载官方fastapi包

```python
pip install fastapi[standard]
uv add fastapi[standard]
```




