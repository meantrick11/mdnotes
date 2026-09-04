# Codex 安装与 DeepSeek API 接入全流程指南（含 CC Switch）

> 适用系统：Windows 10/11（PowerShell）、macOS、Linux
> 更新日期：2026-08-04

## 一、方案说明

Codex 是 OpenAI 推出的 AI 编程 Agent，有 CLI、桌面端（已与 ChatGPT 桌面版合并）和 VS Code 插件三种形态。三种形态共用同一份本地配置（`~/.codex` 目录），配置一次，到处可用。

接入 DeepSeek 的关键变化：

- Codex 通过 OpenAI **Responses API** 与模型通信（2026 年 2 月起已移除对 `wire_api = "chat"` 的支持）。
- DeepSeek **V4-Flash 正式版**（2026 年 7 月 31 日开放公测）**原生支持 Responses API**，因此 Codex 可以直连 DeepSeek，不再需要本地代理或协议转换。
- DeepSeek 官方提供一键配置脚本，也支持手动修改 `config.toml`。
- CC Switch 是可选的图形化管理工具，用于在 OpenAI 官方与 DeepSeek 等供应商之间一键切换。

## 二、准备工作

1. **安装 Codex 并至少运行过一次**，让 `~/.codex` 目录被创建出来（一键脚本需要写入该目录）。
2. **注册 DeepSeek 并创建 API Key**：
   - 打开 <https://platform.deepseek.com>，注册并登录；
   - 充值少量余额（按量计费）；
   - 进入 **API Keys** → **创建 API Key**，复制保存（`sk-` 开头，只完整显示一次）。
3. **模型说明**：
   - `deepseek-v4-flash`：已适配 Codex，推荐使用；
   - `deepseek-v4-pro`：官方预计 2026 年 8 月初完成适配，届时配置方式与 Flash 相同。

## 三、安装 Codex

### 3.1 命令行版（CLI）

**Windows（PowerShell）**

1. 安装 Node.js 22 LTS（<https://nodejs.org>），安装时勾选自动加入 PATH；
2. 重开终端，执行：

```powershell
npm install -g @openai/codex
```

**macOS**

```bash
npm install -g @openai/codex
```

或使用 Homebrew（官方仓库 README 写法）：

```bash
brew install --cask codex
```

**Linux / WSL2**

```bash
npm install -g @openai/codex
```

安装后验证版本：

```bash
codex --version
```

> 建议更新到最新版（配置示例按 v0.144.0 及以上编写）。旧版本可能仍保留 `wire_api = "chat"` 行为，与新配置不一致。

### 3.2 桌面版（可选）

Codex 桌面版已与 ChatGPT 桌面版合并：在 Microsoft Store 搜索 **ChatGPT** 安装桌面版，登录后即可使用 Codex 的编程能力。桌面版与 CLI 共用 `~/.codex` 配置，接入 DeepSeek 后桌面端同样生效。

### 3.3 登录 / 认证

首次运行：

```bash
codex
```

按提示选择登录方式：

- **Sign in with ChatGPT**：任意 ChatGPT 套餐（含 Free）都可以登录使用 Codex；
- **API Key**：使用 OpenAI API 额度登录。

之后可用 `codex login` 重新登录或切换账号。

> 提示：接入 DeepSeek 后，模型请求走 DeepSeek 的 API，不消耗 ChatGPT 套餐额度或 OpenAI API 额度；ChatGPT 登录态仅用于 Codex 本体的身份校验。

## 四、接入 DeepSeek

### 4.1 方法一：官方一键脚本（推荐）

前提：已安装 Codex（或 ChatGPT 桌面版）并运行过一次。

**Windows（PowerShell 执行）**

```powershell
irm https://cdn.deepseek.com/api-docs/codex-deepseek-setup-en.ps1 | iex
```

**macOS / Linux（终端执行）**

```bash
bash <(curl -fsSL https://cdn.deepseek.com/api-docs/codex-deepseek-setup.sh)
```

脚本运行后：

1. 首次运行会提示输入 API Key（`sk-` 开头）。如果系统环境变量里已有 `DEEPSEEK_API_KEY`，会自动读取，不再询问；
2. 按菜单选择模型：
   - 1 → `deepseek-v4-flash`（已适配，推荐）；
   - 2 → `deepseek-v4-pro`（预计 2026 年 8 月初适配后可用）；
   - 3 → 恢复安装前的默认配置。

