# 服务器端请求伪造

> **相关笔记**: [[../API/API|API安全]] | [[../authentication|认证漏洞]] | [[../webLLMattack|Web LLM攻击]] | [[../../DVWA/CommandInjection|命令注入]] | [[../../../../计算机通用/计算机网络/应用层/HTTP协议|HTTP协议]]

## 简介



## labs 

### lab 1
这个实验直接**访问服务器的本地的admin后台**然后删除账户就可以了
![lab1basic](./ssrflab1basic.png)
然后用burp suite 拦截修改请求：
stockApi=http://localhost/admin/delete?username=carlos
![lab1bs](./lab1bs.png)

### lab 2

这个实验需要利用服务器本身**访问内网的其他数据库**的的admin后台(这里需要简单利用intruder模块的numbers爆破一下回环地址http://192.168.0.$x$:8080/admin的值，然后删除calos账户)
![lab2basic](./lab2basic.png)

然后利用burp suite拦截爆破回环地址，
![lab2bs](./lab2bs.png)

最后爆破出来时192.168.0.124,然后发送请求
stockApi=http://192.168.0.124:8080/admin/delete?username=carlos就完成实验了

### lab:SSFT with blacklist_based input filter

![lab3basic](./lab3basic.png)
通过简写127.1，然后自动补全为127.0.0.1，绕过127.0.0.1的黑名单，然后通过双重编码字符a为%2561绕过admin的黑名单，然后通过实验
![lab3bs](./lab3bs.png)

## 常见防御

### blacklist
黑名单：127.0.0.1 admin

-> 双重编码绕过等

### whitelist
有些应用程序只允许输入与白名单中列出的值相匹配的内容。过滤器可能会在输入内容的开头或内部查找匹配项。您或许可以利用 URL 解析中的不一致之处来绕过此过滤器。

URL 规范包含许多特性，当使用此方法实现 URL 的临时解析和验证时，这些特性很可能被忽略：

您可以使用 `--credentials` 字符将凭据嵌入到 URL 中的主机名之前@。例如：

- https://expected-host:fakepassword@evil-host
您可以使用该#字符来表示 URL 片段。例如：

- https://evil-host#expected-host
您可以利用 DNS 命名层次结构，将所需输入内容放入您控制的完全限定域名中。例如：

- https://expected-host.evil-host
你可以对字符进行 URL 编码，从而干扰 URL 解析代码。如果实现过滤器的代码和执行后端 HTTP 请求的代码对 URL 编码字符的处理方式不同，这种方法尤其有用。你还可以尝试对字符进行双重编码；有些服务器会递归地对接收到的输入进行 URL 解码，这可能会导致进一步的差异。
你可以将这些技巧结合起来使用。

### 通过开放重定向绕过 SSRF 过滤器
有时可以通过利用开放重定向漏洞来绕过基于过滤器的防御措施。

在前面的例子中，假设用户提交的 URL 经过严格验证，以防止恶意利用 SSRF 漏洞。但是，允许访问的应用程序存在开放重定向漏洞。如果用于发起后端 HTTP 请求的 API 支持重定向，则可以构造一个符合过滤器要求的 URL，从而将请求重定向到所需的后端目标。

例如，该应用程序存在开放重定向漏洞，其中以下 URL：

- /product/nextProduct?currentProductId=6&path=http://evil-user.net
返回重定向到：

- http://evil-user.net
你可以利用开放重定向漏洞绕过 URL 过滤器，并按如下方式利用 SSRF 漏洞：
```
POST /product/stock HTTP/1.0
Content-Type: application/x-www-form-urlencoded
Content-Length: 118

stockApi=http://weliketoshop.net/product/nextProduct?currentProductId=6&path=http://192.168.0.68/admin
```
这种 SSRF 攻击之所以有效，是因为应用程序首先验证提供的stockAPIURL 是否位于允许的域中，结果证实确实如此。然后，应用程序请求提供的 URL，从而触发开放重定向。它跟随重定向，并向攻击者选择的内部 URL 发出请求。

## 寻找SSRF漏洞的隐藏攻击面
许多服务器端请求伪造漏洞很容易发现，因为应用程序的正常流量涉及包含完整 URL 的请求参数。而其他一些 SSRF 漏洞则更难定位。

### 请求中的部分 URL
有时，应用程序只会将主机名或部分 URL 路径放入请求参数中。提交的值随后会在服务器端被合并成完整的 URL 并发送给用户。如果该值很容易被识别为主机名或 URL 路径，那么潜在的攻击面可能很明显。然而，由于您无法控制请求的完整 URL，因此利用该参数进行完整的 SSRF 攻击的可能性可能有限。

### 数据格式中的 URL
有些应用程序会以允许包含 URL 的格式传输数据，而这些 URL 可能会被数据解析器请求。一个典型的例子是 XML 数据格式，它被广泛用于 Web 应用程序中，用于在客户端和服务器之间传输结构化数据。当应用程序接收并解析 XML 格式的数据时，它可能容易受到 XXE 注入攻击。它也可能容易受到通过 XXE 进行的 SSRF 攻击。我们将在讨论 XXE 注入漏洞时更详细地介绍这一点。

### 通过 Referer 标头进行 SSRF 攻击
有些应用程序使用服务器端分析软件来追踪访客。这类软件通常会在请求中记录 Referer 标头，以便追踪外部链接。分析软件通常会访问 Referer 标头中出现的任何第三方 URL。这样做通常是为了分析引荐网站的内容，包括外部链接中使用的锚文本。因此，Referer 标头往往成为 SSRF 漏洞的攻击面。

### 盲SSRF漏洞
如果能够使应用程序向提供的 URL 发出后端 HTTP 请求，但后端请求的响应没有在应用程序的前端响应中返回，则会出现盲 SSRF 漏洞。

盲源 SSRF 攻击更难利用，但有时会导致在服务器或其他后端组件上执行完整的远程代码。


