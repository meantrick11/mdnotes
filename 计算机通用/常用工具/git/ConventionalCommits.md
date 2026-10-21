# CONVENTIONAL COMMITS 与全局提交校验

> **相关笔记**: [[git|Git 基础]] | [[git复习|Git 复习]]

> 从「提交信息该怎么写」讲到「怎么在这台电脑上强制它被执行」。核心结论：**校验不是发生在 commit 之后，而是挂在 `commit-msg` 钩子上，发生在 commit 对象创建之前的那一瞬间。** 本文提到的所有边界结论都在本机实测过，不是推测。

---

## 0. 速览

### 一次 `git commit` 的四道门

```text
git add
   │
git commit
   │
   ├─① pre-commit ────────── 能拦，但提交信息还没生成
   │
   ├─② prepare-commit-msg ── 能改信息，通常用来预填模板
   │
   ├─③ commit-msg ────────── 信息已写好、提交对象尚未创建
   │                          ★ 唯一能「拿到信息 + 拦住提交」的窗口
   │                          校验必须挂在这里
   │
   ├─④ 创建 commit 对象
   │
   └─⑤ post-commit ───────── 已落地，只能事后通知
```

### 三层防线

```text
第一层  本地钩子          → 即时反馈，可被 --no-verify 绕过      只约束自己
第二层  服务端钩子        → 拦推送，绕不过，需要 git 服务器权限   团队
第三层  CI + MR 必过检查  → 拦合并，绕不过，只要改流水线          团队（推荐）
```

本文重点是**第一层**（个人电脑上的全局约束），第九节讲第二、三层。

---

## 1. Conventional Commits 是什么

它是一份**提交信息书写约定**（spec v1.0.0），源自 Angular 团队的提交规范。

**它不是软件、不是插件、也不是 git 的功能**，就是一套你遵守的文本格式。

### 1.1 格式

```text
<type>(<scope>)!: <description>

[可选正文]

[可选脚注]
```

### 1.2 type 白名单

| type | 用途 |
|---|---|
| `feat` | 新增功能 |
| `fix` | 修 bug |
| `docs` | 只改文档 |
| `style` | 格式调整（不影响逻辑） |
| `refactor` | 重构（既不修 bug 也不加功能） |
| `perf` | 性能优化 |
| `test` | 测试相关 |
| `build` | 构建系统或依赖 |
| `ci` | CI 配置 |
| `chore` | 杂项（不属于以上任何一类） |
| `revert` | 回滚某次提交 |

### 1.3 两个可选结构

```text
feat(api)!: 删掉旧的检索接口        ← 用 ! 标记破坏性变更

feat(api): 删掉旧的检索接口

BREAKING CHANGE: /search 不再接受 mode 参数    ← 或用脚注，等价
```

### 1.4 为什么值得遵循

因为**信息可以被机器解析**，于是版本管理和 CHANGELOG 能自动化：

| 信息里出现什么 | 自动推出的版本变化 |
|---|---|
| `fix:` | 修订号 +1（patch） |
| `feat:` | 次版本号 +1（minor） |
| `!` 或 `BREAKING CHANGE:` | 主版本号 +1（major） |

另外还有两个日常收益：`git log --grep="^fix"` 可以按类型过滤历史；code review 时一眼看出这次提交的意图。

> **注意**：规范只管「怎么写」，**完全不管「怎么校验」**。校验是另一件事，需要你自己搭。

---

## 2. 一次 `git commit` 的四道门

很多人第一次听到「把校验集成到提交流程」，会理解成：

> 我提交完，它自动检查一下。

这个理解不准确。准确地说：

> 我执行 `git commit` 后，git 在**真正创建 commit 对象之前**调用 `commit-msg` 钩子。检查不通过，git 中止这次提交，commit 对象根本不会产生。

### 2.1 四个钩子的时机