脚本会自动完成以下工作（全程安全可控）：

- 把现有 `~/.codex/config.toml` 备份到 `~/.codex/backup-deepseek/`，随时可还原；
- 写入模型目录 `~/.codex/models.json`，向 Codex 声明 DeepSeek 模型的元数据（上下文窗口、推理档位、工具格式等）；
- 修改 `~/.codex/config.toml`，新增 `[model_providers.deepseek]` 配置段，**保留**原有的 MCP 服务器、项目信任级别等配置；
- 写入前校验 `config.toml` / `models.json` 语法，校验失败则中止，不破坏原文件。

验证：

```bash
codex
```

在模型列表中选中 **DeepSeek-V4-Flash** 即可开始使用。之后想切换模型或恢复默认配置，重新执行同一行脚本，按菜单选择即可。

### 4.2 方法二：手动配置（想完全掌控时）

配置文件路径：

- Windows：`%USERPROFILE%\.codex\config.toml`
- macOS / Linux：`~/.codex/config.toml`

**第一步：编辑 `config.toml`**

```toml
model = "deepseek-v4-flash"
model_provider = "deepseek"
preferred_auth_method = "apikey"
forced_login_method = "api"
model_reasoning_effort = "high"
model_catalog_json = "~/.codex/models.json"

[model_providers.deepseek]
name = "deepseek"
base_url = "https://api.deepseek.com/"
wire_api = "responses"
experimental_bearer_token = "<你的 DeepSeek API Key>"
```

字段说明：

| 字段 | 作用 |
|---|---|
| `model` | 默认模型，填 `deepseek-v4-flash` |
| `model_provider` | 指向下方 `[model_providers.deepseek]` 的键名 |
| `wire_api` | 固定为 `"responses"`（V4-Flash 正式版原生支持） |
| `model_reasoning_effort` | 推理深度，可选 `low` / `high` / `max` |
| `model_catalog_json` | 指向 `models.json`，定义模型列表 |
| `experimental_bearer_token` | 你的 DeepSeek API Key |

> ⚠️ 不要使用 `wire_api = "chat"`：2026 年起新版 Codex 已移除对 Chat Completions 协议的支持，配置 `"chat"` 会直接报错。早期 V4-Flash 预览版使用的 `wire_api = "chat"` 和模型名 `deepseek-v4-flash-preview` 均已过期，需迁移到正式版配置。
>
> 如果你的 Codex 版本不支持 `experimental_bearer_token` 字段，可改用 `env_key = "DEEPSEEK_API_KEY"`，并在系统环境变量中设置 `DEEPSEEK_API_KEY`（Windows 用 `setx DEEPSEEK_API_KEY "sk-你的密钥"`）。

**第二步：创建 `~/.codex/models.json`**

```json
{
  "models": [
    {
      "slug": "deepseek-v4-flash",
      "display_name": "DeepSeek-V4-Flash",
      "description": "Latest frontier agentic coding model.",
      "default_reasoning_level": "high",
      "supported_reasoning_levels": [
        { "effort": "low", "description": "Fast responses with lighter reasoning" },
        { "effort": "high", "description": "Extra high reasoning depth for complex problems" },
        { "effort": "max", "description": "Maximum reasoning depth for the hardest problems" }
      ],
      "context_window": 1048576,
      "supported_in_api": true
    }
  ]
}
```

`context_window` 为 1,048,576 token（约 100 万），适合大型代码库的多文件任务。日常建议保持 `high` 推理档位，复杂任务再切 `max`。

> 手动配置与一键脚本等价；官方脚本会自动备份和校验，出错概率更低，二选一即可，推荐优先用脚本。

## 五、CC Switch（图形化一键切换工具）

### 5.1 是什么

CC Switch（<https://github.com/farion1231/cc-switch>）是开源跨平台桌面应用，用于管理和一键切换多个 AI CLI 工具的供应商配置，目前支持 **Claude Code、Claude Desktop、Codex、Gemini CLI、Grok Build、OpenCode、OpenClaw、Hermes** 共 8 个工具。

它的价值在于：不用再手动翻各自的配置文件（Claude Code 的 `settings.json`、Codex 的 `config.toml`、Gemini CLI 的 `settings.json`），在图形界面里选一下就能完成切换，避免改错格式导致工具启动失败。

