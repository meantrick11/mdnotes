### 🚀 环境搭建：使用 `uv` 极速开始

`uv` 是一个用 Rust 写的极速 Python 包管理器，用它来管理 `chromadb` 会非常顺畅。

1.  **安装 `uv`**：根据你的系统，在终端执行以下命令：
    *   **Windows (PowerShell)**:
        ```powershell
        powershell -c "irm https://astral.sh/uv/install.ps1 | iex"
        ```
    *   **macOS / Linux**:
        ```bash
        curl -LsSf https://astral.sh/uv/install.sh | sh
        ```

2.  **创建项目并安装 `chromadb`**：
    `uv` 会自动处理虚拟环境，无需手动创建和激活。
    ```bash
    # 初始化项目
    uv init chroma-lab
    cd chroma-lab

    # 添加 chromadb 依赖
    uv add chromadb
    ```
    这条命令会修改你的 `pyproject.toml` 文件，并自动安装所有依赖。

### 🏗️ ChromaDB 核心概念

在写代码前，先理解两个最核心的概念：

*   **Client (客户端)**：这是你与 ChromaDB 交互的入口。根据场景不同，有几种模式。
*   **Collection (集合)**：可以把它想象成关系型数据库中的“表”。所有文档（Documents）都存储在集合中，每个集合都有自己的索引。

### ✍️ 代码实战：从入门到进阶

#### 1. 客户端 (Client) 的三种模式

根据数据是否需要持久化，选择合适的客户端。

*   **模式一：内存模式 (`Client()`)**
    数据只存在于内存中，程序重启后数据会丢失。**适用于快速原型验证和单元测试**。
    ```python
    import chromadb
    client = chromadb.Client()
    ```

*   **模式二：本地持久化模式 (`PersistentClient(path)`)**
    数据会保存到你指定的本地目录，程序重启后数据依然存在。**适用于单机开发和小规模生产**。
    ```python
    import chromadb
    # 数据会保存在 ./my_chroma_data 目录下
    client = chromadb.PersistentClient(path="./my_chroma_data") 
    ```

*   **模式三：远程服务模式 (`HttpClient(host, port)`)**
    客户端连接到独立运行的 Chroma 服务。**适用于团队共享或微服务架构**。
    ```python
    import chromadb
    client = chromadb.HttpClient(host="localhost", port=8000)
    ```

#### 2. 集合 (Collection) 的操作

*   **创建集合**：使用 `create_collection`，如果集合已存在会报错。
    ```python
    collection = client.create_collection(
        name="my_knowledge_base",
        metadata={"hnsw:space": "cosine"} # 可选，指定距离算法，默认是 "l2"
    )
    ```
    > **参数说明**：
    > *   `name`：集合的唯一名称（必填）。
    > *   `metadata`：集合的元数据，可以配置索引参数，如 `"hnsw:space": "cosine"` 使用余弦相似度。

*   **获取或创建集合**：推荐使用 `get_or_create_collection`，更加安全。
    ```python
    collection = client.get_or_create_collection(name="my_knowledge_base")
    ```

*   **查看所有集合**：
    ```python
    print(client.list_collections())
    ```

#### 3. 增删改查 (CRUD) 操作

*   **添加文档 (Add)**
    `chromadb` 会自动为你的文档生成向量（嵌入）。
    ```python
    collection.add(
        documents=[
            "Python 的 GIL 是全局解释器锁，它让多线程变成伪并行。",
            "uv 是一个用 Rust 写的 Python 包管理器，比 pip 快 100 倍。",
            "向量数据库通过 Embedding 模型把文本变成高维向量，实现语义搜索。"
        ],
        metadatas=[
            {"topic": "Python", "source": "docs"},
            {"topic": "Tool", "source": "astral.sh"},
            {"topic": "VectorDB", "source": "chroma"}
        ],
        ids=["doc1", "doc2", "doc3"] # ID 是必填的，且必须唯一
    )
    print(f"集合中现有 {collection.count()} 条文档。")
    ```