| 钩子名 | 跑在什么时候 | 能不能拦住提交 | 能不能拿到提交信息 |
|---|---|---|---|
| `pre-commit` | 索引已建好，提交信息还没生成 | 能拦，但此刻没信息可查 | **不能** |
| `prepare-commit-msg` | 信息模板已生成、还等你编辑 | 能拦，但一般用来预填内容 | 能拿到，通常不用于校验 |
| `commit-msg` | 信息已写好，提交对象尚未创建 | **能拦，这就是校验该待的位置** | **能** |
| `post-commit` | 提交对象已经存在了 | **不能**，只能事后通知 | 能，但已经晚了 |

### 2.2 为什么必须是 `commit-msg`

- **`pre-commit` 跑得太早** —— 提交信息还没生成，你没法判断是 `feat:` 还是 `fix:`。它适合跑代码检查、lint、格式化。
- **`prepare-commit-msg` 跑在模板刚出来时** —— 适合预填 issue 号、分支名，不适合做强校验。
- **`commit-msg` 是唯一的窗口** —— 信息已存在 + 提交还没落地，两个条件同时满足。
- **`post-commit` 已经晚了** —— 提交对象存在了，只能打印一句抱怨。补救得 `git reset --soft HEAD~1` 重新提交；如果已经 push 过就更麻烦。

> **结论**：校验必须挂在 `commit-msg` 上。挂在 `post-commit` 上就只能事后抱怨。

---

## 3. 校验的通用原理

这一节是最该理解的一节——理解之后，用哪个工具、用什么语言都是次要的。

### 3.1 钩子是 git 自带的机制

`hooks` 不是任何第三方提供的功能。git 在每个仓库里都留了一个目录：

```text
<仓库>/.git/hooks/
```

里面按**固定文件名**放可执行文件，git 在特定时机自动调用。你要做的就是「往这个目录里放一个叫 `commit-msg` 的文件」。

### 3.2 名字即协议

文件必须**正好**叫 `commit-msg`（无扩展名、无后缀）。换成 `pre-commit` 就在信息生成前跑——名字决定了时机和语义。

git 同时只认一个目录：

```bash
# 默认：每个仓库各自一份
<仓库>/.git/hooks/

# 被这个配置覆盖（这是实现「全局」的关键）
git config --global core.hooksPath C:/Users/你/.githooks
```

> 注意：`core.hooksPath` 一旦设置，git **就不再读 `<仓库>/.git/hooks/`**。这是个容易被忽略的副作用，第五节会讲怎么处理。

### 3.3 提交信息是以「文件路径」传进来的

git 调用钩子时，把提交信息**文件的路径**作为第一个参数传进来：

```bash
# git 实际执行的就是这样一条命令
sh  /path/to/commit-msg  /path/to/.git/COMMIT_EDITMSG
```

这一点很重要：正因为信息是文件而不是参数，**不管你是 `git commit -m "..."` 还是打开编辑器慢慢写，钩子都拿得到**——两种方式写的是同一个文件。

### 3.4 退出码是唯一的语言

```text
exit 0   → 通过，git 继续创建 commit
exit 非0 → 失败，git 中止提交，并把钩子的输出打给用户看
```

**git 不看钩子里面写了什么，只看退出码。** 这就是「校验」的全部实现方式。

### 3.5 所以任何语言都能实现

| 语言 | 本地钩子 | CI 里跑 | 备注 |
|---|---|---|---|
| shell | ✅ git 自带 bash | ✅ Linux runner 天然有 | 零依赖，但复杂规则写起来痛苦 |
| Python | ✅ | ✅ CI 镜像通常都有 | 规则好读好测，可两处复用 |
| Node | ✅ | ⚠️ 要 CI 镜像里有 Node | commitlint 就是这条路的代价 |
| 编译 exe | ✅ | ⚠️ 要交叉编译 | 没必要 |

**最佳实践**：把规则写成一个**独立脚本**（入参 = 信息文件的路径），本地钩子和 CI 都调它。一份规则两处生效，不会出现「本地过了服务端不过」。

