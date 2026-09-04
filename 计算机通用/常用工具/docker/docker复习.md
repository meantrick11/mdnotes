### 1. 镜像与容器（先搞清这俩“物件”）
**面试嘴替**：“镜像（Image）是只读的模板，容器（Container）是镜像运行起来的沙箱进程。”

| 命令 | 一句话人话解释 |
| :--- | :--- |
| `docker pull <image>:<tag>` | 从镜像仓库（默认 Docker Hub）**下载**镜像，tag 不写默认 latest。 |
| `docker images` 或 `docker image ls` | 列出本地所有已下载的镜像。 |
| `docker rmi <image-id>` | **删除**本地镜像（如果有关联容器在跑，删不掉，要加 `-f`）。 |
| `docker ps` | 列出**正在运行**的容器。 |
| `docker ps -a` | 列出**所有**容器（含已停止的）。 |
| `docker rm <container-id>` | **删除**容器（建议先 stop，正在跑的要加 `-f`）。 |

---

### 2. 容器生命周期（日常起停核心）

| 命令 | 一句话人话解释 |
| :--- | :--- |
| `docker run <image>` | 基于镜像**创建并启动**一个新容器（最常用）。 |
| `docker run -it <image> /bin/bash` | 以**交互终端**方式进入容器 Shell（调试用）。 |
| `docker run -d <image>` | **后台（detach）** 模式运行容器，不占当前终端。 |
| `docker run --name my-nginx -p 8080:80 nginx` | 映射端口：宿主机 8080 → 容器内 80，并起个名字。 |
| `docker start <container>` | 启动一个**已停止**的容器。 |
| `docker stop <container>` | **优雅**停止运行中的容器（发 SIGTERM 信号，等它自己退出）。 |
| `docker kill <container>` | **强行**立即终止容器（发 SIGKILL，不给你反应时间）。 |
| `docker restart <container>` | 重启容器（= stop + start）。 |

---

### 3. ⭐ 排障与调试（面试必考救急三连）

| 命令 | 一句话人话解释 |
| :--- | :--- |
| `docker logs <container>` | 打印容器的标准输出日志（查启动报错的第一手段）。 |
| `docker logs -f <container>` | **实时追踪**日志（tail -f 效果）。 |
| `docker exec -it <container> /bin/bash` | **进入一个正在运行**的容器内部执行命令（调试神器，比 attach 安全）。 |
| `docker inspect <container>` | 查看容器的**全部元数据**（IP 地址、环境变量、挂载卷路径等，JSON 格式）。 |
| `docker stats` | 实时查看所有容器 CPU/内存/网络 IO 占用（性能排查）。 |
| `docker cp <file> <container>:/path` | 在宿主机和容器之间**拷贝文件**（调试临时用）。 |

> **面试陷阱**：`docker attach` 是直接进入容器主进程，容易卡死或误操作；**`exec` 是新建子进程**，安全得多，面试时务必说用 `exec`。

---

### 4. ⭐ Dockerfile 构建（面试重灾区，必背）

| 命令 | 一句话人话解释 |
| :--- | :--- |
| `docker build -t my-app:1.0 .` | 根据当前目录的 Dockerfile **构建镜像**，并打上标签。 |
| `docker build -f Dockerfile.dev .` | 指定非标准名的 Dockerfile 文件构建。 |

**Dockerfile 内部高频指令（面试默写级）**：

