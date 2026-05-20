# crunch

`crunch` 是 Kali Linux 自带的**密码字典生成工具**，用来按长度、字符集、模式生成所有可能的字符串组合，常用于爆破密码。

---

### 一、安装（Kali 自带，其他系统）
```bash
# Kali（自带，可更新）
sudo apt update
sudo apt install crunch -y

# 查看帮助
man crunch
crunch -h
```

---

### 二、基本语法
```bash
crunch <最小长度> <最大长度> [字符集] [选项]
```
- **min-len**：最短几位（必写）
- **max-len**：最长几位（必写）
- **charset**：自定义用哪些字符（可选，默认小写字母 a-z）
- **options**：输出到文件、分割、模式等

---

### 三、最简单示例（直接跑）

#### 1）默认字符集（小写字母）
```bash
crunch 3 4
```
生成 3～4 位所有小写字母组合（aaa、aab、…、zzzz），输出到屏幕。

#### 2）指定数字（4 位纯数字）
```bash
crunch 4 4 0123456789
```
只生成 0000～9999。

#### 3）保存到文件（-o）
```bash
crunch 4 4 0123456789 -o num4.txt
```
结果写入 `num4.txt`，不再刷屏。

#### 4）混合字符（字母+数字+符号）
```bash
crunch 6 6 abcdef1234!@# -o mix6.txt
```
固定 6 位，从 `abcdef1234!@#` 里随机组合。

---

### 四、常用选项（记这几个就够用）
- `-o 文件名`：输出到文件
- `-b 大小`：按大小分割（如 `10mb`），需配合 `-o START`
- `-c 行数`：每个文件多少行，配合 `-o START`
- `-t 模式`：固定格式，占位符：
  - `@` = 小写字母
  - `,` = 大写字母
  - `%` = 数字
  - `^` = 特殊符号

示例（固定格式：2字母+3数字）：
```bash
crunch 5 5 -t @@%%% -o abc123.txt
```

---

### 五、实用小命令
```bash
# 1-6 位小写字母，保存到 wordlist.txt
crunch 1 6 -o wordlist.txt

# 8 位，大小写+数字
crunch 8 8 abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 -o pass8.txt

# 按 5MB 分割字典
crunch 6 8 0123456789 -o START -b 5mb
```

---

### 六、注意
- 长度越长、字符越多，**组合数爆炸**（比如 8 位数字是 1 亿条），小心磁盘和时间。
- 多用于**授权测试**，不要用于非法爆破。