### 3.6 钩子的能力边界

钩子是**纯本机的**：

- ❌ 不会随仓库分发 —— 克隆你仓库的人不会自动获得你的钩子
- ❌ 管不到网页端提交、别人的机器
- ❌ 管不到 `--no-verify`

它约束的是「**这台机器上的命令行提交**」。

---

## 4. 路线 A：仓库级（husky + commitlint）

这是 npm 生态的主流方案，**作用范围是单个仓库**。

### 4.1 完整步骤

```bash
# 1. 装依赖
pnpm add -D husky @commitlint/cli @commitlint/config-conventional

# 2. 初始化 husky
pnpm exec husky init

# 3. 写钩子内容 .husky/commit-msg
pnpm exec commitlint --edit "$1"

# 4. 给它执行权限
chmod +x .husky/commit-msg
```

### 4.2 commitlint 配置 `commitlint.config.mjs`

```js
export default {
  extends: ['@commitlint/config-conventional'],
};
```

### 4.3 验证

```bash
git commit -m "update"                              # 被拦
git commit -m "fix(auth): prevent duplicate login"   # 通过
```

commitlint 的报错长这样：

```text
✖ subject may not be empty [subject-empty]
✖ type may not be empty [type-empty]
```

### 4.4 commitlint 到底检查什么

它不检查代码质量、不跑测试，**只做一件事：读提交信息，按 Conventional Commits 解析，判断是否合规**。常见规则：

| 规则名 | 含义 |
|---|---|
| `type-empty` | 类型不能为空 |
| `type-enum` | 类型必须在白名单里 |
| `subject-empty` | 描述不能为空 |
| `header-max-length` | 头部行不能过长 |
| `body-leading-blank` | 正文前要有空行 |
| `footer-leading-blank` | 脚注前要有空行 |

**角色分工**：commitlint 是「检查器」，husky 是「触发器」，`commit-msg` 是「挂载点」。

### 4.5 husky 实际上在干什么（关键）

husky 今天的主要工作，说出来有点反直觉：

> **它基本上就是替你在仓库的 `.git/config` 里写一行 `core.hooksPath`**，指向仓库内的 `.husky/` 目录。

那为什么它必须依赖 npm？因为 `.git/config` **不跟着仓库走**——队友克隆下来后，得有人在本地执行那行配置，钩子才会生效。husky 借的是 `npm install` 的生命周期（它的 `prepare` 脚本），队友克隆完必然要装依赖，顺手就把钩子注册上了。

**所以「husky 必须装 npm 包」的原因不是它校验做得好，而是它需要一个分发时机。**

### 4.6 这条路的局限

- **仓库级，不是本机级** —— 满足不了「这台电脑上所有仓库都要管」
- 每个仓库都要装一遍依赖、写一份配置
- 需要 Node 运行时
- 想跨仓库统一规则，只能在每个仓库里复制同一份配置

---

## 5. 路线 B：本机全局（`core.hooksPath` + 自写钩子）

**这是本文实际采用的方案**，零依赖，一次性覆盖本机所有仓库。

### 5.1 三个动作

```bash
# 1. 建目录
mkdir -p ~/.githooks

# 2. 把下面的脚本存成 ~/.githooks/commit-msg（注意：无扩展名）
#    并给执行权限
chmod +x ~/.githooks/commit-msg

# 3. 注册为全局钩子目录
git config --global core.hooksPath C:/Users/你的用户名/.githooks
```

脚本全文见 5.5。

### 5.2 设计要点一：链式调用仓库本地钩子

这是**最容易被忽略的坑**：

```bash
git config --global core.hooksPath ~/.githooks
```

这行配置一旦生效，git 就**不再读 `<仓库>/.git/hooks/`** 了。于是：

- 某个仓库自己写了 `commit-msg`（比如跑单测、生成 changelog）
- 你设了全局 `core.hooksPath`
- → 那个仓库的钩子**静默失效，没有任何报错**

所以脚本里主动做了链式调用：

