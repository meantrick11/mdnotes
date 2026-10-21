# 如何为 Agent 添加 Skills 支持

> How to add skills support to your agent

这篇文档介绍：**如何让一个 AI Agent 或开发工具真正支持 Agent Skills。**

它覆盖 Skill 的完整生命周期：

1. 发现 Skills
2. 告诉模型有哪些 Skills
3. 在需要时加载 Skill 内容
4. 将 Skill 指令放入上下文
5. 在长对话中持续维护 Skill 上下文

无论你的 Agent 架构是什么，核心流程基本一致。

真正会影响具体实现方式的主要是两个问题：

- **Skill 存在哪里？**
  - 本地 Agent：可以扫描用户文件系统。
  - 云端 / Sandbox Agent：通常需要 API、远程 Registry、上传机制或内置资源。

- **模型如何读取 Skill？**
  - 如果模型有文件读取工具，可以直接读取 `SKILL.md`。
  - 如果模型没有文件系统访问能力，则需要提供专门的 Skill 激活工具，或者由 Harness 主动把 Skill 内容注入 Prompt。

前置要求：

你应该先了解 Agent Skills Specification，包括：

- `SKILL.md` 格式
- YAML Frontmatter
- Skill 目录约定
- Skill Metadata

---

# 核心原则：渐进式披露（Progressive Disclosure）

所有支持 Skills 的 Agent，本质上都遵循同一种三级加载策略：

| 层级 | 加载内容 | 什么时候加载 | Token 成本 |
|---|---|---|---|
| Tier 1：Catalog | `name + description` | Session 开始 | 每个 Skill 约 50–100 tokens |
| Tier 2：Instructions | 完整 `SKILL.md` 指令 | Skill 被激活时 | 推荐 `<5000 tokens` |
| Tier 3：Resources | scripts / references / assets | Skill 指令真正引用时 | 根据文件大小而变化 |

核心思想：

Agent **一开始不应该把所有 Skill 全部塞进上下文**。

Session 开始时，只告诉模型：

> 我现在有哪些 Skill，它们分别是做什么的。

模型发现某个 Skill 和当前任务相关后，再读取完整 `SKILL.md`。

如果 `SKILL.md` 又引用：

```text
scripts/evaluate.py
references/design-guide.md
assets/template.json
```

则继续按需读取这些文件。

因此：

```text
Skill Catalog
    ↓
激活 Skill
    ↓
读取 SKILL.md
    ↓
按需读取 references / scripts / assets
```

例如安装了 20 个 Skills，也不意味着一开始需要承担 20 个完整 Skill 的 Token 成本。

只加载当前任务真正使用的 Skill。

> 这是 Agent Skills 设计里最核心的原则之一。

---

# Step 1：发现 Skills（Discover Skills）

Agent 启动 Session 时，需要找到当前环境中所有可以使用的 Skills，并读取其 Metadata。

---

## Where to scan

扫描哪些目录取决于你的 Agent 运行环境。

本地运行的 Agent 通常至少支持两个 Scope：

### Project-level

针对当前项目 / Repository 的 Skill。

例如：

```text
project/
└── .agents/
    └── skills/
```

这些 Skills 一般只在这个项目中使用。

---

### User-level

用户级全局 Skills。

例如：

```text
~/.agents/skills/
```

这些 Skill 可以跨项目使用。

---

除此之外还可以存在：

- Organization-level Skills
- Agent 内置 Skills
- 管理员统一部署的 Skills

常见目录可以设计为：

| Scope | Path | 用途 |
|---|---|---|
| Project | `<project>/.<your-client>/skills/` | 当前 Client 自己的项目 Skill |
| Project | `<project>/.agents/skills/` | 跨 Client 通用 Skill |
| User | `~/.<your-client>/skills/` | 当前 Client 用户级 Skill |
| User | `~/.agents/skills/` | 跨 Client 用户级 Skill |

其中：

```text
.agents/skills/
```

已经逐渐成为一个用于 **跨 Agent Client 共享 Skill** 的通用约定。

Agent Skills Specification 本身并没有强制 Skill 必须放在哪里，它主要规定：

> 一个 Skill 目录内部应该长什么样。

但是 Client 如果支持扫描 `.agents/skills/`，就意味着：

- Claude / Codex / 其他 Agent 安装的 Skills
- 只要符合 Agent Skills 规范
- 你的 Client 也有机会直接发现并使用

