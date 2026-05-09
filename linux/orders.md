# day1

## 查找

### find

**找文件时使用**
find [path] [options] [pattern] 

-name pattern:按照文件名查找，支持*和？
-type type:按照文件类型查找，可以是f、d、l等
-size [+-]size[cwbkMG]:按照文件大小查找，支持使用+或-表示大于或小于指定大小，可以是c（字节）、w（字数）、（块数）、KB、MB、GB
-user username:按照文件所有者查找
-group groupname:按照所属组查找

### grep(global reguler expression)

**用于查找文件里符合条件的字符串或者正则表达式**

grep [-ivnrl]  [pattern]  [files]

-i:忽略大小写进行匹配
-v:反向查找，只打印不匹配的行
-n:显示匹配行的行号
-r:递归查找子目录中的文件
-l:只打印匹配的行数

## 权限

### chown(change owner)

**用于设置文件所有者和文件关联组的命令**

change [-Rvcfh]\ 所有者[:组]文件

-R(--recursive)：递归更改目录及其子目录所有者
-v(--verbose):显示详细操作信息
-c(changes):只显示实际发生更改的文件
-f(-silent)：抑制错误信息
-h（--no-dereference）：影响符号链接本身，而不是链接指向的文件

### chmod(change mode)

**用于更改文件或目录权限的命令**

chmod [-cfvR] [权限模式(124/777)] [文件]

-c:若文件权限已经更改，才显示其更改动作
-f(--silent):抑制错误信息
-v:显示权限变更的详细资料
-R(recursive)：递归变更目录及其子目录的权限

### id 

**用于显示用户的ID,以及所属群聊的ID**

id\ [-gFnru]\ [用户名称]

-g或--group:显示用户所属群组的ID
-G或--groups:显示用户所属的附加群组的ID
-n或--name：显示用户，所属群组或附加群组的名称
-r或--real：显示实际的ID
-u或--user：显示用户的ID
-help：显示帮助
-version：显示版本信息

### sudo(super user do)

**用于临时提权到superuser**

sudo\ [-isuvke] [command]

-i:start a login shell as target user，模拟初始登录，加载目标用户的环境
-s:start a shell without login enviroment，运行指定shell
-u user: run as special user，指定用户运行
-v:验证用户凭据，延长sudo会话
-k:invaidate cached credentials，撤销sudo凭据缓存
-E:preserve enviroment variables

## 路径

### pwd(print working directory)

**用于显示当前用户所在的工作目录完整路径**

pwd [--help][--version]


### ls(list directory contents)

**用于显示指定工作目录下的内容**

ls [-alrtAFR][name...]

-a(all)：显示所有文件
-A(--Almost-all):显示除了.和..的所有文件
-l:以长格式列出文件（权限、所有者、大小、修改时间等）
-R(--recursive):递归列出子目录内容
-r(--reverse):反向排序
-h(--human-readable):以人类可读格式显示文件的大小
-t(time):按修改时间排序
-s(--size):按照大小排序

### cd(change directory)

**改变工作目录**

cd [dirName]


## 文件

### cat(concatenate)

**用于连接文件并打印到标注你输出设备上*连接和查看文件***

cat [-nbsETv] [文件]

-n(number):显示行号，会在输出的每一行前加上行号
-b：显示行号，只对非空行进行编号
-s：压缩连续的空行，只显示一个空行
-E:在每一行的末尾显示$
-v:显示一些非打印字符

## 信息收集

### ifconfig/ip a 

**查看本机ip、网卡模式**

ifconfig ///

add\<地址> 设置网络设备的ipv6地址
del\<地址>：删除网络设备ipv6地址
down：关闭指定的网络设备
up：启动指定的网络设备
netmask\<掩码>：设置网络设备的子网掩码
\[ip]:指定网络设备的名称
\[设备名]：指定网络设备的名称


### netstat

**显示网络状态**

netstat [-tulpn] [-A网络类型][--ip]

-t(--tcp)：显示tcp传输协议的连线状况
-u(--udp):显示udp传输协议的连线状况
-l(--listening):显示监控中的服务器socket
-p(--programs):显示正在使用socket的程序识别码和程序名称
-n(--numeric):直接使用ip地址，不通过域名服务器

### ss(Socket Statistics)

**类似于netstat，但速度更快**
**用于显示和分析系统的网络链接状态**

-a, --all：显示所有的套接字，包括监听和非监听的。
-t, --tcp：仅显示 TCP 套接字。
-u, --udp：仅显示 UDP 套接字。
-l, --listening：仅显示监听状态的套接字。
-p, --processes：显示与套接字关联的进程信息。
-n, --numeric：以数字形式显示地址，而不是解析成主机名。
-r, --resolve：将主机名解析为 IP 地址。
-s, --summary：显示套接字的摘要信息。
-4：仅显示 IPv4 套接字。
-6：仅显示 IPv6 套接字。
-i, --info：显示详细的内部信息。
-o, --options：显示 TCP 计时器信息。
-K, --kill：通过 ID 杀死指定的 socket。
-f, --family=FAMILY：指定协议族（如 inet, inet6, unix, link）。
-m, --memory：显示每个套接字使用的内存。
-H, --no-header：不显示标题行。
-v, --verbose：显示详细的输出。

