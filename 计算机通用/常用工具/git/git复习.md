### 1. 日常开发三连（基础但必须滚瓜烂熟）

| 命令 | 一句话人话解释 |
| :--- | :--- |
| `git init` | 在当前文件夹造一个本地仓库（生出 .git 文件夹）。 |
| `git clone <url>` | 把远程仓库整个拷贝到本地（含所有历史记录）。 |
| `git status` | 看当前工作区和暂存区的状态（红/绿文件）。 |
| `git add <file>` 或 `git add .` | 把改动扔进**暂存区**（准备提交）。 |
| `git commit -m "msg"` | 把暂存区内容生成一个**本地版本快照**。 |
| `git push origin <branch>` | 把本地提交**上传**到远程仓库。 |
| `git pull origin <branch>` | **拉取**远程更新并自动合并到当前分支（= fetch + merge）。 |

---

### 2. 分支操作（团队协作核心）

| 命令 | 一句话人话解释 |
| :--- | :--- |
| `git branch` | 列出所有本地分支（当前分支带 `*`）。 |
| `git branch <name>` | 创建新分支（但还在当前分支不动）。 |
| `git switch <name>` 或 `git checkout <name>` | **切换**到指定分支（旧版用 checkout）。 |
| `git switch -c <name>` 或 `git checkout -b <name>` | **创建并立即切换**到新分支（高频）。 |
| `git merge <branch>` | 把 `<branch>` 的改动**合并**到当前分支（产生合并提交）。 |
| `git branch -d <name>` | 删除本地分支（已合并的才能删）。 |
| `git branch -D <name>` | **强制**删除本地分支（不管是否合并，慎用）。 |

---

### 3. ⭐ 撤销 & 回退（面试必杀区，务必分清）

| 命令 | 一句话人话解释 | 危险等级 |
| :--- | :--- | :--- |
| `git restore <file>` | **工作区**改乱了，直接丢弃该文件的修改（回到上次提交的状态）。 | 低 |
| `git restore --staged <file>` | 已经 `add` 了，**撤回暂存**（文件退回工作区修改状态）。 | 低 |
| `git commit --amend -m "新信息"` | 修改**最近一次**提交的信息，或漏掉文件可 `git add` 后执行此命令补进去。 | ⭐中（会改 hash） |
| `git reset --soft <commit-id>` | 回退到某版本，**保留**工作区和暂存区改动（相当于撤销 commit）。 | 中 |
| `git reset --hard <commit-id>` | 回退到某版本，**丢弃**所有后续改动（工作区+暂存区全没了）——**仅限本地未推送**。 | 🔥高 |
| `git revert <commit-id>` | 生成一个**反向新提交**来抵消指定提交的改动（安全撤销线上代码）。 | ✅低（推荐远程用） |

---

### 4. ⭐ Rebase 变基（面试炫技项）

| 命令 | 一句话人话解释 |
| :--- | :--- |
| `git rebase <base-branch>` | 把当前分支的提交**嫁接到**目标分支的最新提交后面，历史变直线。 |
| `git rebase --continue` | 解决冲突后，继续完成变基操作。 |
| `git rebase --abort` | 变基冲突搞不定了，**放弃**这次变基，回到开始前的状态。 |
| `git rebase -i HEAD~n` | **交互式变基**，可合并（squash）、修改、删除最近 n 个提交（面试亮眼操作）。 |

> **面试嘴替**：“我永远记得 **不要在公共分支（比如 main）上 rebase**，因为会改写历史让队友无法同步。”

---

### 5. ⭐ 远程协作 & 抓取

| 命令 | 一句话人话解释 |
| :--- | :--- |
| `git fetch origin` | 只把远程更新**下载**到本地缓存，**不改**工作区和当前分支（安全查看）。 |
| `git pull --rebase` | 拉取远程并**用 rebase 方式**合并，避免产生多余的 merge 提交（推荐）。 |
| `git push -f` 或 `--force` | 强制推送（改写远程历史）——**团队禁用**。 |
| `git push --force-with-lease` | 更安全的强制推送（如果远程有新提交会拒绝）——**非要强推就用它**。 |

---

### 6. 救急神器（Stash & Reflog）

| 命令 | 一句话人话解释 |
| :--- | :--- |
| `git stash push -m "WIP"` | 把当前未提交的改动**临时存起来**，工作区变干净（用于紧急切分支）。 |
| `git stash list` | 查看所有暂存列表。 |
| `git stash pop` | 恢复最近的暂存内容**并删除**这条暂存记录。 |
| `git stash apply` | 恢复最近的暂存内容**但保留**暂存记录（可多次恢复）。 |
| `git stash drop` | 手动删除某条暂存记录。 |
| `git reflog` | 查看本机所有 HEAD 移动历史（**后悔药**，找回误删的 commit-id）。 |

---

### 7. 查看历史（排查用）

| 命令 | 一句话人话解释 |
| :--- | :--- |
| `git log --oneline` | 查看简洁版提交历史（一行一个 commit）。 |
| `git log --graph --oneline --all` | 图形化查看所有分支的提交网（**面试画图题灵魂命令**）。 |
| `git diff` | 看工作区与暂存区的差异（改了啥还没 add）。 |
| `git diff --staged` | 看暂存区与上一次提交的差异（准备 commit 的内容）。 |
| `git show <commit-id>` | 看某次提交具体改了哪些文件内容。 |

---

