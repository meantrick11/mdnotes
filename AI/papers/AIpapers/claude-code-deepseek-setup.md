# Claude Code 安装与 DeepSeek API 接入全流程指南

> 适用系统：Windows 10/11（PowerShell）、macOS、Linux
> 更新日期：2026-08-04

## 一、方案说明

Claude Code 是 Anthropic 推出的命令行 AI 编程助手。DeepSeek 官方提供了 **Anthropic 兼容的 API 端点**（`https://api.deepseek.com/anthropic`），因此不需要改动 Claude Code 本体，只要把它的请求地址、鉴权凭证和模型名指向 DeepSeek，就可以用 DeepSeek V4 模型驱动 Claude Code，并按 DeepSeek 的 API 计费。

DeepSeek 端会自动做模型映射：

| Claude Code 请求的模型前缀 | 实际使用的 DeepSeek 模型 |
|---|---|
| `claude-opus` 开头 | `deepseek-v4-pro`（主力编码 / 推理） |
| `claude-haiku`、`claude-sonnet` 开头 | `deepseek-v4-flash`（快速、低成本） |

## 二、准备工作

1. 一台 64 位 Windows 10/11、macOS 或 Linux 电脑。原生安装方式零运行时依赖，**不需要预装 Node.js**。
2. （推荐，可选）安装 [Git for Windows](https://git-scm.com/downloads/win)。装了之后 Claude Code 可以使用 Bash 工具；不装则自动使用 PowerShell 工具。
3. 注册 DeepSeek 开放平台并获取 API Key：
   - 打开 <https://platform.deepseek.com>，注册并登录；
   - 充值少量余额（API 按量计费，余额不足会报错）；
   - 左侧菜单进入 **API Keys** → **创建 API Key**，复制并妥善保存。密钥只在创建时完整显示一次。

## 三、安装 Claude Code

### 3.1 原生安装（推荐）

Windows（PowerShell）：

```powershell
irm https://claude.ai/install.ps1 | iex
```

Windows（CMD）：

```cmd
curl -fsSL https://claude.ai/install.cmd -o install.cmd && install.cmd && del install.cmd
```

macOS / Linux：

```bash
curl -fsSL https://claude.ai/install.sh | bash
```

也可以使用系统包管理器：

```powershell
winget install Anthropic.ClaudeCode   # Windows
```

```bash
brew install --cask claude-code       # macOS
```

安装完成后验证版本：

```bash
claude --version
```

> Windows 下原生版安装在 `%USERPROFILE%\.local\bin\claude.exe`，安装器会自动加入 PATH。如果提示找不到 `claude` 命令，关掉终端重开（或注销重登）再试。

### 3.2 从旧版 npm 安装迁移

`npm install -g @anthropic-ai/claude-code` 的安装方式已被官方弃用。如果之前用 npm 装过，先运行：

```bash
claude install
```

它会自动迁移到原生版本并保留原有配置，然后卸载 npm 残留：

```bash
npm uninstall -g @anthropic-ai/claude-code
```

### 3.3 更新与卸载

- 原生版会后台自动更新；也可以手动执行 `claude update`。
- 卸载：`winget uninstall Anthropic.ClaudeCode`（Windows）；`brew uninstall --cask claude-code`（macOS）。
- 如需彻底清理，可同时删除配置目录 `%USERPROFILE%\.claude`（macOS / Linux 为 `~/.claude`），注意这会清掉登录态和自定义配置。

## 四、配置 DeepSeek API（核心步骤）

在 Claude Code 中接入 DeepSeek 只需设置 4 个环境变量：

| 变量 | 值 |
|---|---|
| `ANTHROPIC_BASE_URL` | `https://api.deepseek.com/anthropic` |
| `ANTHROPIC_AUTH_TOKEN` | 你的 DeepSeek API Key（`sk-` 开头） |
| `ANTHROPIC_MODEL` | `deepseek-v4-pro` |
| `ANTHROPIC_SMALL_FAST_MODEL` | `deepseek-v4-flash` |

> ⚠️ 关键点：接入第三方端点时要使用 `ANTHROPIC_AUTH_TOKEN`，**不要**设置 `ANTHROPIC_API_KEY`。如果环境里残留了 `ANTHROPIC_API_KEY`，Claude Code 可能回退到 Anthropic 官方认证。检查方法：PowerShell 执行 `echo $env:ANTHROPIC_API_KEY`；macOS / Linux 执行 `env | grep ANTHROPIC`。如有残留，先清除并重开终端。

### 4.1 临时生效（先快速验证连通性）

PowerShell：

```powershell
$env:ANTHROPIC_BASE_URL = "https://api.deepseek.com/anthropic"
$env:ANTHROPIC_AUTH_TOKEN = "sk-你的DeepSeek密钥"
$env:ANTHROPIC_MODEL = "deepseek-v4-pro"
$env:ANTHROPIC_SMALL_FAST_MODEL = "deepseek-v4-flash"
claude
```

macOS / Linux：

```bash
export ANTHROPIC_BASE_URL="https://api.deepseek.com/anthropic"
export ANTHROPIC_AUTH_TOKEN="sk-你的DeepSeek密钥"
export ANTHROPIC_MODEL="deepseek-v4-pro"
export ANTHROPIC_SMALL_FAST_MODEL="deepseek-v4-flash"
claude
```

这种写法只对当前终端会话有效，重启终端即失效，适合先确认整条链路能跑通。

### 4.2 永久生效（Windows 推荐 `setx`）

PowerShell 中执行 `setx` 写入**当前用户**环境变量（不需要管理员权限）：

```powershell
setx ANTHROPIC_BASE_URL "https://api.deepseek.com/anthropic"
setx ANTHROPIC_AUTH_TOKEN "sk-你的DeepSeek密钥"
setx ANTHROPIC_MODEL "deepseek-v4-pro"
setx ANTHROPIC_SMALL_FAST_MODEL "deepseek-v4-flash"
```

等价的 .NET 写法（作用相同）：

```powershell
[Environment]::SetEnvironmentVariable("ANTHROPIC_BASE_URL", "https://api.deepseek.com/anthropic", "User")
[Environment]::SetEnvironmentVariable("ANTHROPIC_AUTH_TOKEN", "sk-你的DeepSeek密钥", "User")
[Environment]::SetEnvironmentVariable("ANTHROPIC_MODEL", "deepseek-v4-pro", "User")
[Environment]::SetEnvironmentVariable("ANTHROPIC_SMALL_FAST_MODEL", "deepseek-v4-flash", "User")
```

> `setx` / `SetEnvironmentVariable` 只对**新开的终端**生效，当前已打开的窗口需要重开。

macOS / Linux 把变量追加到 shell 配置文件（`~/.bashrc` 或 `~/.zshrc`）：

```bash
export ANTHROPIC_BASE_URL="https://api.deepseek.com/anthropic"
export ANTHROPIC_AUTH_TOKEN="sk-你的DeepSeek密钥"
export ANTHROPIC_MODEL="deepseek-v4-pro"
export ANTHROPIC_SMALL_FAST_MODEL="deepseek-v4-flash"
```

然后执行 `source ~/.zshrc`（或重开终端）。

### 4.3 配置文件方式（推荐长期使用）

把变量写进 Claude Code 的用户级配置文件，所有会话统一生效：

- Windows：`%USERPROFILE%\.claude\settings.json`
- macOS / Linux：`~/.claude/settings.json`

```json
{
  "env": {
    "ANTHROPIC_BASE_URL": "https://api.deepseek.com/anthropic",
    "ANTHROPIC_AUTH_TOKEN": "sk-你的DeepSeek密钥",
    "ANTHROPIC_MODEL": "deepseek-v4-pro",
    "ANTHROPIC_SMALL_FAST_MODEL": "deepseek-v4-flash"
  }
}
```

项目级配置放在项目根目录 `.claude/settings.json`，只对该项目生效。**不要把包含 API Key 的 settings.json 提交到 Git**，建议加入 `.gitignore`。

### 4.4 可选项：1M 上下文与模型别名

- 需要 1M 上下文时，可使用带 `[1m]` 后缀的模型名（如 `deepseek-v4-pro[1m]`、`deepseek-v4-flash[1m]`），具体以 DeepSeek 官方文档为准。
- 也可以显式指定模型别名变量，覆盖 Claude Code 内置的映射：`ANTHROPIC_DEFAULT_OPUS_MODEL`、`ANTHROPIC_DEFAULT_SONNET_MODEL`、`ANTHROPIC_DEFAULT_HAIKU_MODEL`。

## 五、启动与验证

1. 进入项目目录，运行：

   ```bash
   claude
   ```

2. 首次启动若提示登录 Anthropic 账号，按提示完成登录即可（登录仅用于身份校验，模型请求仍通过环境变量转发到 DeepSeek，不消耗 Anthropic 的额度）。
3. 随便问一个问题（例如“用 Python 写一个快速排序”），确认能正常回答。
4. 在 Claude Code 内输入 `/status`，确认端点为 `api.deepseek.com/anthropic`、模型为 `deepseek-v4-pro`；输入 `/model` 可随时切换模型。
5. 用量与余额在 DeepSeek 开放平台（平台 → Usage）查看。

## 六、常见问题排查

| 现象 | 原因与解决办法 |
|---|---|
| `claude` 不是内部或外部命令 | 安装器写入的目录不在 PATH。重开终端；确认 `%USERPROFILE%\.local\bin` 在 PATH 中 |
| `401 Unauthorized` / 鉴权失败 | API Key 错误、已失效或余额不足。到平台重新创建 Key，并确认 `ANTHROPIC_AUTH_TOKEN` 填写正确 |
| 请求仍发往 Anthropic / 反复要求登录 | 存在残留的 `ANTHROPIC_API_KEY` 或旧配置。检查 shell profile、`settings.json`、`claude config`，清除后重启终端 |
| `400 Invalid model` | 模型名写错。确认是 `deepseek-v4-pro` / `deepseek-v4-flash` |
| 网络超时 / 连接失败 | 代理或防火墙未放行 `api.deepseek.com`，将其加入白名单 |
| 升级后行为异常（旧 npm 版） | 执行 `claude install` 迁移到原生版，再 `npm uninstall -g @anthropic-ai/claude-code` |

## 七、参考资料

- Claude Code 官方文档（安装 / 设置）：<https://code.claude.com/docs/zh-CN/setup>
- Claude Code 官方文档（认证）：<https://code.claude.com/docs/en/authentication>
- DeepSeek 官方文档（接入 Claude Code）：<https://api-docs.deepseek.com/zh-cn/quick_start/agent_integrations/claude_code/>
- DeepSeek 官方文档（Anthropic API）：<https://api-docs.deepseek.com/zh-cn/guides/anthropic_api/>
- DeepSeek 开放平台（API Key / 充值 / 用量）：<https://platform.deepseek.com>