```bash
self_dir="$(cd "$(dirname "$0")" && pwd -P)"
common_dir="$(git rev-parse --git-common-dir 2>/dev/null)" || common_dir=".git"
local_dir="$(cd "$common_dir/hooks" 2>/dev/null && pwd -P)"
if [ -n "$local_dir" ] && [ "$local_dir" != "$self_dir" ] && [ -x "$local_dir/commit-msg" ]; then
  "$local_dir/commit-msg" "$msg_file" || exit $?
fi
```

`"$local_dir" != "$self_dir"` 这个判断是防止自己调用自己造成死循环。

> **实测确认**：仓库里同时存在本地钩子时，两个都会跑；信息不合规时照样拦截。

### 5.3 设计要点二：输出用纯 ASCII

钩子的报错是写到 `stderr` 的，会被原样显示在终端。而 Windows 上：

- cmd / PowerShell 默认用 **GBK 码页**（中文系统）
- 钩子文件是 **UTF-8** 编码

两者不一致 → **中文会变成乱码**。

所以钩子的提示信息全部用英文 ASCII 写。格式规范本身就是 ASCII 的，用英文没有信息损失。

> 如果你的提交操作永远只在 Git Bash / VS Code 终端里做（这两个是 UTF-8），可以改成中文。

### 5.4 设计要点三：豁免「继承来的」提交信息

有些提交信息不是你写的，而是从别处继承的，不该被判违规：

| 情形 | 处理 |
|---|---|
| `git merge` 自动生成的 `Merge branch ...` | 豁免 |
| `git revert` 生成的 `Revert "..."` | 豁免 |
| `git commit --fixup` / `--squash` 生成的 `fixup!` / `squash!` | 豁免 |
| 处于 merge / cherry-pick / revert 进行中的提交 | 豁免 |

脚本里的实现：

```bash
for state in MERGE_HEAD CHERRY_PICK_HEAD REVERT_HEAD; do
  [ -f "$(git rev-parse --git-path "$state" 2>/dev/null)" ] && exit 0
done

case "$header" in
  Merge*|Revert\ \"*|fixup!\ *|squash!\ *) exit 0 ;;
esac
```

### 5.5 钩子脚本全文

