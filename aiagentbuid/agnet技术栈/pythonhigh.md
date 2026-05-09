# python 进阶学习（针对于agent build

## day one

### 多线程（asyncio库）

好的，补充上 `async for` 和 `async with`，并对所有常用函数/关键字逐个解释。

---

### 一、关键字

#### `async def`
- **作用**：定义协程函数。调用时返回协程对象，不立即执行。
- **用法**：`async def my_coro(): ...`

#### `await`
- **作用**：等待一个可等待对象（协程、Task、Future）完成，并获取其返回值（如果有）。
- **用法**：`result = await some_coro()`

#### `async for`
- **作用**：异步迭代一个**异步可迭代对象**（如异步生成器、异步队列等）。每次迭代可能触发异步等待。
- **用法**：
  ```python
  async for item in async_iterable:
      process(item)
  ```

#### `async with`
- **作用**：异步上下文管理器。进入和退出时可能执行异步操作（如获取/释放锁、打开/关闭连接）。
- **用法**：
  ```python
  async with asyncio.Lock():
      # 临界区代码
  ```

---

### 二、常用函数（带参数解释）

#### 1. `asyncio.run(main, *, debug=None)`
- **解释**：创建新的事件循环，运行 `main` 协程，结束后关闭循环。是异步程序的统一入口。
- **参数**：
  - `main`：入口协程对象。
  - `debug`：可选，是否开启调试模式（`True/False`），默认 `None`（读取环境变量）。

#### 2. `asyncio.create_task(coro, *, name=None)`
- **解释**：将协程包装为 `Task` 并立即调度执行，返回 `Task` 对象。用于并发运行多个任务。
- **参数**：
  - `coro`：协程对象。
  - `name`：可选，任务名称，便于调试。

#### 3. `asyncio.gather(*aws, return_exceptions=False)`
- **解释**：并发执行多个可等待对象（协程、Task）。返回结果列表（顺序与传入顺序一致）。
- **参数**：
  - `*aws`：多个可等待对象（可变参数）。
  - `return_exceptions`：若为 `True`，异常会被当作结果返回；若为 `False`，遇到第一个异常即向上抛出。

#### 4. `asyncio.sleep(delay, result=None)`
- **解释**：非阻塞延时，常用来模拟异步耗时操作或让出控制权。
- **参数**：
  - `delay`：秒数（可为浮点数）。
  - `result`：延时结束后返回的值（默认为 `None`）。

#### 5. `asyncio.wait_for(aw, timeout)`
- **解释**：等待单个可等待对象完成，若超时则抛出 `TimeoutError` 并取消该任务。
- **参数**：
  - `aw`：可等待对象。
  - `timeout`：超时秒数（可为 `None` 表示无超时）。

#### 6. `asyncio.to_thread(func, /, *args, **kwargs)`
- **解释**：在独立线程池中运行普通（阻塞）函数，不阻塞事件循环。返回协程对象，需 `await`。
- **参数**：
  - `func`：普通函数。
  - `*args` / `**kwargs`：传递给函数的位置/关键字参数。

#### 7. `asyncio.Queue(maxsize=0)`
- **解释**：异步队列，用于协程之间安全地传递数据。
- **参数**：
  - `maxsize`：队列最大容量，`0` 表示无限。
- **常用方法**：
  - `put(item)`：放入数据。
  - `get()`：取出数据（若队列空则等待）。
  - `task_done()`：标记任务完成。
  - `join()`：等待所有任务完成。

#### 8. `asyncio.Lock()`
- **解释**：异步锁，保证同一时刻只有一个协程进入临界区。
- **参数**：无
- **常用方法**：
  - `acquire()`：获取锁。
  - `release()`：释放锁。
  - **推荐使用** `async with lock:` 自动管理。


如下代码的执行逻辑：

```python
async def fetch(url):
    print(f"Fetching {url}")
    await asyncio.sleep(2)
    return f"Data from {url}"

async def main():
    results = await asyncio.gather(
        fetch("url1.com"),
        fetch("url2.com")
    )
    print(results)

asyncio.run(main())
```

