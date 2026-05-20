# curl 命令

## 简介

curl  is **a tool for transferring data from or to a server using URLs.**
It supports these protocols: DICT, FILE, FTP, FTPS, GOPHER,  GOPHERS,
HTTP,  HTTPS,  IMAP,  IMAPS,  LDAP,  LDAPS,  MQTT, POP3, POP3S, RTMP,
RTMPS, RTSP, SCP, SFTP, SMB, SMBS, SMTP, SMTPS, TELNET, TFTP, WS  and
WSS.


## 常见操作

### 一、 核心基础语法

`curl` 的基本结构非常简单：

```bash
curl [选项] [URL]

```

#### 1. 最简单的 GET 请求

```bash
curl https://api.github.com

```

*如果不加任何选项，`curl` 默认发送 **GET** 请求，并将返回的 HTML/JSON 源码直接打印在终端。*

#### 2. 保存文件（下载）

* **`-o` (小写)：** 自定义保存的文件名。
```bash
curl -o linux_logo.png https://www.linux.org/images/logo.png

```


* **`-O` (大写)：** 保留服务器上的原始文件名。

```bash
    curl -O https://www.linux.org/images/logo.png
```
*   **断点续传 (`-C -`)：** 如果大文件下载中断了，加上这个可以接着传。
    
```bash
    curl -C - -O http://example.com/bigfile.zip
```

---

### 二、 玩转 API 调试（重点）

日常开发中，我们用 `curl` 最多的地方就是调 API。

#### 1. 发送 POST 请求 (`-X POST`)
最基础的 POST，不带参数：
```bash
curl -X POST https://httpbin.org/post

```

#### 2. 发送表单数据 (`-d` 或 `--data`)

默认会发送 `application/x-www-form-urlencoded` 编码的数据：

```bash
curl -X POST -d "username=adam&password=123" https://httpbin.org/post

```

#### 3. 发送 JSON 数据（最常用）

现在绝大多数 API 都要求 JSON。你需要做两件事：

1. 用 `-H` 添加请求头更改 `Content-Type`。
2. 用 `-d` 传入 JSON 字符串。

```bash
curl -X POST https://httpbin.org/post \
     -H "Content-Type: application/json" \
     -d '{"name": "Jack", "age": 18}'

```

#### 4. 上传文件 (`-F` 或 `--form`)

模拟表单上传文件（`multipart/form-data`），使用 `@` 符号指定本地文件路径：

```bash
curl -X POST -F "profile_pic=@/path/to/avatar.jpg" -F "username=tom" https://httpbin.org/post

```

#### 5. 自定义请求头 (`-H`)

传 Token、User-Agent 等经常用到：

```bash
curl -H "Authorization: Bearer YOUR_TOKEN_HERE" \
     -H "User-Agent: Mozilla/5.0" \
     https://api.github.com/user

```

---

### 三、 实用高级技巧与参数

#### 1. 自动跟随重定向 (`-L`)

很多网址会自动跳转（比如从 `http` 跳转到 `https`），默认情况下 `curl` 不会跟着走，只会返回一个 301/302 状态码。加上 `-L` 就会自动追踪到最终页面：

```bash
curl -L http://google.com

```

#### 2. 查看完整的请求与响应细节 (`-v`)

`-v` (verbose) 是排查网络问题、查看 TLS 握手和 Header 的利器。

```bash
curl -v https://httpbin.org/get

```

> **💡 怎么看 `-v` 的输出？**
> * `>` 开头的行：代表你**发出去**的请求头。
> * `<` 开头的行：代表服务器**返回**的响应头。
> * `*` 开头的行：代表 `curl` 内部的连接信息（DNS 解析、SSL 握手等）。
> 
> 

#### 3. 只看响应头，不看正文 (`-I`)

如果你只想检查 HTTP 状态码、Cookie、或者 Content-Type，不需要下载大文件：

```bash
curl -I https://www.baidu.com

```

#### 4. 忽略 SSL 证书检查 (`-k` 或 `--insecure`)

在测试环境遇到自签名证书、或者证书过期报错 `curl: (60) SSL certificate problem` 时，用它救急：

```bash
curl -k https://localhost:8443

```

#### 5. 使用代理 (`-x`)

科学上网或者抓包调试时非常实用：

```bash
curl -x http://127.0.0.1:7890 https://api.github.com
# 支持 socks5
curl -x socks5://127.0.0.1:1080 https://api.github.com

```

#### 6. 发送 Cookie

**发送特定 Cookie (`-b`)：**

```bash
    curl -b "session_id=abc123xxx; logged_in=yes" https://example.com
```
**保存服务器返回的 Cookie (`-c`)：**
    
```bash
    curl -c cookies.txt https://example.com/login
```

---

### 四、 骨灰级高阶操作：网络性能分析

`curl` 有个非常强大的隐藏功能：`-w` (write-out)，可以用来精确测量 HTTP 请求的各个阶段耗时（DNS解析、TCP握手、SSL握手、首字节响应等）。

你可以直接运行下面这段命令，来测测某个网站的响应速度：

```bash
curl -o /dev/null -s -w "\
本地解析 DNS 耗时:  %{time_namelookup}s\n\
TCP 建立连接耗时:  %{time_connect}s\n\
TLS/SSL 握手耗时:  %{time_appconnect}s\n\
服务器开始响应耗时: %{time_starttransfer}s\n\
-------------------------\n\
总传输完成时间:     %{time_total}s\n" \
https://www.google.com

```

**参数解释：**

* `-o /dev/null`：把返回的网页正文丢弃，不污染屏幕。
* `-s`：静音模式，不显示下载进度条。
* `-w`：格式化输出特定的时间指标。

---

### 五、 常用快捷速查表

| 需求 | 快捷命令 |
| --- | --- |
| **看状态码和 Header** | `curl -I [URL]` |
| **静音模式（不报下载进度）** | `curl -s [URL]` |
| **发送 JSON POST** | `curl -H "Content-Type: application/json" -d 'json' [URL]` |
| **下载并重命名** | `curl -o file.mp4 [URL]` |
| **忽略 HTTPS 证书报错** | `curl -k [URL]` |
| **跟随 302 跳转** | `curl -L [URL]` |
| **显示请求全过程调试信息** | `curl -v [URL]` |

**💡 终极偷懒小贴士：**
在 Chrome 或 Edge 浏览器的开发者工具（F12）中，切到 `Network`（网络）面板，右键任意一个请求，选择 **"Copy" -> "Copy as cURL"**。把它粘贴到终端里，就能完美复现浏览器里一模一样的请求（连带着 Cookie、User-Agent 等所有参数），连代码都不用自己写！




