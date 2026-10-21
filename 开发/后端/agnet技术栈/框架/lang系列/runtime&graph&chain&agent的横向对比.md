# Runtimes, frameworks, and harnesses
运行时、框架与开发套件
> 原文来源：https://docs.langchain.com/oss/python/concepts/products#agent-runtimes-like-langgraph

## 本页目录
- 智能体框架（例如 LangChain）
  - LangChain 的适用场景
- 智能体运行时（例如 LangGraph）
  - LangGraph 的适用场景
- 智能体开发套件（例如 Deep Agents SDK）
  - Deep Agents SDK 的适用场景
- 功能对比
- 更多学习资料
  - 概念概述

# 运行时、框架与开发套件
理解 LangChain、LangGraph、Deep Agents 三者的差异，以及各自的适用场景

LangChain 维护了多款开源软件包，用于帮助开发者构建智能体。在智能体开发技术栈中，每个包承担不同的职责。搞懂**智能体框架（agent frameworks）**、**智能体运行时（agent runtimes）**、**智能体开发套件（agent harnesses）**之间的区别，有助于根据需求选择合适的工具。

|  | 运行时 Runtime | 框架 Framework | 开发套件 Harness |
|---|---|---|---|
| 核心价值 | - 持久化执行<br>- 流式输出<br>- 人在回路(HITL)<br>- 持久化存储 | - 抽象封装<br>- 多方集成 | - 预置工具集<br>- 提示词模板<br>- 子智能体 |
| 适用场景 | - 需要底层精细控制<br>- 长生命周期、带状态的工作流与智能体 | - 快速上手开发<br>- 团队统一开发范式 | - 更高自主性的智能体<br>- 处理复杂、非确定性任务的智能体 |
| 代表产品 | - LangGraph<br>- Temporal<br>- Inngest | - LangChain<br>- Vercel AI SDK<br>- CrewAI<br>- OpenAI Agents SDK<br>- Google ADK<br>- LlamaIndex | - Deep Agents SDK<br>- Claude Agent SDK<br>- Manus |

## 智能体框架（以 LangChain 为例）
智能体框架提供各类抽象封装，降低基于大模型开发的上手门槛。

LangChain 是一款智能体框架，提供结构化内容块、智能体循环、中间件等抽象能力。LangChain 的抽象设计兼顾上手简易度与高级场景所需的灵活性。虽然 LangChain 底层基于 LangGraph 构建，但开发者使用 LangChain 时无需掌握 LangGraph。

其他智能体框架包括：Vercel AI SDK、CrewAI、OpenAI Agents SDK、Google ADK、LlamaIndex 等。

### LangChain 什么时候用
满足以下场景可以选择 LangChain：
- 希望快速搭建智能体与自主应用
- 需要针对模型、工具、智能体循环的标准化抽象层
- 想要简单易用、同时保留灵活度的开发框架
- 开发流程编排逻辑不复杂的智能体应用

## 智能体运行时（以 LangGraph 为例）
智能体运行时为智能体的生产环境运行提供全套能力，包含：
- **持久化执行（Durable execution）**：智能体遭遇故障不会中断，可以长时间运行，并从故障断点恢复执行
- **流式输出（Streaming）**：支持工作流与响应结果流式返回
- **人在回路（Human‑in‑the‑loop）**：允许人工查看、修改智能体状态，引入人工监督
- **持久化存储（Persistence）**：会话内与会话之间的状态持久化管理
- **底层精细控制**：直接管控智能体编排逻辑，不依赖高层抽象

LangGraph 是面向构建、管理、部署长生命周期带状态智能体的底层编排框架与运行时。智能体框架一般层级更高，运行在智能体运行时之上。例如 LangChain 1.0 就构建在 LangGraph 之上。

其他智能体运行时：Temporal、Inngest，以及其他具备持久执行能力的引擎。

### LangGraph 什么时候用
满足以下场景可以选择 LangGraph：
- 需要对智能体编排做细粒度、底层的控制
- 长生命周期带状态智能体，要求支持故障后恢复的持久执行
- 构建融合确定性步骤与智能体自主决策步骤的复杂工作流
- 需要可用于生产部署的智能体基础设施

## 智能体开发套件（以 Deep Agents SDK 为例）
智能体开发套件是一套**强预设、开箱即用**的框架，内置丰富能力，用来构建复杂、长时运行的智能体。内置能力包含：
- **规划能力**：通过待办清单跟踪多项任务
- **任务委派**：借助子智能体分配任务，保持上下文整洁
- **文件系统**：对接可插拔存储后端，实现文件读写
- **令牌管理**：会话历史摘要、对体积过大的工具返回结果做清理淘汰

Deep Agents SDK 基于 LangGraph 二次开发，额外增加任务规划、用于上下文管理的文件系统、创建子智能体等能力。Deep Agents 面向需要任务规划、任务拆解的复杂多步骤任务，典型场景包含处理搜索结果、脚本、以及其他状态内产出物。

其他智能体开发套件：Claude Agent SDK、Manus，以及其他代码命令行工具。

### Deep Agents SDK 什么时候用
满足以下场景可以选择 Deep Agents SDK：
- 开发需要长时间持续运行的智能体
- 开发处理复杂多步骤任务的智能体
- 希望直接使用预置工具：文件系统操作、bash 执行、自动化上下文工程等
- 希望复用内置提示词模板与子智能体

## 功能对比
LangChain、LangGraph、Deep Agents 都可以实现相似业务目标，但三者的集成层级不一样：

| 功能 | LangGraph | LangChain | Deep Agents |
|---|---|---|---|
| 短期记忆 | 短期记忆 | 短期记忆 | `StateBackend` 状态后端 |
| 长期记忆 | 长期记忆 | 长期记忆 | 长期记忆 |
| 技能集 | - | 多智能体技能 | 技能集 |
| 子智能体 | 子图(Subgraphs) | 多智能体子智能体 | 子智能体 |
| 人在回路 | 中断机制(Interrupts) | 人在回路中间件 | `interrupt_on` 参数 |
| 流式输出 | 流式输出 | 智能体流式输出 | 流式输出 |

## 拓展阅读
- LangChain 概览
- LangGraph 概览
- Deep Agents 概览

---

### 术语小注释
1. **HITL Human‑in‑the‑loop 人在回路**：智能体运行过程允许人类介入、审核、修改状态
2. **Durable execution 持久化执行**：程序崩溃后重启，从上次状态继续执行，而不是从头重来
3. **Harness**：文中译为「开发套件」，指内置大量预设工具、提示词的开箱即用的上层SDK
4. **Subgraph / Subagent**：子图 / 子智能体，用于任务拆分