1. 先 run 调度中心
2. 开始运行 main 函数
3. 遇到 await，表示 main 接下来的代码要等 gather 运行完
4. 在 run 注册等待
5. gather 运行，创建任务1
6. 进入任务1，遇到 await，注册等待，放到后面
7. gather 再创建任务2
8. 任务2遇到 await，同样注册等待
9. 两个后台任务交给 run 调度、并发执行
10. run 调度完两个任务
11. gather 收集结果
12. 回到 main，执行 await 后面的代码
13. 最后打印结果

---

### 这里补充多进程、多线程、asyncio的对比表
| 特性                         | 多进程 (Multiprocessing)                             | 多线程 (Threading)                                  | asyncio                                                   |
|------------------------------|------------------------------------------------------|-----------------------------------------------------|-----------------------------------------------------------|
| 本质                         | 多个独立进程，各有独立内存和解释器                  | 一个进程内多个线程，共享内存                        | 单线程内多个协程，事件循环调度                            |
| 调度者                       | 操作系统（抢占式）                                   | 操作系统（抢占式）                                  | 事件循环（协作式，靠 `await` 让出）                      |
| 同一时刻执行 Python 代码     | 多个进程可以真正并行（多核）                         | 只有一个线程（GIL 限制）                            | 只有一个协程（单线程）                                    |
| 切换成本                     | 高（进程上下文切换，TLB 刷新）                      | 中（系统调用，内核态切换）                          | 极低（用户态函数调用，纳秒级）                            |
| 内存占用（每任务）           | ~10MB+（完整解释器副本）                            | ~1MB（线程栈）                                      | ~几KB（协程栈）                                           |
| 并发上限                     | 几百（受内存和调度限制）                            | 几千（线程数过多导致内存/调度崩溃）                 | 数十万（轻量协程）                                        |
| 数据共享                     | 复杂（需 IPC：队列、管道、共享内存）                | 简单但需要加锁（`threading.Lock`）                 | 无需锁（单线程无竞态条件）                               |
| 编程复杂度                   | 中等（需处理 IPC）                                  | 高（锁、死锁、竞态）                                | 中等（需 `async/await` 语法，全异步生态）                |
| 适用场景                     | CPU 密集型、利用多核                                | I/O 密集型（中等并发，任务数 < 几千）               | 超高并发 I/O 密集型（长连接、爬虫、WebSocket）           |

### with

with 语句背后是 Python 的上下文管理协议，该协议要求对象实现两个方法：

__enter__()：进入上下文时调用，返回值赋给 as 后的变量
__exit__()：退出上下文时调用，处理清理工作

### @的使用

#### 修饰器的使用

在 Python 中，“@” 符号主要用于装饰器。装饰器是一种强大且灵活的工具，用来在不修改函数或方法代码的情况下扩展或修改它们的行为。
```python
# Define a decorator function
def log_function_call(func):
    def wrapper(*args, **kwargs):
        print(f"Calling function {func.__name__} with arguments {args} and keyword arguments {kwargs}")
        result = func(*args, **kwargs)
        print(f"Function {func.__name__} returned {result}")
        return result
    return wrapper
 
# Apply the decorator to a function
@log_function_call
def add(a, b):
    return a + b
 
# Call the decorated function
result = add(5, 3)
print(result)
```

#### @符号作用二: 使用 “@” 操作符进行矩阵乘法
```python
import numpy as np

m1 = np.array([[1, 2], [3, 4]])
m2 = np.array([[1, 2], [2, 1]])

print(m1 @ m2)
```

#### 符号作用三:使用 __matmul__ 方法自定义@ 操作符
```python
class Dog:
    def __matmul__(self, other):
        return f'hello {other}'
    
dog = Dog()

print(dog @ 500)
```


### 其他

*args :表示接收所有普通参数（直接传入没有表明关键字的参数）
**kwargs：表示接收所有关键字参数（即："a"=5,"b"=8这种标明了的关键字参数）