这也是 interoperability（互操作性）的关键。

一些 Client 为兼容现有生态，还会扫描：

```text
.claude/skills/
```

此外还可能扫描：

- Git Repository 的祖先目录
- XDG config 目录
- 用户自定义 Skill 路径

---

# What to scan for

在 Skills 目录下面，需要查找：

> 包含名字严格为 `SKILL.md` 的子目录。

例如：

```text
~/.agents/skills/
├── pdf-processing/
│   ├── SKILL.md
│   └── scripts/
│       └── extract.py
│
├── data-analysis/
│   └── SKILL.md
│
└── README.md
```

Agent 会发现：

```text
pdf-processing
data-analysis
```

但是：

```text
README.md
```

不是 Skill。

---

## 实际扫描建议

扫描时应该：

- 跳过 `.git/`
- 跳过 `node_modules/`
- 可以遵守 `.gitignore`
- 限制最大递归深度
- 限制最大扫描目录数量

例如：

```text
max_depth = 4~6
max_directories = 2000
```

目的：

防止大型项目中扫描失控。

---

# Handling name collisions

如果两个 Skill 的：

```yaml
name:
```

相同，需要一个确定性的优先级规则。

目前比较通用的规则是：

> Project-level Skill 覆盖 User-level Skill。

例如：

```text
~/.agents/skills/code-review/
```

和：

```text
project/.agents/skills/code-review/
```

同时存在时：

```text
project > user
```

同一 Scope 内如果出现同名 Skill：

可以采用：

```text
first-found
```

或者：

```text
last-found
```

关键不是选择哪一个，而是：

> 行为必须稳定且可预测。

同时应该输出 Warning，告诉用户：

某个 Skill 被另一个 Skill shadow / override 了。

---

# Trust considerations

Project-level Skill 存在一个安全问题。

因为项目代码可能来自：

```text
git clone unknown-repository
```

而其中可能偷偷包含：

```text
.agents/skills/
```

如果 Agent 自动加载这些 Skill，就相当于：

> 一个不可信 Repository 可以向 Agent 上下文注入指令。

所以建议：

只有用户明确将项目标记为：

```text
trusted
```

之后，才加载 Project-level Skills。

也就是说：

```text
untrusted repository
        ↓
不加载项目 Skill

trusted repository
        ↓
允许加载项目 Skill
```

---

# Cloud-hosted and sandboxed agents

如果 Agent 跑在：

- Container
- Sandbox
- Remote Server
- Cloud Agent

里面，那么它通常无法读取用户本机：

```text
~/.agents/skills/
```

这时 Skill Discovery 机制需要变化。

### Project-level Skills

通常最好解决。

因为 Repository 本身会被 Clone 到 Sandbox。

例如：

```text
repo/
├── src/
└── .agents/
    └── skills/
```

Skill 会跟着代码一起进入 Sandbox。

---

### User-level / Organization-level Skills

这些不会自动存在于 Sandbox。

需要其他机制，例如：

- Clone 一个配置仓库
- 用户提供 Skill URL
- Skill Package
- 用户通过 Web UI 上传 Skill
- Remote Skill Registry
- API 获取

---

### Built-in Skills

Agent 自带 Skill 可以直接打包到：

```text
deployment artifact
```

里面。

这样每个 Session 都可以直接使用。

一旦 Skill 文件已经出现在 Agent 环境里：

后面的流程：

```text
parse
→ disclose
→ activate
```

基本没有区别。

---

# Step 2：解析 `SKILL.md`

发现 Skill 之后，需要解析每一个：

```text
SKILL.md
```

提取：

```text
metadata
+
body
```

---

# Frontmatter extraction

标准 `SKILL.md` 分为两个部分：

```text
---
YAML Frontmatter
---

Markdown Body
```

例如：

```yaml
---
name: pdf-processing
description: Process PDF files
---

# PDF Processing

Use this skill when...
```

解析过程：

### 1. 找到开头的 `---`

### 2. 找到第二个 `---`

两者之间：

```yaml
name:
description:
...
```

按照 YAML 解析。

至少需要提取：

```text
name
description
```

以及其他可选字段。

### 3. 第二个 `---` 之后

全部作为：

```text
Skill Body
```

也就是 Skill 真正的操作说明。

---

# Handling malformed YAML

现实中的 Skill 并不一定完全遵守 YAML 标准。

特别是其他 Client 创建的 Skill。

典型错误：

