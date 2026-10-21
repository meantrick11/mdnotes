

# Haystack：创建自定义 Document Store

> 官方文档：https://docs.haystack.deepset.ai/docs/creating-custom-document-stores
>
> 文档版本：Haystack 3.3
>
> 类型：官方文档主要内容中文翻译与整理（非逐字全文翻译）

## 1. 概述

Haystack 允许开发者创建自己的 Document Store（文档存储），以管理文档数据。

当 Haystack 已有的 Document Store 无法满足实际需求时，可以自行实现自定义 Document Store。

典型场景包括：

1. 使用 Haystack 尚未支持的向量数据库。
2. 需要特定的文档检索策略。
3. 希望自定义 Haystack 读取、写入文档的行为。

与自定义 Component 类似，只要 Python 程序能够导入相应代码，就可以在 Haystack 应用中使用自定义 Document Store。

**最佳实践：将自定义 Document Store 打包为独立的集成包（Integration Package）。**

---

## 2. 开发建议（Recommendations）

Haystack 对自定义 Document Store 提供了若干推荐规范。

这些规范不是强制要求，但遵循规范可以提高一致性、可维护性和可发现性。

### 2.1 命名规范（Naming Convention）

推荐使用以下格式命名：

`<TECHNOLOGY>-haystack`

例如：

`chroma-haystack`

该命名规范同时适用于：

- Git 仓库名称。
- Python 包名称。

示例：

```text
Git Repository:
https://github.com/your-org/example-haystack

Python Package:
example-haystack
```

统一的命名方式能够减少用户的理解成本，也方便其他开发者查找集成包。

### 2.2 项目结构（Structure）

Document Store 的实现可能较复杂，因此官方建议使用独立的 Git 仓库维护。

Haystack 提供了一个 GitHub 项目模板，帮助开发者快速建立自定义 Document Store 项目。

模板包含：

- Python 包结构。
- 测试所需的基础代码。
- 打包配置。
- 发布配置。
- 项目基础文件。

相关资源：

