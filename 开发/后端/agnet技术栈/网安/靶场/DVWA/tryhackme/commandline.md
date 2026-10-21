# Command Line

> **相关笔记**: [[experience|TryHackMe入门]] | [[network|网络基础]] | [[../../../../计算机通用/linux/orders|Linux命令]] | [[../../../../计算机通用/linux/渗透命令|渗透命令]]

## advantages

1. Lower resource usage
2. Automation
3. remote management:SSH

## Windows Command Line

### cmd.exe 终端控制程序

### ssh user@ip 建立ssh连接

### basic systeminfo 

#### set 列出/设置所有环境变量

#### ver(version) 输出当前设备版本

#### systemifo 输出当前系统的全面信息

### Netework Troubleshooting

#### ipconfig 显示当前网络配置问题：ip address、subnet mask ...
- /all 显示更多的信息，如DNS servers

#### ping target_name(domain/ip) 发送个ICMP包并检测响应，测试网络连通性

#### tracert target_name(domain/ip) 更详细的测试数据，往返目的的所有路由网关的延迟问题

#### nslookup target_domain (DNS server8.8.8.8) （使用某一个DNS服务器）去尝试解析域名的ip地址

#### netstat  查看本机的端口占用情况

-an 查看所有连接+端口
-ano 显示端口+PID进程号
-an |findstr LISTENING 只看监听端口(开放端口)
-r 查看路由表

135 RpcSc服务
3389 RDP 服务

### File & Disk Management

#### cd change directory or print current directory

#### dir 显示子文件夹

- /a 显示所有的文件
- /s 递归显示当前文件夹

#### tree 

#### mkdir directory_name 创建文件夹

#### rmdir directory_name 删除文件夹

#### more file_name 分页显示长文本文件

#### type  file_name 直接把文本内容输出到屏幕

#### copy source.txt directory_name 把文件复制到另一个位置（支持正则

### Task & Process Management

#### tasklist 列出当前正在运行的任务

- /? 帮助信息
- /FI 筛选条件
> tasklist /FI "imagename eq sshd.exe"
- /P
#### taskkill /PID 关闭进程

### 其他

#### chkdsk 检查文件系统损坏、修复分区故障

#### driverquery 查看电脑已经安装的驱动

#### sfc /scannow  扫描并修复损坏的系统文件

#### shutdown 
- /s 关机
- /r 重启
- /t 设置时间
- /a 终止执行


## powershell command

## linux shell command