```yaml
description: Use this skill when: the user asks about PDFs
```

这里：

```text
when:
```

中的 `:` 可能导致 YAML Parser 认为它是新的 Key。

严格来说应该写：

```yaml
description: "Use this skill when: the user asks about PDFs"
```

因此 Client 可以考虑提供容错机制，例如：

解析失败后：

- 自动给 Value 加引号
- 转成 YAML Block Scalar
- 再尝试解析一次

目的不是鼓励错误格式，而是：

> 提升不同 Agent Client 之间 Skill 的兼容性。

---

# Lenient validation

官方建议 Client 在实现时可以：

> Warning 优先，而不是动不动直接拒绝 Skill。

例如：

### Name 与父目录不同

```text
directory:
pdf-processing/

name:
pdf-tool
```

处理：

```text
warning
+
继续加载
```

---

### Name 超过 64 字符

处理：

```text
warning
+
继续加载
```

---

### Description 缺失

处理：

```text
skip skill
+
error
```

因为 `description` 是模型判断是否应该激活 Skill 的核心信息。

---

### YAML 完全无法解析

处理：

```text
skip skill
+
error
```

---

也就是说：

```text
格式小问题
→ Warning

无法用于 Skill Discovery 的核心问题
→ Skip
```

这些诊断信息可以通过：

- Debug Command
- Log
- UI

暴露给用户。

---

# What to store

Agent 至少应该为每个 Skill 保存三个字段：

| 字段 | 内容 |
|---|---|
| `name` | Frontmatter 中的 name |
| `description` | Frontmatter 中的 description |
| `location` | `SKILL.md` 的绝对路径 |

例如：

```python
skills = {
    "pdf-processing": {
        "name": "pdf-processing",
        "description": "...",
        "location": "/home/user/.agents/skills/pdf-processing/SKILL.md"
    }
}
```

建议：

```text
key = skill name
```

这样 Skill 激活时可以快速 Lookup。

---

Skill Body 有两种处理方式。

### 方案 A：Discovery 时一起加载

优点：

```text
激活快
```

缺点：

```text
占内存
Skill 文件变化后无法立即获取新内容
```

---

### 方案 B：Activation 时读取

只保存：

```text
location
```

Skill 真正激活时：

```text
read SKILL.md
```

优点：

- 更节省内存
- Skill 文件更新后可以读取最新版本

Skill Base Directory 可以通过：

```text
parent(location)
```

获得。

之后：

```text
scripts/
references/
assets/
```

等相对路径都基于这个目录解析。

---

# Step 3：向模型披露可用 Skills

这一阶段是：

> Progressive Disclosure Tier 1。

重点是：

告诉模型：

```text
有哪些 Skill
```

而不是：

```text
把 Skill 全文给模型
```

---

# Building the skill catalog

对于每一个 Skill，可以提供：

```text
name
description
location（可选）
```

格式并没有强制要求。

可以是：

- XML
- JSON
- Markdown List

例如：

```xml
<available_skills>
  <skill>
    <name>pdf-processing</name>
    <description>
      Extract PDF text, fill forms, merge files.
      Use when handling PDFs.
    </description>
    <location>
      /home/user/.agents/skills/pdf-processing/SKILL.md
    </location>
  </skill>

  <skill>
    <name>data-analysis</name>
    <description>
      Analyze datasets, generate charts,
      and create summary reports.
    </description>
    <location>
      /home/user/project/.agents/skills/data-analysis/SKILL.md
    </location>
  </skill>
</available_skills>
```

这里：

```text
location
```

有两个作用。

### 1. Skill Activation

模型知道：

```text
SKILL.md 在哪里
```

于是可以直接调用文件工具读取。

### 2. Relative Path Resolution

例如 Skill 中出现：

```text
scripts/evaluate.py
```

模型知道应该基于：

```text
Skill Directory
```

解析。

---

如果你提供一个专门的：

```text
activate_skill
```

工具，并且工具返回 Skill Directory，那么 Catalog 里面可以不暴露 `location`。

每个 Skill Catalog Entry 大约只需要：

```text
50–100 tokens
```

所以即使几十个 Skill，Catalog 仍然比较轻量。

---

# Where to place the catalog

常见有两种方案。

## 方案 1：System Prompt

例如：

```text
System Prompt

...
Available Skills:
...
```

优点：

- 最简单
- 兼容性最好
- 几乎所有 Agent 架构都可以支持