*   **查询文档 (Query)**
    这是 ChromaDB 的核心功能——语义搜索。
    ```python
    results = collection.query(
        query_texts=["怎么管理 Python 依赖包比较好？"], # 查询文本
        n_results=2, # 返回最相似的两条结果
        # include=["documents", "metadatas", "distances"] # 控制返回哪些字段
    )

    # 打印结果
    for i, (doc_id, doc, meta, dist) in enumerate(zip(
        results["ids"][0], 
        results["documents"][0], 
        results["metadatas"][0], 
        results["distances"][0]
    )):
        print(f"结果 #{i+1} | ID: {doc_id} | 距离: {dist:.4f}")
        print(f"  文档: {doc}")
        print(f"  元数据: {meta}")
    ```
    > **参数说明**：
    > *   `query_texts`：查询文本列表，Chroma 会自动将其向量化。
    > *   `n_results`：返回的结果数量。
    > *   `include`：指定返回结果中包含哪些字段。
    > *   `where`：根据元数据进行过滤。
    > *   `where_document`：根据文档内容进行过滤。

*   **更新或插入文档 (Upsert)**
    如果 ID 存在则更新，不存在则插入。
    ```python
    collection.upsert(
        documents=["更新后的文档内容"],
        ids=["doc1"]
    )
    ```

*   **删除文档 (Delete)**
    ```python
    collection.delete(ids=["doc2"])
    ```

### 🕵️‍♂️ 那些你必须知道的“隐形坑”

#### 坑一：Embedding 模型自动下载，占满 C 盘

这是新手最容易遇到的问题！**当你第一次调用 `collection.add()` 或 `collection.query()` 时，Chroma 会使用默认的 `all-MiniLM-L6-v2` 模型来生成向量**。这个模型大约 90MB，会被自动下载到你的用户缓存目录。

*   **默认下载位置**：
    *   **Windows**: `C:\Users\<你的用户名>\.cache\huggingface\hub`
    *   **macOS/Linux**: `~/.cache/huggingface/hub`
*   **解决方法**：
    1.  **更改缓存路径（推荐）**：在代码运行前设置环境变量，将模型下载到其他盘。
        ```python
        import os
        # 将模型缓存路径修改到 D 盘
        os.environ["HF_HOME"] = "D:/hf_cache" 
        # 或者更精确地只修改 hub 路径
        os.environ["HF_HUB_CACHE"] = "D:/hf_cache/hub" 
        ```
    2.  **使用其他 Embedding 模型**：你可以使用自己的或第三方的 Embedding 模型，甚至直接传入已经计算好的向量，从而完全避免模型下载。

#### 坑二：Python 版本与编译环境问题

*   **Python 版本**：请确保你的 Python 版本 >= 3.10。可以用 `uv python list` 查看，用 `uv python install 3.11` 安装。
*   **编译失败**：`chromadb` 依赖 `onnxruntime` 和 `hnswlib` 等库，在 Windows 上安装需要 **Visual C++ 运行时**。macOS 用户需要确保安装了 **Xcode Command Line Tools**。

#### 坑三：`sqlite3` 模块缺失

如果运行时报错 `ModuleNotFoundError: No module named '_sqlite3'`，说明你的 Python 编译时没有包含 sqlite 支持。解决方法是用 `uv python install 3.11` 重装一个官方版本的 Python。

#### 坑四：集合重复创建

如果重复调用 `create_collection` 会抛出 `ValueError: Collection already exists` 异常。解决方案是始终使用 `get_or_create_collection` 方法。

### 💎 总结与最佳实践

1.  **环境**：优先使用 `uv` 管理项目，它比 `pip` 快得多。
2.  **客户端**：开发测试用 `Client()`，单机项目用 `PersistentClient(path)`，团队协作用 `HttpClient()`。
3.  **集合**：始终使用 `get_or_create_collection` 来避免异常。
4.  **ID**：为每个文档提供唯一且具有业务意义的 ID，这是更新和删除操作的基础。
5.  **模型缓存**：在项目开始前，通过设置 `HF_HOME` 环境变量来指定模型下载目录，避免占满系统盘。
6.  **查询**：用好 `n_results`、`include`、`where` 等参数，可以实现非常灵活的搜索。