```sh
#!/bin/sh
# Global commit-msg hook: enforces Conventional Commits.
#
#   <type>(<scope>)!: <description>
#
#   type     feat|fix|docs|style|refactor|perf|test|build|ci|chore|revert
#   scope    optional, lowercase
#   header   <= 72 characters; a body, if present, needs a blank line first
#
# Exempt: merge / cherry-pick / revert commits, whose message is inherited
#         from another commit rather than written by hand.
# Bypass:  git commit --no-verify
# Remove:  git config --global --unset core.hooksPath

msg_file="$1"
[ -n "$msg_file" ] && [ -f "$msg_file" ] || exit 0

# A global core.hooksPath makes git stop reading .git/hooks, so a repo's own
# commit-msg hook would go silently dead. Run it ourselves instead.
self_dir="$(cd "$(dirname "$0")" && pwd -P)"
common_dir="$(git rev-parse --git-common-dir 2>/dev/null)" || common_dir=".git"
local_dir="$(cd "$common_dir/hooks" 2>/dev/null && pwd -P)"
if [ -n "$local_dir" ] && [ "$local_dir" != "$self_dir" ] && [ -x "$local_dir/commit-msg" ]; then
  "$local_dir/commit-msg" "$msg_file" || exit $?
fi

for state in MERGE_HEAD CHERRY_PICK_HEAD REVERT_HEAD; do
  [ -f "$(git rev-parse --git-path "$state" 2>/dev/null)" ] && exit 0
done

# Drop comment lines and trailing whitespace, so the checks below see only
# what the author actually wrote.
msg="$(grep -v '^#' "$msg_file" | sed -e 's/\r$//' -e 's/[[:space:]]*$//')"
[ -n "$msg" ] && [ -n "$(printf '%s' "$msg" | tr -d '[:space:]')" ] || exit 0

header="$(printf '%s\n' "$msg" | head -n 1)"
second="$(printf '%s\n' "$msg" | sed -n '2p')"

case "$header" in
  Merge*|Revert\ \"*|fixup!\ *|squash!\ *) exit 0 ;;
esac

fail() {
  {
    echo ""
    echo "  commit-msg: rejected - Conventional Commits hook"
    echo ""
    echo "  problem:  $1"
    echo "  got:      $header"
    echo ""
    echo "  expected: <type>(<scope>): <description>"
    echo "            type   feat|fix|docs|style|refactor|perf|test|build|ci|chore|revert"
    echo "            scope  optional, lowercase"
    echo "            header <= 72 chars, blank line before any body"
    echo ""
    echo "  examples: feat(rag): support per-model collections"
    echo "            fix(eval): scope the search gate to retrieval scenarios"
    echo "            docs: sync the roadmap index"
    echo ""
    echo "  bypass for this commit only: git commit --no-verify"
    echo ""
  } >&2
  exit 1
}

types='feat|fix|docs|style|refactor|perf|test|build|ci|chore|revert'

if ! printf '%s' "$header" | grep -Eq "^($types)(\([a-z0-9][a-z0-9._/-]*\))?!?: .+$"; then
  if printf '%s' "$header" | grep -Eq '^[A-Za-z]+(\([^)]*\))?!?: '; then
    fail "unknown type, or the scope/description is malformed"
  fi
  fail "header does not match <type>(<scope>): <description>"
fi

if [ "${#header}" -gt 72 ]; then
  fail "header is ${#header} characters, limit is 72"
fi

if [ -n "$second" ]; then
  fail "a blank line is required between header and body"
fi

exit 0
```

### 5.6 为什么选它

- **零依赖** —— 不需要 Node、不需要 pip、不需要下载任何东西（只用到 git 自带的 bash）
- **一次配置，本机所有仓库生效** —— 包括公司仓库，不需要在每个仓库里重复配置
- **规则只有一处** —— 改规则就编辑这一个文件
- **可随时关掉** —— 一行命令（见第十节）

---

## 6. 两条路线怎么选

| 维度 | 路线 A：husky + commitlint | 路线 B：全局 `core.hooksPath` + 自写脚本 |
|---|---|---|
| 作用范围 | 单个仓库 | **本机所有仓库** |
| 一次性步骤 | 4 步 + 装依赖 | 3 步，**零下载** |
| 规则维护 | 每个仓库一份配置 | **一个文件** |
| 依赖 | Node + npm 包 | 无（git 自带 bash） |
| 规则丰富度 | 成熟生态，规则现成 | 自己写，简单规则够用 |
| 能否接自动发版 | 能（semantic-release 等） | 需自己接 |
| 适合场景 | 团队项目、仓库级规范 | **个人机器、跨项目统一约束** |

简单判断：

- 你要管的是**一个团队仓库** → 路线 A（而且应该配合第九节的 CI）
- 你要管的是**自己这台电脑上的所有提交**（包括 AI 帮你提交的） → 路线 B，就是本文做的
- 两者不冲突，可以叠加

---

## 7. 实测边界：拦得住什么、拦不住什么

**这一节是本文最该看的部分**——这些结论只能实测得到，官方文档不会直接告诉你。

### 7.1 总表

| 提交方式 | 是否拦截 | 说明 |
|---|---|---|
| `git commit -m "..."` | **拦** | 人、AI、GUI 客户端都走这条 |
| `git commit` 打开编辑器写 | **拦** | 与上面同一条路径、同一个文件 |
| `git commit --amend -m "..."` | **拦** | 改已有提交的信息也会被校验 |
| `git rebase` 回放提交 | **不拦** | 信息原样保留 |
| `git cherry-pick` | **不拦** | 信息原样保留 |
| `git merge` 自动生成的 merge commit | **不拦** | 继承信息，脚本主动豁免 |
| `git revert` | **不拦** | 同上 |
| `git commit --no-verify` | **不拦** | 钩子根本没被执行 |