---

## 方案 2：Skill Activation Tool Description

例如 Tool：

```text
activate_skill(name)
```

它的 Description 中包含：

```text
available skills
```

这样：

```text
Discovery
+
Activation
```

绑定在一起。

优点：

System Prompt 更干净。

如果 Agent 已经有专用 Skill Tool，这是很自然的实现。

---

# Behavioral instructions

除了 Catalog，还应该告诉模型：

> Skill 应该怎么用。

如果模型通过文件工具读取：

```text
以下 Skills 提供针对特定任务的专门指令。

当当前任务符合某个 Skill 的 description 时，
在继续任务之前，先使用文件读取工具加载该 Skill 对应的 SKILL.md。

当 Skill 引用了相对路径时，
以 SKILL.md 所在目录作为 Skill 根目录进行解析，
并在工具调用时尽量使用绝对路径。
```

如果通过专门工具激活：

```text
以下 Skills 提供针对特定任务的专门指令。

当当前任务符合某个 Skill 的 description 时，
调用 activate_skill，并传入 Skill name，
加载完整 Skill 指令。
```

这里不应该写得太复杂。

因为这段 Prompt 的作用仅仅是：

```text
告诉模型：
Skill 存在
+
什么时候加载
+
怎么加载
```

真正复杂的流程应该放在：

```text
SKILL.md
```

中。

---

# Filtering

并不是发现的所有 Skill 都必须告诉模型。

有些 Skill 应该过滤掉。

常见情况：

### 用户禁用了 Skill

例如：

```text
enabled = false
```

---

### Permission System 不允许访问

例如：

```text
当前 Workspace 无权限
```

---

### Skill 禁止模型自动激活

例如类似：

```text
disable-model-invocation
```

这样的配置。

---

被过滤的 Skill：

> 应该直接从 Catalog 中隐藏。

而不是：

```text
告诉模型 Skill 存在
→ 模型尝试激活
→ 系统拒绝
```

否则会浪费 Agent Turn。

---

# When no skills are available

如果：

```text
skills = []
```

那么：

不要生成：

```xml
<available_skills />
```

也不要注册一个没有任何可用 Skill 的：

```text
activate_skill
```

工具。

因为：

> 空的 Skill 系统只会增加模型困惑。

直接当 Skill 系统不存在即可。

---

# Step 4：激活 Skills

这一阶段属于：

> Progressive Disclosure Tier 2。

当：

- 模型选择 Skill
- 用户明确选择 Skill

时，将：

```text
完整 Skill Instructions
```

加载进 Conversation Context。

---

# Model-driven activation

多数 Agent Skills 实现并不会写很多：

```text
if keyword == xxx:
    activate skill
```

也不会主要依赖关键词匹配。

更常见的是：

> 让模型自己根据 `name + description` 判断是否应该使用 Skill。

流程：

```text
用户任务
↓
模型读取 Skill Catalog
↓
模型判断某 Skill 是否相关
↓
模型加载 Skill
```

这就是：

```text
Model-driven activation
```

---

## Pattern 1：File-read activation

模型已经拥有普通的：

```text
read_file
```

工具。

那么：

```text
Catalog
↓
找到 location
↓
read_file(SKILL.md)
```

即可。

不需要额外实现专门 Skill Framework。

这是最简单的方案。

---

## Pattern 2：Dedicated tool activation

实现一个：

```text
activate_skill(name)
```

工具。

例如：

```python
activate_skill("pdf-processing")
```

返回：

```text
Skill instructions
```

如果模型没有文件访问能力，这种方式是必须的。

即使有文件访问能力，也可能值得实现。

因为专用 Skill Tool 可以做更多控制。

例如：

### 控制返回内容

可以决定：

```text
保留 YAML
```

还是：

```text
只返回 Markdown Body
```

---

### Structured Wrapping

可以把 Skill 包在特殊 Tag 中。

---

### 列出 Resources

比如：

```text
scripts/
references/
assets/
```

---

### Permission

可以：

```text
检查权限
要求用户确认
```

---

### Analytics

可以统计：

```text
Skill activation count
```

---

Tool Schema 中的：

```text
name
```

最好做成 enum：

```json
{
  "name": {
    "enum": [
      "pdf-processing",
      "data-analysis"
    ]
  }
}
```

这样模型就不能随便 hallucinate：

```text
不存在的 Skill name
```

如果没有任何 Skill：