- [Haystack 集成项目模板](https://github.com/deepset-ai/haystack-core-integrations)
- [官方文档中的模板入口](https://docs.haystack.deepset.ai/docs/creating-custom-document-stores)

使用模板可以减少从零搭建 Python 项目基础设施的成本。

### 2.3 打包与分发（Packaging）

自定义 Document Store 和其他 Haystack Integration 一样，可以作为额外的 Python 包安装。

假设已经建立了：

- Git 仓库。
- `pyproject.toml`。
- Python 包结构。

可以直接从 Git 仓库安装：

```bash
pip install git+https://github.com/your-org/example-haystack.git
```

对于原型开发，这种方式已经足够。

如果希望其他开发者长期使用，官方建议将包发布到 PyPI。

发布后可以直接安装：

```bash
pip install example-haystack
```

官方提供的 GitHub 模板还包含自动发布到 PyPI 的 GitHub Actions 工作流。

### 2.4 文档（Documentation）

推荐为自定义 Document Store 编写完整的使用文档。

至少应该包含：

- README。
- 安装方式。
- 配置方式。
- 基本使用示例。
- API 使用说明。

对于较复杂的项目，可以使用静态文档生成器构建 API 文档。

参考项目：

[neo4j-haystack](https://github.com/neo4j/neo4j-haystack)

---

## 3. 实现（Implementation）

### 3.1 DocumentStore Protocol

Haystack 不要求 Document Store 必须继承某个特定的基类。

**任何 Python 类，只要实现 DocumentStore Protocol 所要求的方法，就可以作为 Document Store。**

核心接口如下：

```python
from typing import Protocol, Dict, Any, Optional, List
from haystack import Document
from haystack.document_stores.types import DuplicatePolicy


class DocumentStore(Protocol):

    def to_dict(self) -> Dict[str, Any]:
        """
        将 Document Store 序列化为字典。
        """
        ...

    @classmethod
    def from_dict(
        cls,
        data: Dict[str, Any]
    ) -> "DocumentStore":
        """
        从字典反序列化 Document Store。
        """
        ...

    def count_documents(self) -> int:
        """
        返回当前存储的文档数量。
        """
        ...

    def filter_documents(
        self,
        filters: Optional[Dict[str, Any]] = None,
    ) -> List[Document]:
        """
        根据过滤条件返回符合要求的文档。
        """
        ...

    def write_documents(
        self,
        documents: List[Document],
        policy: DuplicatePolicy = DuplicatePolicy.FAIL,
    ) -> int:
        """
        写入文档，并返回成功写入的文档数量。
        """
        ...

    def delete_documents(
        self,
        document_ids: List[str]
    ) -> None:
        """
        根据文档 ID 删除对应文档。
        """
        ...
```

说明：

以上为官方 Protocol 的结构化展示，包含六个接口。

其中：

| 方法 | 作用 |
|---|---|
| `to_dict()` | 序列化 |
| `from_dict()` | 反序列化 |
| `count_documents()` | 统计文档数量 |
| `filter_documents()` | 按条件筛选文档 |
| `write_documents()` | 写入文档 |
| `delete_documents()` | 删除文档 |

其中四个方法负责基本的文档数据操作：

1. `count_documents`
2. `filter_documents`
3. `write_documents`
4. `delete_documents`

另外两个方法负责序列化与反序列化。

Haystack 的通用组件，例如 `DocumentWriter`，依赖这些标准接口与 Document Store 进行交互。

### 3.2 为什么使用 Protocol？

> 本节为 Python 工程概念补充。

Python 的 Protocol 提供结构化类型约束。

与传统的类继承不同，Protocol 关注的是：

**对象是否具有指定的方法和行为，而不是对象是否继承某个基类。**

因此，开发者不必继承 Haystack 的特定存储实现类。

只需要实现相应的方法，即可让对象符合协议要求。

这有利于：

- 降低组件之间的耦合。
- 支持不同数据库实现。
- 保持统一的数据访问接口。
- 扩展新的存储技术。

---

## 4. 扩展方法（Additional Methods）

除了 Protocol 中规定的方法之外，Document Store 还可以实现额外的搜索和管理功能。

官方强调：

**扩展方法不属于 DocumentStore Protocol，因此不要求遵循统一的方法签名。**

### 4.1 常见扩展方法

```python
def delete_all_documents(
    recreate_index: bool = False
):
    ...
```

作用：删除全部文档，并可根据参数重新创建索引。

```python
def update_by_filter(
    filters: dict[str, Any],
    meta: dict[str, Any],
    refresh: bool = False
) -> int:
    ...
```

作用：根据过滤条件更新文档元数据，返回更新数量。

```python
def delete_by_filter(
    filters: dict[str, Any]
) -> int:
    ...
```

作用：根据过滤条件批量删除文档。

这些方法虽然不是强制要求，但官方强烈建议实现，因为用户通常希望 Document Store 提供这些操作能力。

### 4.2 自定义搜索方法

不同数据库具有不同的检索能力。

例如：

- 向量相似度搜索。
- 全文检索。
- 混合检索。
- 特定索引搜索。
- 数据库专有的过滤策略。

Haystack 不会强制所有 Document Store 使用完全相同的搜索接口。

开发者可以根据底层数据库的特点自行设计方法签名。

例如：

```python
def search(
    query_embedding,
    top_k=10,
    filters=None,
):
    ...
```

上述方法仅作为扩展接口的示意，并非官方统一 API。

通常由专门的 Retriever 组件调用这些额外的搜索方法。

---

## 5. Retriever（检索器）

如果希望充分使用自定义 Document Store，通常还需要实现一个或多个配套的 Retriever。

### 5.1 Document Store 与 Retriever 的职责

**Document Store：**

负责文档的存储、管理，以及具体的数据访问和搜索能力。

**Retriever：**

负责接收检索输入，调用 Document Store 的相关搜索方法，并输出检索结果。

例如：

```text
用户查询
   |
   v
Retriever
   |
   | 调用 search()
   v
Custom Document Store
   |
   v
底层数据库
   |
   v
返回相关 Documents
```

### 5.2 实现方式

Retriever 本质上是一个标准的 Haystack Component。

因此，可以按照 Haystack 自定义组件的方式实现。

官方参考：

[Creating Custom Components](https://docs.haystack.deepset.ai/docs/custom-components)

官方也建议遵循 Retriever 的命名规范，以便其他开发者理解组件的用途。

### 5.3 工程理解

> 本节为补充说明。

Document Store 和 Retriever 的关系可以理解为：

- Document Store：负责数据存储及数据库访问逻辑。
- Retriever：负责将检索能力封装为 Haystack Pipeline 可以使用的组件。

例如，可以分别构建：

- BM25Retriever
- EmbeddingRetriever
- HybridRetriever

它们可以依赖同一个 Document Store，但采用不同的检索策略。

---

## 6. 序列化（Serialization）

Haystack 要求组件能够正确进行序列化和反序列化。

某些组件，例如 Retriever、DocumentWriter，会持有 Document Store 实例的引用。

因此，Document Store 需要实现：

- `to_dict()`
- `from_dict()`

这样才能在保存和加载 Pipeline 时重新构建 Document Store。

### 6.1 序列化应该保存什么？

假设创建了一个数据库客户端：

```python
client = DatabaseClient(
    host="127.0.0.1",
    database="knowledge"
)
```

序列化时应该保存：

- 数据库地址。
- 数据库名称。
- 其他必要的初始化配置。

而不是直接保存数据库客户端实例。

原因：

数据库客户端实例通常包含运行时连接和内部状态，并不适合作为可移植的配置数据进行序列化。

### 6.2 序列化示例

> 以下代码为辅助理解示例，并非官方完整实现。

```python
class CustomDocumentStore:

    def __init__(self, host: str, database: str):
        self.host = host
        self.database = database

    def to_dict(self):
        return {
            "host": self.host,
            "database": self.database,
        }

    @classmethod
    def from_dict(cls, data):
        return cls(
            host=data["host"],
            database=data["database"],
        )
```

实际 Haystack 项目还需要按照其序列化约定保存组件类型和初始化参数等信息。

---

## 7. 敏感信息管理（Secrets Management）

创建 Document Store 时，通常需要传递敏感参数：

- 数据库密码。
- API Key。
- 私有服务地址。
- 其他认证信息。

如果这些信息直接以明文形式传递或写入序列化结果，就可能发生泄露。

因此，Haystack 提供了 `Secret` 机制。

该机制用于包装敏感信息，并避免它们在序列化、反序列化过程中意外泄露。

官方强烈建议：

**在自定义 Document Store 中使用 Haystack 的 Secret 机制管理敏感数据。**

参考：

[Secret Management](https://docs.haystack.deepset.ai/docs/secret-management)

---

## 8. 测试（Testing）

Haystack 提供了专门针对 Document Store 的测试工具。

### 8.1 DocumentStoreBaseTests

开发者可以通过继承：

`DocumentStoreBaseTests`

运行 Document Store 应满足的标准测试。

通过这些测试，是确保自定义 Document Store 与 Haystack 生态保持一致的最低要求。

### 8.2 分阶段实现与测试

官方建议不要一次性完成所有接口。

可以逐步实现、逐步测试。

**第一步：实现 count_documents**

```python
def count_documents(self) -> int:
    ...
```

然后在测试模块中：

```python
class TestDocumentStore(CountDocumentsTest):
    ...
```

运行对应测试，直到测试通过。

**第二步：实现 write_documents**

```python
def write_documents(self, documents, policy):
    ...
```

同时增加对应的测试 Mixin：

```python
class TestDocumentStore(
    CountDocumentsTest,
    WriteDocumentsTest
):
    ...
```

随后继续实现和验证剩余接口。

这种方式可以避免一次性引入大量问题，也方便定位故障。

### 8.3 扩展测试

Haystack 还提供：

`DocumentStoreBaseExtendedTests`

这些测试覆盖用户常用的扩展功能，例如：

- `delete_all_documents`
- `update_by_filter`

虽然并非最低要求，但官方建议尽可能兼容扩展测试，以增强 Document Store 的完整性。

---

## 9. 实现建议（Implementation Tips）

### 9.1 参考现有实现

官方推荐阅读现有 Document Store 的源码，例如：

- `InMemoryDocumentStore`
- `ElasticsearchDocumentStore`

通过这些实现，可以了解：

- Document Store 的代码组织方式。
- 如何实现标准接口。
- 如何处理文档读写。
- 如何集成数据库搜索能力。

### 9.2 从基础 CRUD 开始

推荐开发顺序：

1. 实现 `count_documents`。
2. 编写和运行对应测试。
3. 实现 `write_documents`。
4. 增加写入测试。
5. 逐步完善过滤和删除接口。
6. 最后完善扩展功能。

实际开发中还需完成序列化、连接管理等必要能力。

### 9.3 提供 Notebook 示例

官方建议提供一个可以直接运行的 Notebook。

例如：

- 初始化自定义 Document Store。
- 写入文档。
- 创建 Retriever。
- 构建 Haystack Pipeline。
- 执行一次完整检索。

这样不仅可以作为使用示例，也有利于其他开发者理解和采用该集成。

可以参考：

[haystack-cookbook](https://github.com/deepset-ai/haystack-cookbook)

### 9.4 支持异步操作

如果底层数据库支持异步访问，官方建议同时提供异步版本的接口方法。

这样能够使应用和 Pipeline 更充分地利用异步能力，并改善特定场景下的吞吐和可扩展性。

需要注意：

异步接口不意味着每个查询必然更快，其优势主要取决于 I/O 模型和并发负载。

---

## 10. 发布到 Haystack Integrations

当自定义 Document Store 已经可以正常使用，并完成打包后，可以考虑将其提交到 Haystack 的集成目录。

操作方式：

1. 完成 Document Store 的开发。
2. 补充使用文档和测试。
3. 打包并发布集成。
4. 向 `haystack-integrations` 仓库提交 Pull Request。
5. 申请将集成展示在 Haystack Integrations 页面中。

相关链接：

- [Haystack Integrations](https://haystack.deepset.ai/integrations)
- [haystack-integrations GitHub](https://github.com/deepset-ai/haystack-integrations)

---

## 11. 核心知识总结

> 以下为根据官方内容整理的工程总结。

### 11.1 自定义 Document Store 的能力层次

| 能力层次 | 主要内容 | 是否必需 |
|---|---|---|
| 基础数据操作 | count、filter、write、delete | 必需 |
| 序列化 | to_dict、from_dict | 必需 |
| 高级管理 | 批量更新、条件删除、清空存储 | 推荐 |
| 检索能力 | 向量、全文、混合检索 | 按需 |
| Retriever | 封装为 Pipeline 检索组件 | 通常需要 |
| 安全管理 | Secret 机制 | 强烈推荐 |
| 测试 | DocumentStoreBaseTests | 最低兼容性验证要求 |
| 异步接口 | async 数据库操作 | 按底层能力推荐 |
| 打包发布 | Python Package、PyPI | 对外分发时推荐 |

### 11.2 什么情况下需要自定义 Document Store？

通常只有当现有集成无法满足需求时，才有必要自定义。

例如：

- Haystack 没有支持当前数据库。
- 需要数据库特有的检索行为。
- 需要特殊的文档读写逻辑。

如果已有 Document Store 可以满足业务需求，通常没有必要重新实现。

### 11.3 最重要的设计思想

Haystack 将文档管理和检索组件进行职责分离：

**Document Store 定义数据存储与访问能力，Retriever 将具体检索能力接入 Pipeline。**

同时通过 `DocumentStore Protocol` 约束基础操作，通过扩展方法保留底层数据库的差异化能力。

这让 Haystack 可以接入多种数据库，而不要求所有数据库提供完全相同的查询接口。

---

## 参考资料

1. [Creating Custom Document Stores](https://docs.haystack.deepset.ai/docs/creating-custom-document-stores)
2. [Document Store](https://docs.haystack.deepset.ai/docs/document-store)
3. [Creating Custom Components](https://docs.haystack.deepset.ai/docs/custom-components)
4. [Secret Management](https://docs.haystack.deepset.ai/docs/secret-management)
5. [Haystack Integrations](https://haystack.deepset.ai/integrations)
6. [Haystack Cookbook](https://github.com/deepset-ai/haystack-cookbook)