### 5.2 安装

1. 打开 [GitHub Releases](https://github.com/farion1231/cc-switch/releases)（当前最新版约 v3.18.0）；
2. Windows 用户下载 `CC-Switch-v{版本号}-Windows.msi` 安装包，或 `CC-Switch-v{版本号}-Windows-Portable.zip` 绿色版；
3. macOS 用户下载 `.dmg`，Linux 用户下载 `.AppImage`。

### 5.3 在 CC Switch 中接入 DeepSeek 并切换

1. 启动 CC Switch，进入 **Codex** 面板；
2. 新增一个 Provider：
   - 供应商类型选 DeepSeek；
   - 填 DeepSeek API Key（`sk-` 开头）；
   - base_url 填 `https://api.deepseek.com`；
   - 模型选 `deepseek-v4-flash`；
3. 点击应用 / 切换，CC Switch 会把供应商配置写入 `~/.codex/config.toml`；
4. 打开 Codex（CLI 或桌面版），在模型列表中选用 DeepSeek-V4-Flash。

几点说明：

- **官方登录与第三方并存**：先在 CC Switch 的 Codex 面板切到 `OpenAI Official`，用 ChatGPT 账号登录一次 Codex（Free 订阅也可以），然后在 `设置 → 通用 → Codex 应用增强` 开启**“切换第三方时保留官方登录”**。这样切到 DeepSeek 时不会丢失官方登录态，随时能一键切回官方模型。
- **本地路由（旧方案）**：早期 DeepSeek 只有 Chat Completions 接口时，CC Switch 通过“本地路由”把 Codex 的请求翻译后转发（端点指向 `127.0.0.1:15721`）。V4-Flash 正式版原生支持 Responses API 后，**直连即可，一般不需要再开本地路由**。
- **适用范围**：如果你只用 DeepSeek + Codex，可以不用 CC Switch；如果同时在用 Claude Code、Codex、Gemini CLI 等多个工具、多个供应商，CC Switch 能显著减少配置维护成本。

## 六、验证与使用

1. 运行：

```bash
codex
```

2. 在模型选择菜单中选中 `DeepSeek-V4-Flash`（也可以用 `codex --model deepseek-v4-flash` 直接指定）；
3. 跑一个小任务验证，例如“用 Python 写一个快速排序，并写单元测试”；
4. 用量与余额在 DeepSeek 开放平台（平台 → Usage）查看。

## 七、常见问题排查

| 现象 | 原因与解决办法 |
|---|---|
| 提示找不到 `~/.codex` 目录 | Codex / ChatGPT 桌面版还没运行过。先安装并启动一次，再重跑一键脚本 |
| 模型列表里没有 DeepSeek | 检查 `~/.codex/models.json` 的 `slug` 是否与 `config.toml` 里的 `model` 完全一致（`deepseek-v4-flash`，注意连字符），并确认文件路径正确 |
| 配置报 `wire_api = "chat"` 错误 | 新版 Codex 只支持 `responses`。重跑官方一键脚本，或按 4.2 节改回 `wire_api = "responses"` |
| 鉴权失败 / 401 | API Key 错误、已失效或余额不足。检查 `experimental_bearer_token` / `DEEPSEEK_API_KEY`，到平台重新创建 Key 并充值 |
| 想恢复 OpenAI 默认配置 | 重跑官方脚本选菜单 3，或把 `~/.codex/backup-deepseek/` 里的备份还原；用 CC Switch 则一键切回 `OpenAI Official` |
| 选了 `deepseek-v4-pro` 但不可用 | 官方尚未完成 V4-Pro 的 Codex 适配（预计 2026 年 8 月初），先用 V4-Flash |

## 八、参考链接

- OpenAI Codex 官方文档（安装 / 快速开始）：<https://developers.openai.com/codex/quickstart>
- DeepSeek 官方文档（接入 Codex）：<https://api-docs.deepseek.com/zh-cn/quick_start/agent_integrations/codex/>
- DeepSeek 官方文档（Responses API）：<https://api-docs.deepseek.com/zh-cn/guides/responses_api/>
- DeepSeek 开放平台（API Key / 充值 / 用量）：<https://platform.deepseek.com>
- CC Switch 项目主页：<https://github.com/farion1231/cc-switch>
- CC Switch 下载（Releases）：<https://github.com/farion1231/cc-switch/releases>