```text
不要注册 activate_skill
```

---

# User-explicit activation

除了模型自动选择 Skill：

用户也应该可以直接指定 Skill。

例如：

```text
/pdf-processing
```

或者：

```text
$pdf-processing
```

Harness 识别到后：

```text
lookup skill
↓
load SKILL.md
↓
inject context
```

这种情况下：

模型本身甚至不需要执行：

```text
activate_skill
```

因为 Harness 已经提前完成加载。

也可以提供：

```text
autocomplete
```

例如用户输入：

```text
/
```

自动显示：

```text
/pdf-processing
/data-analysis
/code-review
```

提高 Skill 可发现性。

---

# What the model receives

Skill 激活之后，模型到底应该看到什么？

有两种方案。

## 方案 1：完整 `SKILL.md`

包括：

```yaml
---
name:
description:
compatibility:
---
```

和完整 Markdown Body。

这通常是模型直接：

```text
read_file(SKILL.md)
```

时自然得到的结果。

优点：

Frontmatter 中可能存在激活后仍然有价值的信息。

例如：

```text
compatibility
```

可能说明：

- 所需环境
- 所需工具
- 系统要求

---

## 方案 2：只返回 Body

Harness Discovery 阶段已经解析：

```text
name
description
```

激活时：

```text
去掉 YAML
```

只给模型：

```markdown
# PDF Processing

## When to use
...
```

很多专门实现：

```text
activate_skill
```

的 Agent 都倾向这样做。

两种方案都可以。

---

# Structured wrapping

如果使用专门的 Skill Activation Tool：

建议给 Skill 加结构化包装。

例如：

```xml
<skill_content name="pdf-processing">

# PDF Processing

## When to use this skill

Use this skill when the user needs
to work with PDF files...

[rest of SKILL.md body]

Skill directory:
/home/user/.agents/skills/pdf-processing

Relative paths in this skill are relative
to the skill directory.

<skill_resources>

  <file>scripts/extract.py</file>
  <file>scripts/merge.py</file>
  <file>references/pdf-spec-summary.md</file>

</skill_resources>

</skill_content>
```

这样有三个明显好处。

### 1. 模型可以识别 Skill 内容

不会和普通：

- User Message
- Tool Result
- Conversation

混在一起。

---

### 2. Context Management 更容易

之后 Context Compaction 时，可以识别：

```xml
<skill_content>
```

从而保护 Skill 指令。

---

### 3. 可以告诉模型有哪些 Resources

例如：

```text
scripts/extract.py
references/pdf-spec-summary.md
```

但并不需要立即加载它们。

---

# Listing bundled resources

如果使用专门 Activation Tool，可以顺便扫描 Skill Directory：

```text
skill/
├── SKILL.md
├── scripts/
├── references/
└── assets/
```

把资源列表告诉模型。

例如：

```text
Available resources:

scripts/extract.py
scripts/merge.py
references/pdf-guide.md
```

但是：

> 不要自动读取所有 Resource 内容。

仍然遵循：

```text
Progressive Disclosure
```

Skill 什么时候需要：

```text
references/pdf-guide.md
```

模型才去读取。

如果 Skill Directory 特别大：

应该限制：

```text
resource listing count
```

并告诉模型：

```text
列表可能并不完整
```

---

# Permission allowlisting

如果 Agent 有严格的 File Permission System：

Skill Directory 最好加入：

```text
allowlist
```

否则：

```text
Skill
↓
读取 scripts/a.py
↓
Permission Dialog

读取 reference/b.md
↓
Permission Dialog

读取 assets/x.json
↓
Permission Dialog
```

整个 Skill Workflow 会不断被权限确认打断。

因此对于已经可信的 Skill：

可以允许模型访问：

```text
skill directory
```

内部的 Bundled Resources。

---

# Step 5：长期管理 Skill Context

Skill 加入 Context 后：

不能认为工作结束了。

Agent 还需要确保：

> Skill 指令在整个 Session 中持续有效。

---

# Protect skill content from context compaction

长对话通常会发生：

```text
truncate
summarize
compact
```

旧 Message 会被删除或压缩。

但：

> Skill Instructions 不应该轻易被压缩掉。

因为 Skill 本质上属于：

```text
Durable Behavioral Guidance
```

也就是当前任务持续依赖的行为规则。

如果 Skill 内容被静默删除：

```text
Agent 不会报错
```

但是模型会逐渐忘记：