### ps(process status)

**用于显示当前进程的状态**

ps [options] [--help]

-A:列出所有的进程
-w：显示加宽可以显示较多的资讯
-au：显示较详细的资讯
-aux：显示所有包含其他使用者的进程

### uname(unix name)

**用于显示操作系统信息，例如内核版本、主机名等**

uname [-amnrsv] [--help] [--version]

-a：显示全部信息
-m(--machine):显示处理器类型
-n(--nodename):显示主机名
-r(--release):显示内核版本号
-v：显示操作系统的版本
-p：显示处理器类型==-m

### cat /etc/passwd 

**枚举系统用户，筛选可登录账号**

### cat /etc/*-release

**查看发行版本**

### cat /etc/shadow 

**如可读，直接获取hash**

### w(who)

**查看当前在线用户**

### last(lastlog)

**查看历史登录记录**

### route

**用于查看和操作ip路由表**

-n:查看完整路由表
add (-host/-net) + ip：添加特定主机路由/添加到网络路由
del (-net/default):删除特定路由/删除默认网关

### grep -r "password" /etc/ 

**从配置文件中递归搜索硬编码密码**

### find / -perm -4000 -type f 2>/dev/null

**扫描全系统SUID文件**

## 文本处理

### sort

**用于将文本内容加以排序**

sort [-bcdfimMnr][-o<输出文件>][-t<分隔字符>][+<起始栏位>-<结束栏位>][--help][--verison][文件][-k field1[,field2]]

-b 忽略每行前面开始出的空格字符。
-c 检查文件是否已经按照顺序排序。
-d 排序时，处理英文字母、数字及空格字符外，忽略其他的字符。
-f 排序时，将小写字母视为大写字母。
-i 排序时，除了040至176之间的ASCII字符外，忽略其他的字符。
-m 将几个排序好的文件进行合并。
-M 将前面3个字母依照月份的缩写进行排序。
-n 依照数值的大小排序。
-u 意味着是唯一的(unique)，输出的结果是去完重了的。
-o<输出文件> 将排序后的结果存入指定的文件。
-r 以相反的顺序来排序。
-t<分隔字符> 指定排序时所用的栏位分隔字符。
+<起始栏位>-<结束栏位> 以指定的栏位来排序，范围由起始栏位到结束栏位的前一栏位。
--help 显示帮助。
--version 显示版本信息。
[-k field1[,field2]] 按指定的列进行排序。

### uniq

**用于检查删除文本内容中重复出现的行列，一般与sort命令结合使用**

uniq [-cdu][-f<栏位>][-s<字符位置>][-w<字符位置>][--help][--version][输入文件][输出文件]

-c或--count 在每列旁边显示该行重复出现的次数。
-d或--repeated 仅显示重复出现的行列。
-f<栏位>或--skip-fields=<栏位> 忽略比较指定的栏位。
-s<字符位置>或--skip-chars=<字符位置> 忽略比较指定的字符。
-u或--unique 仅显示出一次的行列。
-w<字符位置>或--check-chars=<字符位置> 指定要比较的字符。
--help 显示帮助。
--version 显示版本信息。
[输入文件] 指定已排序好的文本文件。如果不指定此项，则从标准读取数据；
[输出文件] 指定输出的文件。如果不指定此选项，则将内容显示到标准输出设备（显示终端）。

### sed

**利用脚本来处理文本文件**

sed [-hnV][-e<script>][-f<script文件>][文本文件]

-e<script>或--expression=<script> 以选项中指定的script来处理输入的文本文件。
-f<script文件>或--file=<script文件> 以选项中指定的script文件来处理输入的文本文件。
-h或--help 显示帮助。
-n或--quiet或--silent 仅显示script处理后的结果。
-V或--version 显示版本信息。


### strings

**用于在对象文件或二进制文件中查找可打印字符串**

strings [options] [file]

-a 或 --all：扫描整个文件，而不仅仅是目标文件的初始化和装载段。
-f 或 --print-file-name：在显示字符串前先显示文件名。
-n 或 --bytes=[number]：设置最小字符串长度，默认是4个字符。
-t 或 --radix={o,d,x}：输出字符的位置，基于八进制、十进制或十六进制。
-e 或 --encoding={s,S,b,l,B,L}：选择字符大小和排列顺序，例如 s 表示7-bit，S 表示8-bit，b 和 l 表示16-bit，B 和 L 表示32-bit。

