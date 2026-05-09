# GIT LEARNING NOTES

```markdown
# 🐙 Git 基础与 GitHub 个人访问令牌 (Token) 使用笔记

> 记录如何在本地初始化 Git 仓库、关联 GitHub 远程仓库、使用 Personal Access Token 安全推送代码，以及常见问题的解决方法。

---

## 1. Git 初始化与基本配置

### 1.1 设置用户名和邮箱
```bash
git config --global user.name "你的GitHub用户名"
git config --global user.email "你的GitHub邮箱"
```
这一步讓 Git 知道谁在做提交，显示在提交历史中。

### 1.2 初始化本地仓库
进入项目根目录，执行：
```bash
git init
```
该命令会创建一个隐藏的 `.git` 文件夹，用于跟踪版本变化。

---

## 2. 忽略不需要追踪的文件 (.gitignore)

在项目根目录创建 `.gitignore` 文件，列出不需要提交的文件/文件夹，例如：
```bash
echo ".env" >> .gitignore
echo ".venv" >> .gitignore
echo "__pycache__" >> .gitignore
```
**为什么这样做？**  
- `.env` 通常包含 API 密钥等敏感信息，绝不能上传到公开仓库。  
- `.venv` 是 Python 虚拟环境，体积大且可通过 `requirements.txt` 重建。  
- `__pycache__` 是 Python 缓存目录，无需版本控制。

---

## 3. 添加文件并提交

```bash
# 添加所有文件到暂存区
git add .

# 提交，并附上有意义的提交说明
git commit -m "feat: 初始化项目 - AI 渗透测试 Agent 原型"
```
`git add .` 会将所有新增和修改过的文件放入暂存区，`git commit` 则将这些快照记录为一次版本。

---

## 4. 关联 GitHub 远程仓库并推送

### 4.1 在 GitHub 上创建仓库
1. 登录 GitHub，点击右上角 `+` → `New repository`。
2. 填写仓库名（如 `llm-sec-lab`），选择 **Public**（公开）。
3. **不要**勾选 “Add a README file” 等选项（因为我们要推送已有代码）。
4. 点击 `Create repository`。

### 4.2 关联远程仓库
复制创建后页面上显示的 HTTPS 地址（例如 `https://github.com/meantrick11/llm-sec-lab.git`），在本地执行：
```bash
git remote add origin https://github.com/你的用户名/你的仓库名.git
```
`origin` 是远程仓库的默认别名，指向你刚刚创建的 GitHub 仓库。

### 4.3 推送代码
```bash
git branch -M main         # 将当前分支重命名为 main
git push -u origin main    # 推送 main 分支到远程仓库，并设置上游跟踪
```

---

## 5. 使用 Personal Access Token 进行身份验证

从 2021 年 8 月起，GitHub 不再接受密码直接登录，推送时需要 **Personal Access Token (经典令牌)** 代替密码。

### 5.1 生成 Token 的步骤
1. 登录 GitHub → 右上角头像 → **Settings**。
2. 左侧菜单拉到底部 → **Developer settings**。
3. 点击 **Personal access tokens** → **Tokens (classic)**。
4. 点击 **Generate new token** → **Generate new token (classic)**。
5. **Note**：填写描述，例如 `kali-push`。
6. **Select scopes**：勾选 **repo**（完整控制仓库），会自动勾选其下所有子项。
7. 点击页面底部绿色的 **Generate token**。
8. **立即复制**生成的 Token（类似 `ghp_xxxxxxxxxxxxxxxxxxxx`）！这个 Token **只会显示一次**，关闭页面后无法再查看。

### 5.2 在终端中使用 Token
当推送时提示：
```
Username for 'https://github.com': 你的用户名
Password for 'https://username@github.com': 
```
- **Username**：输入你的 GitHub 用户名。
- **Password**：**粘贴刚才复制的 Token**（注意：粘贴时屏幕上不会显示任何字符，这是正常的安全措施），然后按回车。

成功后，代码会被推送到 GitHub，下次在同一台机器上再次推送通常无需重新输入（凭据会被缓存）。

---

## 6. 常见错误与解决

| 错误信息 | 原因 | 解决办法 |
|:---|:---|:---|
| `fatal: not a git repository` | 当前目录未初始化 Git | 先执行 `git init` |
| `remote: Invalid username or password.` | 使用了登录密码而非 Token | 重新生成 Token 并粘贴到密码提示处 |
| `failed to push some refs` | 本地仓库与远程仓库历史不一致 | 先 `git pull origin main --allow-unrelated-histories`，解决冲突后再推送 |
| `Permission denied (publickey)` | 使用了 SSH 链接但未配置密钥 | 改用 HTTPS 链接，或配置 SSH 密钥 |

---

## 7. 完整操作示例（从零到推送）

```bash
# 1. 进入项目目录
cd ~/my_project

# 2. 初始化 Git
git init

# 3. 创建 .gitignore
echo ".env" >> .gitignore

# 4. 添加文件并提交
git add .
git commit -m "first commit"

# 5. 关联远程仓库（替换为你的地址）
git remote add origin https://github.com/你的用户名/你的仓库.git

# 6. 推送
git branch -M main
git push -u origin main
# 输入用户名和 Token 完成推送
```

---

## 8. 额外小贴士

- 定期执行 `git status` 查看文件状态。
- 使用 `git log --oneline` 查看简洁的提交历史。
- 如果不想每次都输入 Token，可以配置 `git config --global credential.helper store`（不推荐在不安全的机器上使用），或使用 SSH 方式免密。
- 永远不要将 Token 或 `.env` 提交到仓库中！