```text
Skill 的工作流程
Skill 的限制
Skill 的验收标准
```

导致性能下降。

因此可以：

### 方案 1

给 Skill Tool Result 设置：

```text
protected = true
```

Context Pruning 时跳过。

---

### 方案 2

通过：

```xml
<skill_content>
```

识别 Skill 内容。

Compaction 时特殊保留。

---

# Deduplicate activations

应该记录：

```text
当前 Session 已经激活过哪些 Skill
```

例如：

```python
activated_skills = {
    "pdf-processing"
}
```

如果模型再次调用：

```text
activate_skill("pdf-processing")
```

可以不重复 Inject。

否则 Context 可能变成：

```text
Skill Instructions
...
Skill Instructions
...
Skill Instructions
...
```

浪费 Token，并可能影响模型行为。

---

# Subagent delegation（可选）

这是更高级的 Skill 使用模式。

并不是所有 Agent Client 都支持。

传统方式：

```text
Main Agent
↓
加载 Skill
↓
执行 Skill Workflow
```

另一种方式：

```text
Main Agent
↓
创建 Subagent
↓
Subagent 加载 Skill
↓
Subagent 完成任务
↓
返回结果 / Summary
↓
Main Agent
```

也就是说：

Skill 不一定必须直接 Inject 到主对话。

可以创建一个：

```text
Skill-specific Agent Session
```

优点：

对于复杂 Skill：

```text
长 Workflow
大量 Reference
大量中间步骤
```

可以避免污染 Main Agent Context。

适用于：

> Skill 本身已经复杂到值得拥有一个独立、专注的执行 Session。

---

# 整体生命周期总结

从 Client 实现角度，可以把整个 Skill Runtime 理解成：

```text
① Discovery
发现 Skill

② Parse
解析 SKILL.md Metadata

③ Catalog
将 name + description 暴露给模型

④ Selection
模型 / 用户选择 Skill

⑤ Activation
加载完整 SKILL.md

⑥ Resource Loading
按需读取 scripts / references / assets

⑦ Execution
按照 Skill Instructions 执行任务

⑧ Context Management
保护 Skill 指令、避免重复激活
```

---

# 最核心的架构

如果只记一个结构，可以记：

```text
Skill Registry
      │
      │ name + description
      ↓
Skill Catalog
      │
      │ Model 判断
      ↓
Skill Activation
      │
      │ SKILL.md
      ↓
Skill Instructions
      │
      │ 按需引用
      ↓
Resources
scripts / references / assets
```

---

# 对实现 Agent Skills 最重要的几个原则

## 1. 不要一次加载所有 Skill

错误：

```text
System Prompt
+
Skill1全文
+
Skill2全文
+
Skill3全文
+
...
```

正确：

```text
Catalog
→ Skill
→ Resource
```

渐进加载。

---

## 2. `description` 是 Skill 路由的核心

模型一开始主要看到：

```text
name
description
```

所以：

```text
description
```

实际上承担了：

> 什么时候应该调用这个 Skill？

的功能。

---

## 3. Skill 激活主要依靠模型语义判断

不是简单：

```text
关键词 → Skill
```

而是：

```text
User Intent
+
Skill Description
+
LLM Reasoning
→ Skill Selection
```

---

## 4. SKILL.md 是 Instruction，Reference 是 Knowledge

典型关系：

```text
SKILL.md
告诉 Agent：

做什么
什么时候做
怎么做
按什么顺序做
什么时候读取哪些资料
```

而：

```text
references/
```

主要保存：

```text
知识
规范
背景资料
详细说明
领域资料
```

这也是 Skill 和知识库设计之间非常重要的边界。

---

## 5. Resources 继续遵循按需加载

不要：

```text
激活 Skill
=
读取整个 Skill 文件夹
```

而应该：

```text
激活 Skill
→ SKILL.md

需要知识
→ references/x.md

需要执行
→ scripts/x.py

需要模板
→ assets/x.json
```

---

## 6. Skill 本质上需要 Runtime 支持

单独写一个：

```text
SKILL.md
```

并不意味着 Agent 就真正支持 Skill。

完整 Skill 系统至少包含：

```text
Discovery
Registry
Catalog
Activation
Resource Resolution
Context Management
Permissions
```

所以从 Agent 架构角度：

> Skill 并不是单纯一种 Prompt 文件格式，而是一套由 Agent Runtime/Harness 支持的渐进式能力加载机制。