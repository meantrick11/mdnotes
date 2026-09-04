# huggingface Agent course

## 1.0 What is Agent

give a goal and finish it by itself.
workflow(steps are complied solidly) or Agent(LLM driven)

## 2.0 LLM as a brain 

narural language input
|
plus system prompt(system&tools difinition and so on)
|
tokenizer
|
transformer(encoder&decoder)
|
final result (agent next instruction or final output)
|
(might get back to step two or three)
|
final output

## 3.0 React model (Thought-Action-Observation)

### 3.1 System propmt 

most of the privilege of system's are high than user's ,definde by different model training result

#### Role

- The Agent’s behavior.
- The Tools our Agent has access to, as we described in the previous section.
- The Thought-Action-Observation Cycle, that we bake into the LLM instructions.

### 3.2 Thought-Action(Tool use)-Observation


### 3.2.1 Thought: Internal Reasoning and the ReAct Approach

#### Chain-of-Thought (CoT)

Chain-of-Thought (CoT) is a prompting technique that guides a model to think through a problem step-by-step before producing a final answer.

#### 以下给出推理层面/Agent层面的不同范式

| 名称 | 维度 | 介绍 | 使用场景 |
|---|---|---|---|
| **CoT**（Chain of Thought，思维链） | 推理范式 | 让模型在给出答案前，显式生成一条线性的中间推理步骤 | 数学题、逻辑题、多步计算等基础推理任务 |
| **Zero-shot CoT** | 推理范式 | 不提供示例，仅用一句触发语（如"一步步思考"）诱导模型推理 | 通用推理任务，追求省token、快速验证 |
| **Few-shot CoT** | 推理范式 | 提供若干"问题+推理+答案"的完整示例作为模板 | 任务格式特殊、需要模型严格遵循特定推理格式时 |
| **Self-Consistency**（自洽性） | 推理范式 | 对同一问题采样多条独立CoT路径，投票选出现次数最多的答案 | 答案唯一但单次推理易错的场景，如数学题、选择题（代价：多次推理成本高） |
| **ToT**（Tree of Thoughts，思维树） | 推理范式 | 每一步生成多个候选分支，可评估、剪枝、回溯，组织成树状结构 | 需要试错和多路径探索的问题，如解谜、创意规划（成本和延迟较高） |
| **GoT**（Graph of Thoughts，思维图） | 推理范式 | 比ToT更灵活，推理节点间可任意合并、聚合，形成图结构 | 需要综合多条推理线得出最终结论的复杂任务，工程复杂度高，多见于研究 |
| **Least-to-Most Prompting** | 推理范式 | 先显式拆解出子问题清单，再按顺序逐一解决 | 有明显层级依赖的组合性问题（需先解A才能解B） |
| **PoT**（Program of Thoughts，程序思维） | 推理范式 / 过渡到执行 | 让模型生成可执行代码代替自然语言推理来完成计算 | 涉及精确数值计算的任务，避免模型"口算"出错 |
| **ReAct**（Reasoning + Acting） | Agent范式 | 推理与行动交替进行：想一步→调用工具/查环境→根据反馈继续想 | 需要联网搜索、调用API、操作外部系统的任务，如Agent、工具增强问答 |
| **Reflexion** | Agent范式 | 执行动作失败后，将失败反馈存入"记忆"，下次推理时参考并改进 | 需要多轮尝试、从失败中学习的任务，如代码调试、复杂任务执行 |
| **Self-Refine** | 推理范式 或 Agent范式（视是否引入外部验证） | 模型先给出答案，再自我审视、批评，然后修改重写 | 写作、代码质量打磨等需要迭代改进的任务；简单客观题不适用（浪费token） |
| **STaR**（Self-Taught Reasoner） | 训练方法 | 用模型自己生成的推理链作为训练数据，迭代自我训练 | 模型研发阶段，用于提升模型自身的推理能力 |
| **RLHF / RLAIF + 推理奖励**（如o系列、DeepSeek-R1） | 训练方法 | 通过强化学习将长链条推理能力直接固化进模型参数，使用时无需手写CoT prompt | 复杂数学、代码、科学推理等高难度任务，直接选用这类"推理模型"即可 |

#### 大模型推理相关方法
│
├── 【推理范式】模型内部怎么想（无外部交互）
│   ├── CoT / Zero-shot CoT / Few-shot CoT   —— 线性思考
│   ├── Self-Consistency                      —— 多路径+投票
│   ├── ToT / GoT                             —— 树/图状思考
│   ├── Least-to-Most                         —— 分解子问题
│   └── PoT                                   —— 代码化思考（过渡到执行）
│
├── 【Agent范式】想+做+看反馈的循环（有外部交互）
│   ├── ReAct                    —— 边想边做，每步动态决策
│   ├── Plan-and-Execute          —— 先整体规划，再顺序执行
│   ├── Plan-Execute-Reflect      —— 规划+执行+失败后重新规划
│   ├── Reflexion                 —— 失败经验存入记忆，下次改进
│   └── Multi-Agent / Orchestrator-Worker  —— 多角色协作/分工
|   |__ **想到agent，就应该想到能否用workflow解决，动态的LLM更不可控，token消耗更多，workflow&Agent（LLM Driven）不是互斥而是互补的关系，两 者可能进行嵌套使用（如大体上和workflow相似，但是决定具体的步骤还可能是交给LLLM驱动的Agent）**
│
└── 【训练方法】这种能力怎么进到模型参数里
    ├── STaR                      —— 自生成推理链自我训练
    └── RLHF/RLAIF + 推理奖励     —— 强化学习内化长链推理（o系列/R1）

**以上范式不一定是独属某一个类别的，比如,Agent的React范式就涉及到输入思考-行动-再输入思考，整个就也像是一个CoT，所以，只是广义上的区分，大多数分类都有狭义的交叉**

### 3.2.2 Actions: Enabling the Agent to Engage with Its Environment

| Type of Action         | Description                                                      |
|------------------------|------------------------------------------------------------------|
| Information Gathering  | Performing web searches, querying databases, or retrieving documents. |
| Tool Usage             | Making API calls, running calculations, and executing code.      |
| Environment Interaction| Manipulating digital interfaces or controlling physical devices. |
| Communication          | Engaging with users via chat or collaborating with other agents.|

### 3.2.3 Observe: Integrating Feedback to Reflect and Adapt

In the observation phase, the agent:

Collects Feedback: Receives data or confirmation that its action was successful (or not).
Appends Results: Integrates the new information into its existing context, effectively updating its memory.
Adapts its Strategy: Uses this updated context to refine subsequent thoughts and actions.

