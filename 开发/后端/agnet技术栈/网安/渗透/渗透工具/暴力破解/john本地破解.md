# john的使用

> **相关笔记**: [[hydra|Hydra网络破解]] | [[crunch生成字典|Crunch字典生成]] | [[../../../../靶场/DVWA/BruteForce|DVWA暴力破解]] | [[../../../../CTF/常用工具/暴力破解/john本地破解|CTF-John]]

## 简介

john是用来本地暴力破解的工具，用来对比wordlist中的密码是否能够暴力破解对应目标

## 应用场景

### 一、本地文件暴力破解

首先，**你需要根据提示自行构造一个wordlist，或者直接使用kali自带的wrodlist，指定使用字典**

然后根据文件类型将文件中的密码指纹（通常是hash形式）给提取出来：

#### zip文件
```zip2john secret.zip > hash.txt```
#### rar文件
```rar2john secret.tar > hash.txt```
#### pdf文件
```pdf2john secret.pdf > hash.txt```
#### office文件(.docx,.xlsx等)
```office2john secret.docx > hash.txt```
#### bitlocker加密
```bitlocker2john``` *十分不常用*

#### ssh2john
对付加了密码保护的 SSH 私钥文件（比如 id_rsa）。
```ssh2john id_rsa > hash.txt```

### 🌐 无线网络与流量类

这类工具不处理静态文件，专门用来“撕碎”拦截到的网络数据包。

* **`wpapcap2john`**：这就是你发现的那个！它的全称是 WPA PCAP to John。当你在无线渗透中截获了别人连 WiFi 时的 WPA/WPA2 握手包（通常是 .pcap 格式），用它就能把握手包里的哈希提取出来，然后在本地疯狂爆破 WiFi 密码。
* **`pcap2john`**：更通用的流量包提取工具，可以从网络抓包中提取操作系统认证等其他协议的哈希。

### 💰 密码管家与数字资产类

* **`keepass2john`**：专门对付大名鼎鼎的密码管理器 KeePass 的数据库文件（.kdbx）。一旦爆破成功，目标所有的账号密码就全盘托出了。
* **`bitcoin2john` / `ethereum2john**`：极其硬核！专门用来提取比特币（wallet.dat）或以太坊钱包文件的密码哈希。在现实世界中，经常有人悬赏巨额奖金求破解自己忘记密码的早期比特币钱包，用的就是这类工具。

### 🍏 系统镜像与证书类

针对各种操作系统底层的加密机制。

* **`dmg2john`**：对付苹果 Mac 系统专属的 .dmg 加密磁盘镜像文件。
* **`putty2john`**：专门提取 Windows 下常用的 SSH 客户端 PuTTY 的私钥文件（.ppk）哈希。
* **`pfx2john`**：用于提取 .pfx 或 .p12 格式的数字证书保护密码。

### 🗜️ 进阶压缩包类

* **`7z2john`**：专门对付压缩率极高、加密极强的 .7z 压缩包。由于 7z 的加密算法非常消耗算力，爆破它的速度通常比 ZIP 慢得多。

---

可以说，只要是互联网上叫得上名字的加密文件格式，John 的社区基本都为它写了一个 `*2john` 的转换脚本。黑客遇到不认识的加密文件，第一反应往往就是去终端里敲一下 `ls -l /usr/sbin/*2john` （或者 John 的安装目录），看看有没有现成的工具。
上述将目标文件中的hash指纹提取出来，以便后续让jhon根据字典爆破


最后，需要使用指定字典进行爆破：
```john --wordlist=/user/share/worlists/rockyou.txt hash.txt``

然后john会把密码直接打印出来