| 指令 | 一句话人话解释 | 面试考点 |
| :--- | :--- | :--- |
| `FROM` | 指定基础镜像（尽量用 alpine 精简版）。 | 基础层 |
| `WORKDIR /app` | 设置容器内的工作目录（后续命令都在这执行）。 | 约定位置 |
| `COPY . /app` | 把本地文件**复制**到镜像里（保留文件属性）。 | **⭐ 对比 ADD** |
| `ADD . /app` | 类似 COPY，但支持自动**解压 tar** 和 URL 下载。 | 面试金句：优先用 COPY，语义清晰，ADD 仅在需要解压时用。 |
| `RUN` | 构建镜像时执行的命令（比如 `apt-get install`）。 | 每层都会生成新镜像层 |
| `ENV` | 设置容器内环境变量。 | 传参 |
| `EXPOSE 80` | 声明容器监听端口（**仅声明**，不会实际映射，需配合 -p）。 | 文档作用 |
| `CMD ["nginx", "-g"]` | 容器**启动时**执行的命令，可被 `docker run` 后的参数**覆盖**。 | **⭐ 对比 ENTRYPOINT** |
| `ENTRYPOINT ["nginx"]` | 容器**启动时**执行的命令，`docker run` 后的参数会**追加**给它。 | 两者区别是面试 100% 题 |

> **面试金句（背下来）**：
> “CMD 和 ENTRYPOINT 都定义启动命令，但 **CMD 容易被覆盖，ENTRYPOINT 不容易**。最佳实践是 ENTRYPOINT 定义固定主程序，CMD 定义默认参数。比如 `ENTRYPOINT ["python"] CMD ["app.py"]`，运行时想加参数就 `docker run ... python script.py`。”

---

### 5. 数据持久化（Volumes 卷）

| 命令 | 一句话人话解释 |
| :--- | :--- |
| `docker volume create my-data` | 创建一个 Docker 管理的命名卷。 |
| `docker volume ls` | 查看所有卷。 |
| `docker run -v my-data:/app/data ...` | **绑定挂载**（旧语法）：将卷或宿主机目录挂进容器。 |
| `docker run --mount type=bind,source=/host,target=/container ...` | **挂载**（新推荐语法，更明确）。 |
| `docker volume inspect my-data` | 查看卷在宿主机上的实际物理路径。 |

> **面试区分**：
> - **Bind Mount（绑定挂载）**：把宿主机绝对路径挂进容器（开发时热更新用）。
> - **Volume（卷）**：Docker 自己管理路径（生产环境数据持久化用，权限更安全）。

---

### 6. 网络（容器间通信）

| 命令 | 一句话人话解释 |
| :--- | :--- |
| `docker network ls` | 查看所有网络（bridge/host/none/自定义）。 |
| `docker network create my-net` | 创建自定义桥接网络（容器之间可通过**容器名**互 ping）。 |
| `docker run --network my-net ...` | 让容器加入指定网络。 |
| `docker network inspect my-net` | 查看该网络下挂了哪些容器及 IP。 |

> **面试嘴替**：“容器之间通信，**不要用 IP**（IP 会变），推荐用自定义网络，通过**容器名称**做 DNS 解析互访。”

---

### 7. ⭐ 仓库与镜像分发

| 命令 | 一句话人话解释 |
| :--- | :--- |
| `docker login` | 登录 Docker Hub 或私有仓库。 |
| `docker tag my-app:1.0 myuser/my-app:1.0` | 给镜像打一个**新标签**（相当于重命名，指向同一个镜像 ID）。 |
| `docker push myuser/my-app:1.0` | **上传**镜像到远程仓库。 |
| `docker save -o my-app.tar my-app:1.0` | 将镜像**导出为 tar 压缩包**（离线迁移用）。 |
| `docker load -i my-app.tar` | 从 tar 包**导入**镜像到本地。 |

---

### 8. Docker Compose（多容器编排）

| 命令 | 一句话人话解释 |
| :--- | :--- |
| `docker-compose up -d` | 根据 `docker-compose.yml` 在后台**启动**所有服务。 |
| `docker-compose down` | **停止并删除**所有容器和网络（数据卷默认保留）。 |
| `docker-compose down -v` | 停止并**连数据卷一起删**（全清空，慎用）。 |
| `docker-compose logs -f` | 查看所有服务的聚合日志。 |
| `docker-compose restart` | 重启所有服务。 |
| `docker-compose exec <service> bash` | 进入某个服务的容器内部（同 docker exec）。 |

---

