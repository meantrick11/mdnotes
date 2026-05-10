# DOCKER 使用教程

## 基本概念：

**镜像**：
**容器**：
**镜像仓库**：

## docker的安装

docker是基于linux系统环境（利用linux的Cgroups功能来限制和隔离进程的资源[cpu、内存等]使用,Namespaces功能用来隔离进程的资源视图[只能使容器看到自己的进程，而看不到宿主机的]来运行的，当进入容器内部，docker表现得像是一个独立的linux系统，windows和mac都需要虚拟一个linux系统然后才能安装并运行

windows步骤：
1. 打开电脑的虚拟器服务
2. 打开终端运行一下命令：
```
wsl --set-defalut-version 2 设置wsl版本为2
wsl --upgate #安装wsl
```
3. 到www.docker.com官网下载对应的桌面docker程序
4. 打开对应的docker应用，在使用docker时，就一定要打开docker desktop应用

## docker的基本命令

1. `docker pull docker.io/library/ngix:latest`,docker.io表示docker仓库的注册表（官方仓库可以省略），library部分表示命名空间（作者的名字），ngix表示对应的docker镜像名，latest部分表示版本号，默认为最新版本

2. `docker images`列出所有下载过的镜像

3. `docker rmi ngix(id)`表示删除ngix镜像

4. `docker pull -- platform=xxxxx nginx`表示拉取对应cpu架构的镜像，默认最合适当前设备的架构

5. `docker run [options] [镜像名：镜像id] [command] [参数]`，会按指定镜像新建一个容器，如果镜像没有被拉取pull，那么docker会默认自动拉取最新的镜像
> -i(--interactive)表示标准输入打开，容器可以接收你的输入
> -t(--tty) 分配一个伪终端，-it表示直接进入容器的交互式终端，输入exit退出
> -d(--detach) 不占用当前终端，只返回容器id
> --name 指定容器名（避免每次操作都要输入容器id）
> -p(--publish)宿主机端口:容器内端口 端口映射，打开本地端口，让外部可以访问容器服务（默认docker的内部网络服务是隔离的，无法被宿主机/其他docker访问），如果使用 `docker run -p 80:80 ngix`，就会将容器的端口映射到宿主机对应的端口
> -v 宿主机目录:容器内目录 将宿主目录和容器内目录绑定，这个文件被两者共享，并且删除容器的时候，对应的挂载卷不会被清除，可以做数据持久化的管理
>-e 指定环境变量，比如`dockere run -e 容器环境变量的值 ngix`表示指定对应镜像中可以有的环境变量的值，可以在dockerhub的官网或者镜像的使用文档中找到
>--rm 表示在容器停止的时候就把容器删除掉
>--restart always 配置重启策略，这里的always表示只要容器停止就重启 unless-stopped，除了always的手动停止外都重启
>--network 子网名 指定容器加入到某个子网段

6. `docker ps(process status)`列出所有容器的状态，包含容器id、image等等内容,只能列出正在运行的命令，如果要看所有容器就得添加 -a参数列出所有容器

7. `docker rm -f(强制删除) 容器名/id`表示删除某个容器


8. `docker volume create ngix_html`在本地创建一个docker的挂载卷
> `docker run -v ngix_html:容器内挂载卷 ngix`，这里将在本地创建的挂在卷挂载到容器内部
>`docker volume inspect ngix_html`表示查看ngix_html在宿主机中的具体位置
>`docker volume list`列出所有挂载卷
>`docker volume rm ngix_html`删除具体的卷名
>`docker volume prune -a`删除所有没有容器使用的挂载卷

9. `docker start/stop 容器名/id` 启停容器

10. `docker inspect 容器名/id`列出容器的详细配置信息：端口绑定等等

11. `docker create ...`和run命令很像，但是不立即运行容器，只是创建静默

12. `docker logs 容器名/id` 查看容器日志

13. `docker exec 容器名/id ps -ef`在容器中执行命令
> `docker exec -it 容器名/id /bin/sh`进入对应的容器内部
>**由于docker尽可能简化容器的占用，所以大多只有所需的工具命令，如果要新安装，那么就先执行上述命令进入docker内部，然后运行对应的安装包安装(比如：apt install或者winget的包安装命令，可以运行cat /etc/os-release查看当前系统发行版)**

## dockerfile

dockerfile可以被视作制作镜像的操作过程
```
# 1. 指定基础镜像（必须是第一行）
FROM openjdk:17-jdk-slim

# 2. 作者信息（可选）
MAINTAINER 你的名字 <你的邮箱>

# 3. 设置工作目录（后续命令都在这个目录执行）
WORKDIR /app

# 4. 复制宿主机的 jar 包到容器内 /app 目录
COPY target/demo.jar app.jar

# 5. 暴露容器端口（只是声明，不实际发布端口）
EXPOSE 8080

# 6. 容器启动时执行的命令
ENTRYPOINT ["java", "-jar", "app.jar"]
```

`docker build -t 镜像名(可以是用户名+镜像名):12.1.0 .(当前文件夹)`在当前文件夹创建镜像

`docker run ...`可以用来测试对应的镜像

`docker tar 本地镜像名:版本号 dokerhub用户名/镜像名:版本号`给镜像打标签

`docker login`按照流程在终端中登录你的docker账户

`docker push 用户名/镜像名：12.1.0 `将镜像推送到dockerhub

`docker pull docker.io/用户名/镜像名`就可以直接拉取你创建的镜像使用

## docker 网络

`docker network list`展示出所有的网络：name，id，模式

通常是桥接模式，是内部虚拟网桥 docker0（172.17.0.1/16），容器在一个私有子网里就是创建的容器在
`docker network create network1`创建子网，然后可以将容器移动到某一个子网中，跨子网不可以通信（因为网桥不一样了，可以配置同一网桥，则可以达到互通的目的），同一容器还可以指定容器名直接通信

host模式，容器直接共享宿主机的地址和各个端口

none模式：不进行网络通信，只能宿主机控制

## docker compose



