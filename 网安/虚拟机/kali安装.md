# VMware虚拟机安装kali

## 方法一：直接使用预装版本
如果您不想自己下载镜像文件，可以直接使用预装版本。

到达kali官网，点击“Virtual Machines",
![kali官网](./pictures/kali官网.png)

然后选择第一个"Vmware",推荐的文件，点击下载
![pre7zip官网](./pictures/pre7zip官网.png)

找到已下载的7zip格式压缩的文件，然后复制到安装的目录下，最后解压
![压缩包](./pictures/pre7zip.png)

点击进入刚刚解压缩的文件夹中，找到带有.vmx后缀的文件，然后打开VMware Workstation,选择左上角的"文件",然后选择"打开",找到刚刚解压缩的文件夹，选择带有.vmx后缀的文件，双击打开

![打开虚拟机](./pictures/VMware虚拟机.png)
![打开虚拟机](./pictures/openpre.png)

然后就是打开虚拟机了,点击"开始此虚拟机"，

![打开虚拟机](./pictures/打开虚拟机.png)

到达虚拟机登录界面，初始登录账号和密码为kali/kali,登录后就可以开始使用kali了

![虚拟机登录界面](./pictures/amd64虚拟机.png)

登录后最终界面如下：
![kali登录界面](./pictures/最终界面.png)


## 方法二：使用kali镜像文件安装

首先，我们需要下载kali镜像文件，下载地址为：https://www.kali.org/get-kali/#kali-platforms，选择"Install Images"版本，然后开始下载

![kali官网](./pictures/kali官网.png)

下载完成后是一个.iso的镜像文件，不需要解压缩，直接复制到安装的目录下

![kali镜像文件](./pictures/kali镜像文件.png)

然后打开VMware Workstation,选择左上角的"文件",然后选择"新建虚拟机"

![VM新建虚拟机](./pictures/VM新建虚拟机.png)

界面：
![VM新建虚拟机界面](./pictures/VM新建虚拟机界面.png)

点击"下一步",然后选择"安装程序光盘映像文件",然后选择刚刚下载的kali镜像文件，点击"下一步",直到选择虚拟机安装的位置，这里可以手动选择安装位置

![kali安装位置选择](./pictures/kali安装位置.png)

然后持续点击"下一步"直到安装完成

![kali安装完成界面](./pictures/kali安装完成界面.png)

最后，点击"开启此虚拟机",然后上下键选择"install",进入安装界面

![kali安装界面](./pictures/kali安装界面.png)

根据需求进行选择并安装等等，下图未展示完全

![kali安装界面](./pictures/kaliinstalling.png)

输入账户
![输入账户](./pictures/输入账户.png)
磁盘分区选择
![磁盘分区选择](./pictures/安装磁盘选择.png)
软件选择,这里根据需要选择安装桌面，否则就只有一个命令行界面，也可以后续通过命令行安装桌面（楼主就是没有选择桌面，直接安装了）
![软件选择](./pictures/software选择.png)
选择完成后就是漫长的安装时间，等待完成后,安装引导程序
![安装引导程序](./pictures/引导程序.png)
选择/dev/sda作为引导程序
![选择引导程序](./pictures/引导程序2.png)
结束进程
![结束进程](./pictures/kali结束进程.png)

最后重启一下，输入在上述安装过程中设置的账户和密码，就可以使用了
![登录界面](./pictures/登录界面.png)