### 7.2 为什么 rebase 和 cherry-pick 拦不住

因为这两种操作**不是「写新信息」**，而是把一条已经存在的提交信息搬过来、重新构成提交。git 在这些路径上**根本不调用 `commit-msg` 钩子**。

所以这不是「脚本放行了」，而是「钩子压根没跑」。

**实际意义（两面）**：

- ✅ **好消息**：历史里那些不合规的旧提交，rebase 时不会炸掉。比如你的历史里有一条 `merge: xxx`（`merge` 不是标准 type），rebase 你的分支不会因为它失败。
- ⚠️ **代价**：rebase 和 cherry-pick 也是一条绕过通道，拦不住。

### 7.3 `--no-verify` 的精确语义（反直觉）

实测结果是：

```text
正常提交              加 --no-verify 后
  pre-commit         ✗ 被跳过
  prepare-commit-msg ✔ 照跑
  commit-msg         ✗ 被跳过
  post-commit        ✔ 照跑
```

也就是说 —— **`--no-verify` 只跳过 `pre-commit` 和 `commit-msg`**，`prepare-commit-msg` 和 `post-commit` 依然执行。

两个推论：

1. `--no-verify` 一定能绕开提交信息校验，本地层面**堵不住**（钩子没被执行，任何语言写的都一样）。
2. 因为 `post-commit` 不受 `--no-verify` 影响，理论上可以放一个**探针**：检测到不合规信息就大声警告。但它**只能报警，不能阻止**——提交已经落地了。

### 7.4 一句话总结

> 本地钩子约束的是「**这台机器上、通过命令行、新写一条提交信息**」这个动作。
> 「所有提交都必须合规」靠本地钩子是做不到的，要真正的强制必须上服务端。

---

## 8. AI 编程时代的特殊考虑

如果提交是由 AI（Claude Code / Qoder / Cursor 等）完成的，规则要额外考虑几件事。

### 8.1 钩子对 AI 天然有效

AI 提交走的是 `git commit` 命令行，**所以钩子对它天然生效**，不需要额外做什么。

而且对 AI 来说，钩子**不是一堵墙，而是一个反馈环**：

```text
AI 写信息 → 钩子拦截 → AI 读到 stderr → 自己改正 → 重新提交 → 通过
```

### 8.2 所以报错文案的质量 = AI 的重试次数

同一个拦截，文案不同，AI 的收敛速度差很多：

| 报错文案 | AI 的反应 |
|---|---|
| `header does not match` | 可能猜错方向，反复试 |
| `problem: ... / got: 你的原文 / expected: 完整格式 / examples: 可套用例` | **一次改对** |

**结论：给 AI 用的钩子，报错里必须包含「你的原文」和「正确的样子」。** 本文的脚本就是这么写的（参见 `fail()` 函数）。

### 8.3 指令层：让 AI 第一次就写对

钩子是**兜底**，不是**引导**。如果 AI 每次都要被拦一次才写对，那是在浪费轮次。更高效的做法是让 AI **一开始就知道规则**：

| 机制 | 落点 | 生效方式 |
|---|---|---|
| 项目级指令文件 | 仓库根的 `AGENTS.md` / `CLAUDE.md` / `QODER.md` | AI 每次进入该项目时读取 |
| 全局记忆 | 各 AI 工具自己的全局记忆/规则目录 | 跨项目生效 |

原则是「**一份规则、多个入口**」：规则正文只写一处，各个入口只放一行指针指过去，避免散成多份将来对不上。

### 8.4 必须显式禁止 `--no-verify`

这是当前配置的一个**真实漏洞**：AI 遇到阻力时，可能会用 `--no-verify` 绕过校验。钩子层面拦不住它，所以只能在**指令层**明确写死：

