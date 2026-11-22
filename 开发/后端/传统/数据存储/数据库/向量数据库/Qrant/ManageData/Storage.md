一个集合内的所有数据都会被切分为**片段**。每个片段都有独立的向量和负载存储，同时也配有对应的索引。

存储在段中的数据通常不会重叠。不过，由于搜索包含去重机制，将同一个点存储在不同的段中不会引发问题。

根据所使用的存储类型和索引，段可以是`appendable`或`non-appendable`。你可以在`appendable`段中自由添加、删除和查询数据。而`non-appendable`段仅能读取和删除数据。

## 向量存储

Qdrant 始终将向量存储在磁盘上的[内存映射文件](https://en.wikipedia.org/wiki/Memory-mapped_file)中。根据应用程序的需求，你可以配置[内存层级](https://qdrant.tech/documentation/ops-configuration/memory-tiers/)来控制是否也将向量加载到 RAM 中以实现更快的访问：
- **`cached`** - Qdrant 在启动时会将文件预加载到磁盘缓存中，因此首次请求速度很快。这是默认设置；需要足够的内存来存储向量。
- **`cold`** - Qdrant 不会将文件预加载到内存中。首次请求可能会较慢，因为 Qdrant 会从磁盘读取数据，但操作系统会在页面被访问时对其进行缓存。

## 配置映射存储

将向量移动到`cold`层有两种方式：

- 为集合创建 API 中的向量设置 `memory` 选项：
```python
from qdrant_client import QdrantClient, models

client.create_collection(
    collection_name="{collection_name}",
    vectors_config=models.VectorParams(
        size=768, distance=models.Distance.COSINE, memory=models.Memory.COLD
    ),
)
```
- 设置 `memmap_threshold` 选项。此选项将设置一个阈值，达到该阈值后，段将被转换为内存映射存储。
```python
from qdrant_client import QdrantClient, models

client = QdrantClient(url="http://localhost:6333")

client.create_collection(
    collection_name="{collection_name}",
    vectors_config=models.VectorParams(size=768, distance=models.Distance.COSINE),
    optimizers_config=models.OptimizersConfigDiff(indexing_threshold=20000),
)
```

如果你的使用场景是均衡的——将 memmap 阈值设置为与`indexing_threshold`相同（默认值为 10000）。在这种情况下，优化器不会进行任何额外运行，而是一次性优化所有阈值。
如果你的写入负载较高且内存（RAM）较低，请将 memmap 阈值设置得低于 `indexing_threshold`，例如设为 5000。在这种情况下，优化器会先将片段转换为内存映射存储，之后再应用索引。


## Payload storage


Qdrant 为负载数据支持两种[内存层级](https://qdrant.tech/documentation/ops-configuration/memory-tiers/)：`cached`和`cold`（默认）。无论使用哪种层级，磁盘和[Gridstore](https://qdrant.tech/articles/gridstore-key-value-storage/)均用于数据持久化。

`cached`的有效载荷存储与`cached`向量的组织方式相同：Qdrant 在启动时会将其预加载到磁盘缓存中，因此访问速度很快。要让所有数据在内存中保持活跃可能需要大量空间，尤其是当有效载荷附带较大数值时——比如文本摘要甚至图片。

在有效载荷值较大的情况下，最好使用 `cold` 层级。该层级会直接向 Gridstore 读写有效载荷，无需预加载，因此无需占用大量内存来进行存储。不过，其缺点是访问延迟较高。如果需要基于有效载荷的条件查询向量——那么读取存储在磁盘上的值可能会耗费大量时间。在这种情况下，建议为过滤条件中用到的每个字段创建有效载荷索引，以避免磁盘访问操作。一旦创建了字段索引，无论有效载荷的存储类型如何，Qdrant 都会将索引字段的所有值保留在内存中。

## Versioning
为确保数据完整性，Qdrant 分两个阶段执行所有数据更改。第一步，数据会被写入预写日志（WAL），该日志会对所有操作进行排序并为其分配序列号。

一旦更改被添加到预写日志（WAL）中，即使发生断电也不会丢失。随后这些更改会被写入数据段。每个数据段会存储应用于它的更改的最新版本，以及每个单独数据点的版本。如果新更改的序列号小于该数据点的当前版本，更新程序将忽略该更改。这一机制使 Qdrant 能够在异常关闭的情况下，通过预写日志（WAL）安全高效地恢复存储。