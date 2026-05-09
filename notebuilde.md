下面我把整个过程整理成一份完整的教程，你可以直接保存为文档。从自建节点到 Claude Code 配置，所有关键路径和命令都在里面。

---

# 自建节点 + Claude Code 配置全流程教程

## 一、自建新加坡节点（VPS 搭建代理）

### 1.1 VPS 信息
- **服务商**：ByteVirt
- **产品**：SG-China Optimized - VPS-512-KVM-Premium-SG
- **系统**：Ubuntu 22.04
- **IP 地址**：`185.115.207.117`
- **价格**：$15.00 USD / 季（约 $5/月）
- **快照**：创建快照“初始纯净节点”以防后续折腾出问题

### 1.2 部署代理（服务端）
1. SSH 登录
   ```bash
   ssh root@185.115.207.117
   ```
2. 运行一键脚本
   ```bash
   wget -N --no-check-certificate "https://raw.githubusercontent.com/mack-a/v2ray-agent/master/install.sh" && bash install.sh
   ```
3. 菜单选择：输入 `3` → “一键无域名 Reality”
4. 核心选择：`1`（Xray-core）
5. UUID：直接回车（随机生成）
6. 用户名：输入 `musk`
7. 端口：输入 `22478`（或回车随机）
8. 目标域名：输入 `www.microsoft.com:443`
9. Private Key：直接回车（随机生成）
10. 其他选项一路回车默认
11. 记录生成的 `vless://` 链接