> 提交信息不合规时必须修改信息重试，**禁止使用 `git commit --no-verify` 绕过校验**。

### 8.5 `commit.template` 对 AI 完全没用

`git config --global commit.template <文件>` 能让你打开编辑器时看到一份预填模板。

但 AI **永远用 `git commit -m` 或 heredoc**，从不打开编辑器 —— 所以模板一次都不会触发。

**AI 优先的工作流里，不要在这上面浪费时间。**

---

## 9. 服务端兜底（可选，团队级）

本地钩子有一个天然漏洞（第 7.3 节）：`--no-verify`。所以成熟团队会在服务端再查一遍。

### 9.1 服务端钩子 `pre-receive`

放在 git 服务器上，每次推送时运行。它能拒绝整个推送，而且**推送者无法绕过**。缺点是需要 git 服务器的管理权限。

### 9.2 CI + MR 必过检查（推荐）

更主流的做法：在 CI 里跑一次校验，然后把它设成 MR 的**必过检查** —— 不通过就不能合。

```yaml
- run: pnpm exec commitlint --from origin/main --to HEAD --verbose
```

注意两点：

- 需要拉取足够的历史，通常设置 `fetch-depth: 0`
- `--from origin/main --to HEAD` 检查的是一个范围内的**多个**提交

### 9.3 分层配合

```text
本地 commit-msg  → 即时反馈，提交时就告诉你错了（体验层）
CI 必过检查      → 最终兜底，绕不过（强制层）
```

两者配合，规范才真正落地。

---

## 10. 附录：常用命令

### 10.1 安装与查看

```bash
# 查看全局钩子目录配置（没输出表示未设置）
git config --global --get core.hooksPath

# 设置为自定义目录
git config --global core.hooksPath C:/Users/你的用户名/.githooks

# 查看钩子文件是否存在、是否有执行权限
ls -l ~/.githooks/
```

### 10.2 临时绕过与整体卸载

```bash
# 仅这一次提交绕过（谨慎使用）
git commit --no-verify

# 整体关掉全局钩子（恢复原状）
git config --global --unset core.hooksPath
```

### 10.3 确认钩子真的在生效

最快的验证方式——在任意临时目录里试一条不合规的信息：

```bash
cd "$(mktemp -d)" && git init -q
git commit --allow-empty -m "updated some stuff"        # 应该被拦截
git commit --allow-empty -m "chore: verify hook works"  # 应该通过
```

被拦截说明钩子生效；如果两条都成功，检查 `core.hooksPath` 和文件执行权限。

### 10.4 检查清单

| 检查项 | 命令 / 位置 |
|---|---|
| 全局钩子目录已配置 | `git config --global --get core.hooksPath` |
| 钩子文件名正确（无扩展名） | `~/.githooks/commit-msg` |
| 有执行权限 | `ls -l ~/.githooks/` 看是否有 `x` |
| 脚本能独立运行 | `sh ~/.githooks/commit-msg <任意信息文件>` |
| 已知绕过路径 | `--no-verify`、rebase、cherry-pick |
| 已知豁免信息 | merge / revert / fixup! / squash! |

---

## 11. 总结

一句话链条：

```text
Conventional Commits  是「提交信息怎么写」的约定
commitlint            是「检查你写对了没」的工具（可选）
husky                 是「把检查挂到钩子上」的桥梁（可选）
commit-msg            是「信息已存在、提交未创建」的唯一校验窗口
post-commit           是「提交已落地」的事后通知层，无法强制拦截
CI / pre-receive      是「防止本地绕过」的最终兜底
```

正确的集成不是「提交之后跑个脚本」，而是：

> **在 `git commit` 完成之前，由 `commit-msg` 钩子检查提交信息。不通过，提交中止；通过，才创建 commit 对象。**

而「个人电脑上全局约束」的实现，本质只需要两样东西：

1. 一行 `git config --global core.hooksPath`
2. 那个目录里一个叫 `commit-msg` 的可执行文件（任何语言写的都行）
