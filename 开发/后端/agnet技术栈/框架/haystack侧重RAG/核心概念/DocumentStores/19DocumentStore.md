
你可以将文档存储库视为一个数据库，它用于存储你的数据，并在查询时将数据提供给检索器。了解如何在管道中使用文档存储库，或如何创建你自己的文档存储库。

文档存储是用于存储文档的对象。在 Haystack 中，文档存储与组件不同，因为它没有`run()`方法。你可以将其视为访问数据库的接口——你可以将信息存入其中，也可以从中检索信息。这意味着文档存储并非管道的组成部分，而是管道组件可访问并与之交互的工具。

在 Haystack 中使用文档存储（Document Store）最常见的方式是通过检索器（Retriever）获取文档。

Haystack 中的文档存储旨在将以下方法用作其协议的一部分：
- `count_documents` 以整数形式返回给定存储中存储的文档数量。
- `filter_documents` 返回一个与提供的筛选条件相匹配的文档列表。
- `write_documents` 会将文档写入或覆盖到指定的存储库中，并以整数形式返回写入的文档数量。
- `delete_documents` 会从文档库中删除所有带有给定 `document_ids` 的文档。
## 初始化

要在管道中使用文档存储库，必须先对其进行初始化。

不同的DocumentStore的初始化都不同，比如你可能使用的是向量数据库、关系型数据库、网络搜索等等不同种类RAG，所以初始化也是多样的。

## 处理文档

在将数据及其元数据和文档 ID 写入文档存储之前，先将其转换为`Document`对象。

ID 字段为必填项，因此如果你没有自行选择特定的 ID，Haystack 会尽力根据文档信息生成唯一 ID 并自动分配。**但由于 Haystack 是依据文档内容生成 ID，两份完全相同的文档可能会拥有相同的 ID。在更新文档时请留意这一点，因为 ID 不会自动更新。**

```python
document_store = ChromaDocumentStore()
documents = [
    Document(
        meta={"name": DOCUMENT_NAME}, id="document_unique_id", content="this is content"
    ),
    ...,
]
document_store.write_documents(documents)
```

要将文档写入`InMemoryDocumentStore`，只需调用`.write_documents()`函数：
```python
document_store.write_documents(
    [
        Document(content="My name is Jean and I live in Paris."),
        Document(content="My name is Mark and I live in Berlin."),
        Document(content="My name is Giorgio and I live in Rome."),
    ],
)
```


## 冲突处理(DuplicatePolicy)

`DuplicatePolicy`是一个类，用于定义在`DocumentStore`中处理具有相同 ID 的文档的不同选项。它有四种可能的值：
- None:`DocumentWriter` 使用的默认值。它依赖于`DocumentStore` 的设置，因此每个存储库都会应用其自身的策略。
- OWERWRITE:表示如果`DocumentStore`中已存在具有相同 ID 的文档，则应使用新文档覆盖它。
- SKIP:如果已存在具有相同 ID 的文档，新文档将被跳过且不会添加到`DocumentStore`中。
- FAIL:如果`DocumentStore`中已存在相同ID的文档，则引发错误。它可防止重复文档被添加。
```python
from haystack.document_stores.in_memory import InMemoryDocumentStore
from haystack.components.writers import DocumentWriter
from haystack.document_stores.types import DuplicatePolicy

document_store = InMemoryDocumentStore()
document_writer = DocumentWriter(
    document_store=document_store,
    policy=DuplicatePolicy.SKIP,
)
```

## Custom Document Store

所有自定义文档存储都必须实现[协议](https://github.com/deepset-ai/haystack/blob/13804293b1bb79743e5a30e980b76a0561dcfaf8/haystack/document_stores/types/protocol.py)，并包含四个必需方法：`count_documents`、`filter_documents`、`write_documents`和`delete_documents`。

`init`函数应指明所选数据库或向量存储的所有具体信息。

建议配备一个定制的对应检索器，以充分发挥特定文档存储的效能。