### 1.3 客户端：v2rayN（Windows）
- 下载：[https://github.com/2dust/v2rayN/releases](https://github.com/2dust/v2rayN/releases)
  - 选择 `v2rayN-windows-64.zip`（普通版，非 desktop）
- 导入：复制 `vless://` 链接 → 在 v2rayN 主界面按 `Ctrl+V`
- 连接：右键节点 → “设为活动服务器” → 右键托盘图标 → “系统代理” → “自动配置系统代理”（或 PAC 模式）
- 日常推荐：**PAC 模式** + 路由规则 `V4-绕过大陆(Whitelist)`
- TUN 模式（终端代理）：以管理员身份运行 → 点击“启用 Tun” → 必要时清除系统代理

---

## 二、支付问题（尚未完成，供参考）

- **目标**：解决 Claude Pro / API 境外支付
- **尝试过**：DCS D-Lite 卡（新加坡），但因 App 注册限制放弃
- **当前备选**：
  1. Bitget Wallet Card（Fiat24）—— 需护照 + 加密货币充值
  2. 国内 DeepSeek API 平替（已实现，见下一节）
  3. 未来赴港办卡（门槛提高，需资产证明）
- **注意**：支付卡与环境（IP）要地区一致

**目前使用的方案：Claude Code + DeepSeek API，国内支付，无需境外卡。**

---

## 三、Claude Code + DeepSeek API 配置

### 3.1 环境准备
- **Node.js**：已安装 `v22.16.0`
- **安装路径规划**：所有全局工具安装在 `D:\networksecurity\tercommand\npm-global`

### 3.2 设置 npm 全局路径到 D 盘
```powershell
mkdir D:\networksecurity\tercommand\npm-global -Force
npm config set prefix "D:\networksecurity\tercommand\npm-global"
```

### 3.3 安装 Claude Code
```powershell
npm install -g @anthropic-ai/claude-code --registry=https://registry.npmmirror.com
```
- 安装后启动文件位于：`D:\networksecurity\tercommand\npm-global\claude.cmd`

### 3.4 获取 DeepSeek API Key
1. 访问 [https://platform.deepseek.com](https://platform.deepseek.com)
2. 注册并完成实名认证（支付宝扫码）
3. 在 “API Keys” 创建 Key，形如 `sk-xxxx`
4. 确保账户有余额（最低充值10元）或使用免费额度

### 3.5 配置 Claude Code 使用 DeepSeek

#### 创建配置文件
```powershell
# 强制创建目录和文件
mkdir $env:USERPROFILE\.claude -Force
New-Item -Path "$env:USERPROFILE\.claude\settings.json" -ItemType File -Force
```

#### 写入配置（用 PowerShell 一键写入，注意替换 API Key）
```powershell
@"
{
  "env": {
    "ANTHROPIC_API_KEY": "你的DeepSeek_API_Key",
    "ANTHROPIC_BASE_URL": "https://api.deepseek.com/anthropic",
    "ANTHROPIC_MODEL": "deepseek-v4-pro",
    "ANTHROPIC_SMALL_FAST_MODEL": "deepseek-v4-flash",
    "API_TIMEOUT_MS": "3000000",
    "CLAUDE_CODE_DISABLE_NONESSENTIAL_TRAFFIC": "1"
  }
}
"@ | Out-File -FilePath "$env:USERPROFILE\.claude\settings.json" -Encoding utf8
```

#### 修复首次启动的“引导锁”问题（若遇到 `ERR_BAD_REQUEST`）
```powershell
$f = "$env:USERPROFILE\.claude.json"
$j = Get-Content $f | ConvertFrom-Json
$j | Add-Member -NotePropertyName 'hasCompletedOnboarding' -NotePropertyValue $true -Force
$j | ConvertTo-Json | Set-Content $f
```

### 3.6 让 `claude` 命令全局可用（永久）
1. 打开 Windows 系统环境变量：
   - 按 `Win` → 输入“编辑系统环境变量” → “环境变量”
2. 在 **用户变量** 的 `Path` 中 **新建**：
   ```
   D:\networksecurity\tercommand\npm-global
   ```
3. 关闭所有终端，重新打开即可在任意路径输入 `claude`。

### 3.7 使用方式
```powershell
cd 你的项目目录
claude
```
- 首次进入会询问是否信任当前文件夹，选择 `1. Yes`
- 看到 `deepseek-v4-pro` 提示即表示连接成功

---

## 四、重要文件路径汇总

| 用途 | 路径 / 命令 |
|------|------------|
| VPS IP | `185.115.207.117` |
| 代理端口 | `22478` |
| 节点链接 | `vless://668cd336-...`（请妥善保存） |
| npm 全局安装目录 | `D:\networksecurity\tercommand\npm-global` |
| Claude Code 配置文件 | `C:\Users\lenovo\.claude\settings.json` |
| 首次引导标志文件 | `C:\Users\lenovo\.claude.json` |
| 激活脚本（备用） | `D:\networksecurity\tercommand\claude-env.bat` |
| VPS 管理命令 | SSH 后运行 `vasma` |

---

## 五、注意事项与维护

### 5.1 代理使用
- **日常上网**：v2rayN 开启 PAC 模式，国内直连、国外走代理
- **终端程序（如 git, npm）**：若需要代理，可开启 TUN 模式，或在终端中临时设置环境变量：
  ```powershell
  $env:HTTP_PROXY="http://127.0.0.1:10808"
  $env:HTTPS_PROXY="http://127.0.0.1:10808"
  ```
- **Claude Code 当前走 DeepSeek API，国内直连，无需代理！**

### 5.2 VPS 维护
- 命令行管理：SSH 登录后执行 `vasma`
- 换端口：`vasma` → 选择 `12`
- 更新核心：`vasma` → 选择 `16` 和 `17`
- 快照备份：在 ByteVirt 后台可创建/恢复快照

### 5.3 安全提醒
- **API Key 防泄漏**：对话中曾明文显示 Key，建议去 DeepSeek 后台重新生成一个
- 节点仅个人使用，勿共享，避免触发风控

### 5.4 常见错误处理
- `claude` 命令找不到：检查 Path 是否添加了 `npm-global` 目录
- 启动报 `api.anthropic.com` 错误：检查 `settings.json` 变量名是否为 `ANTHROPIC_API_KEY`，并执行第三节中的“引导锁”修复
- 余额不足：`Insufficient Balance` → 去 DeepSeek 充值

---

## 六、当前状态与后续计划

- ✅ 新加坡自建节点，VLESS+Reality，稳定运行
- ✅ Windows 代理客户端已配置，PAC 模式分流
- ✅ Claude Code 安装并成功对接 DeepSeek V4 Pro API
- ⏳ 境外支付问题待解决，后续可考虑虚拟卡/实体卡
- ⏳ 如需手机上使用，可安装 v2rayNG 导入同一节点

---

教程至此结束，以后随时可以按此文档恢复环境或介绍给他人。祝使用愉快！