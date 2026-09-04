● Claude Code 的配置文件分布在以下几个位置：

> **相关笔记**: [[openclawwindows11|OpenClaw Windows]] | [[../agnet技术栈/bilicourse/agent应用开发指南|Agent开发]] | [[../../notebuilde|Claude Code配置教程]] | [[../../secretkey|密钥信息]]

  ---
  1. 全局配置 — C:\Users\lenovo\.claude\

  ┌──────────────────┬────────────────────────────────────────────────────┐
  │       文件       │                        作用                        │
  ├──────────────────┼────────────────────────────────────────────────────┤
  │ settings.json    │ 主配置文件 — 模型选择、API Key、环境变量、权限规则 │
  ├──────────────────┼────────────────────────────────────────────────────┤
  │ keybindings.json │ 键盘快捷键自定义（你目前没有这个文件）             │
  ├──────────────────┼────────────────────────────────────────────────────┤
  │ .claude.json     │ 全局凭证/auth 配置                                 │
  └──────────────────┴────────────────────────────────────────────────────┘

  你当前的 settings.json 配置的是 DeepSeek 代理模式。

  ---
  2. 项目级配置 — 项目根目录下

  ┌──────────────────────────────┬─────────────────────────────────────────────────────────────────┐
  │             文件             │                              作用                               │
  ├──────────────────────────────┼─────────────────────────────────────────────────────────────────┤
  │ CLAUDE.md                    │ 项目指令文件 — 告诉 Claude 怎么理解这个项目（你已有，内容很全） │
  ├──────────────────────────────┼─────────────────────────────────────────────────────────────────┤
  │ .claude/settings.json        │ 项目级 settings，覆盖全局配置（你当前项目没有，需要可新建）     │
  ├──────────────────────────────┼─────────────────────────────────────────────────────────────────┤
  │ .claude/scheduled_tasks.json │ 持久化的定时任务（CronCreate durable=true 时生成）              │
  └──────────────────────────────┴─────────────────────────────────────────────────────────────────┘

  ---
  3. Skills — 不是文件，是内置命令

  Skills 是 Claude Code CLI 内置的，不需要手动编辑文件。你当前的可用 skills：

  - 配置类: update-config, keybindings-help, fewer-permission-prompts
  - 开发类: simplify, init, review, security-review, claude-api
  - 工具类: loop

  调用方式：/skill-name 或直接描述需求。

  ---
  4. Plugins（插件）

  路径：C:\Users\lenovo\.claude\plugins\marketplaces\claude-plugins-official\external_plugins\

  插件自带的 skills 在各自目录的 skills/ 子目录下，比如 Discord 插件有 skills/access/SKILL.md 和
  skills/configure/SKILL.md。

  ---
  5. Memory（持久记忆）

  路径：C:\Users\lenovo\.claude\projects\D--develop-aidevelop-airealative-agentbuild-CTFAgent\memory\

  存储跨会话的记忆（用户偏好、项目上下文、反馈等），由 Claude 自动维护。

  ---
  总结：想改配置主要看两个地方：
  - 全局行为（模型、权限）→ C:\Users\lenovo\.claude\settings.json
  - 项目行为（告诉 Claude 怎么理解项目）→ 项目根目录的 CLAUDE.md

 Claude Code 实用技巧汇总

  1. CLAUDE.md 写得越好，Claude 越聪明

  这是投入产出比最高的优化。一个好的 CLAUDE.md 应该包含：

  # 必写内容
  - 项目一句话描述 + 技术栈
  - 目录结构（哪个文件干什么）
  - 架构规则（"X 和 Y 不能互相导入"）
  - 踩过的坑（"SSL 必须关，因为 Windows 证书链问题"）
  - 运行命令（怎么启动、怎么测试）

  # 不要写
  - 能从代码里读出来的东西（函数签名、变量名）
  - 过长的代码示例片段

  你的 CTFAgent 的 CLAUDE.md 就是个很好的例子。

  ---
  2. Hooks — 自动化你的工作流

  在 settings.json 里配置，让 Claude 在执行特定操作前后自动做事情：

  {
    "hooks": {
      "PostToolUse": [
        {
          "matcher": "Edit|Write",
          "hooks": [{
            "type": "command",
            "command": "python -m pytest"
          }]
        }
      ]
    }
  }

  常见用法：每次编辑代码后自动运行 lint、每次提交前自动跑测试。

  ---
  3. 自定义 Slash Command（别名）

  Hooks 里还可以定义 / 快捷命令：

  {
    "hooks": {
      "SlashCommand": [
        {
          "name": "/deploy",
          "description": "部署到测试环境",
          "prompt": "运行 deploy.sh 脚本，确认构建成功，把输出摘要发给我"
        }
      ]
    }
  }

  这样你输入 /deploy 就自动执行一整套流程。

  ---
  4. 权限精细化控制

  不用每次都点"Allow"：

  {
    "permissions": {
      "allow": [
        "Bash(npm:*)",
        "Bash(git:*)",
        "Bash(python:*)"
      ],
      "deny": [
        "Bash(rm -rf *)"
      ]
    }
  }

  还可以按项目设置不同权限（项目 settings > 全局 settings）。

  ---
  5. 用 /plan 模式避免返工

  大任务不要直接写代码，先用计划模式：

  /plan 实现用户认证功能

  Claude 会先探索代码库、设计方案、让你审核，确认后再动手。比你边写边改高效得多。

  ---
  6. 善用子 Agent 并行工作

  Claude 可以同时启动多个子 Agent 并行干活：

  ▎ "帮我在 3 个文件中搜索 bug，每个文件用一个独立 agent"

  Claude 会自动并行启动，结果汇总，比串行快很多。

  ---
  7. 定时任务 /loop 和 /cron

  # 每 5 分钟检查一次构建状态
  /loop 5m 检查 CI 构建状态

  # 每天 9 点提醒
  /cron 0 9 * * * 检查 PR review 情况

  ---
  8. 键盘快捷键

  常用默认快捷键（可自定义）：
  - Ctrl+E — 用外部编辑器打开当前输入
  - Ctrl+R — 搜索历史命令
  - Esc — 取消当前操作

  自定义在 ~/.claude/keybindings.json。

  ---
  9. 模型切换

  // settings.json
  {
    "model": "claude-sonnet-4-6"  // 全局默认
  }

  快速模式（/fast）用 Opus 更快输出。日常用 Sonnet 省点额度，复杂任务切 Opus。

  ---
  10. 环境变量管理

  {
    "env": {
      "NODE_ENV": "development",
      "DEBUG": "myapp:*"
    }
  }

  注意：不要把 API Key 直接写代码里，用 .env 或 settings.json 的 env 字段。

  ---
  11. Memory 系统 — 跨对话记住偏好

  你不用手动管，但如果想让 Claude 记住重要的东西：

  ▎ "记住：我们这个项目的测试必须用真实数据库，不要 mock"

  Claude 会自动写入 ~/.claude/projects/<项目>/memory/，下次对话也能读取。你已经在用了。

  ---
  12. 项目级 vs 全局配置的优先级

  项目 .claude/settings.json  >  全局 ~/.claude/settings.json  >  CLI 默认值

  权限建议放全局，项目特定配置（如 hooks）放项目级。

  ---
  13. 快速参考

  ┌────────────────────────┬───────────────────────────┐
  │          场景          │           做法            │
  ├────────────────────────┼───────────────────────────┤
  │ 让 Claude 理解你的项目 │ 写好 CLAUDE.md            │
  ├────────────────────────┼───────────────────────────┤
  │ 避免重复点 Allow       │ 配置 permissions.allow    │
  ├────────────────────────┼───────────────────────────┤
  │ 自动跑测试             │ 配置 PostToolUse hook     │
  ├────────────────────────┼───────────────────────────┤
  │ 大任务减少返工         │ /plan 先设计方案          │
  ├────────────────────────┼───────────────────────────┤
  │ 重复性任务             │ 写自定义 / 命令           │
  ├────────────────────────┼───────────────────────────┤
  │ 提高速度               │ /fast 模式 + 并行子 Agent │
  ├────────────────────────┼───────────────────────────┤
  │ 跨会话记忆             │ 让 Claude "记住 X"        │
  └────────────────────────┴───────────────────────────┘