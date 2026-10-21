# binwalk 命令详解（Kali 固件分析神器）

> **相关笔记**: [[../../虚拟机/kali安装|Kali安装]] | [[../信息收集/nmap|Nmap]] | [[../../计算机通用/linux/orders|Linux命令]] | [[../webshell|Webshell]]
## 作用
**binwalk**：二进制固件解析、分离、提取工具
专门拆 **固件包、压缩包、镜像、内嵌文件、加密分区**
常用于：路由器固件、IoT设备、安卓固件、内嵌隐藏文件提取

---

## 常用参数
```bash
# 1. 最简单：扫描分析固件
binwalk 固件名.bin

# 2. 自动提取所有文件（最常用）
binwalk -e 固件.bin

# 3. 递归提取（层层拆里面嵌套文件）
binwalk -Me 固件.bin

# 4. 查看16进制详细信息
binwalk -H 固件.bin

# 5. 只显示压缩包/可执行文件
binwalk -f 固件.bin

# 6. 提取到指定文件夹
binwalk -e -C /tmp/out 固件.bin
```

### 参数简写记忆
- `-e` = extract 提取
- `-M` = 递归深度提取
- `-C` = 指定输出目录
- `-D` = 删除提取失败文件
- `-1` 静默只输出结果

---

## 实战场景
1. **拆路由器固件**
```bash
binwalk -Me openwrt.bin
```
自动拆出：内核、文件系统、配置文件、脚本

2. **找图片/隐藏资源**
```bash
binwalk -e test.img
```
把镜像里内嵌图片、压缩包全部扒出来

3. **分析恶意样本内嵌文件**
看木马是否捆绑其他程序、资源

---

## 搭配命令
提取后常用：
- `unsquashfs` 解压squashfs文件系统
- `strings` 看明文密码
- `file` 判断文件类型

---

## 一句话总结
**binwalk = 二进制文件解包拆包神器，渗透IoT、固件逆向必备**
