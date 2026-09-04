# ansyncio

## 协程

协程（Coroutine）是一种程序组件，它允许执行流程在特定位置挂起（暂停）和恢复，并且支持多次进入和退出，而每次恢复时都能保留上一次挂起时的状态（局部变量、程序计数器等）。

### 创建协程

#### 普通单协程

使用asyncio库的async def & await创建一个协程函数，该函数允许在await处中断执行，一般用于等待io，比如网络请求、数据库连接等操作。

```python
import asyncio

async def main():
    print("hello")
    await asycio.sleep(1)
    print("world~")
if __name__ == "__main__":
    main()

```

这里创造了一个协程，如果你直接运行上述代码是不行的，因为该代码只是创建了一个协程，创建之后，并不能进入函数内部执行，而需要调用异步程序顶层入口工具`asyncio.run(协程)`来运行，它主要做了以下工作：
1. 创建事件循环：协程调度器，所有的协程的暂停、切换、恢复等全靠它管理
2. 将传入的协程丢入循环，一直阻塞等待所有协程完整执行完毕
3. 全部任务跑完之后，自动关闭、销毁事件循环，释放资源

所有的创建的协程，都需要`asyncio.run()`创建事件循环进行调度执行。

所以上述代码应该改为：
```python
import asyncio 

async def main():
    print("hello")
    await asycio.sleep(1)
    print("world~")

if __name__ == "__main__":
    asyncio.run(main())
    
```

#### 创建多个协程

##### 普通的多个协程封装

`asyncio.run(协程)`只能放一个协程对象，只负责启动一个协程入口，如果想要多个协程一起跑并发，那么需要将多个协程任务封装到一个协程入口函数中：

```python
import asyncio
import time

async def say_after(delay, what):
    await asyncio.sleep(delay)
    print(what)

async def main():
    print(f"started at {time.strftime('%X')}")

    await say_after(1, 'hello')
    await say_after(2, 'world')

    print(f"finished at {time.strftime('%X')}")

asyncio.run(main())
```

#### asyncio.create_task()


使用`asyncio.create_task()`会创建协程对象，然后交给事件循环执行。
```python

import asyncio
import time

async def say_after(delay, what):
    await asyncio.sleep(delay)
    print(what)

async def main():
    task1 = asyncio.create_task(
        say_after(1, 'hello'))

    task2 = asyncio.create_task(
        say_after(2, 'world'))

    print(f"started at {time.strftime('%X')}")

    # 等待直到两个任务都完成
    # （会花费约 2 秒钟。）
    await task1
    await task2


    print(f"finished at {time.strftime('%X')}")
    

if __name__ == "__main__":
    asyncio.run(main())
    
```

这里仔细说一下，由于`asyncio.run()`创建了事件循环，并且以`main()`为入口，也就是入口协程，循环事件会查看入口协程是否有需要调度的协程，如果没有，那么它依次执行完`main()`函数中的代码，就关闭了事件循环，所以，虽然`asycio.create_task()`创建了协程，并将协程放进事件循环里，但是如果没有`main()`中的await，那么事件循环不知道main()中需要在某一个地方停留等待，此时，事件循环中虽然有通过`create_task()`创建的协程，但是由于入口协程`main()`依次执行没有await的地方，那么`main()`依次执行完之后，事件循环就会被关闭，此时就算事件循环内部有两个协程，但是由于事件循环关闭，那么其中协程就会强制退出，导致只会有时间打印出来，而内部协程并没有完全执行完就退出了。所以，`main()`中的await语句等待task结束必不可少。

##### asyncio.TaskGroup()

`async with asyncio.TaskGroup()...`提供了`asyncio.creat_task()`更现代的替换，即是对create_task+await等的上层封装，可以类似于文件处理中的`with open...`语句
这里的say_after()与上面的协程函数相同。

```python
async def main():
    async with asyncio.TaskGroup() as tg:
        task1 = tg.create_task(
            say_after(1, 'hello'))

        task2 = tg.create_task(
            say_after(2, 'world'))

        print(f"started at {time.strftime('%X')}")

    # 当上下文管理器退出时 await 是隐式执行的。

    print(f"finished at {time.strftime('%X')}")
```

当然，如果要执行上述协程，那么还是需要导入asycio以及使用`asycio.run()`创造事件循环才能运行。


## 可等待对象

如果一个对象可以在await()语句中使用，那么他就是可等待对象，可等待对象分为三种
- 协程
- 任务
- Futune

### 协程

Python协程属于可等待对象，因此可以在其他协程中被await()
```python
import asyncio

async def nested():
    return 42

async def main():
    # 如果我们只调用 "nested()" 则无事发生。
    # 一个协程对象被创建但是没有被等待，
    # 因此它 *根本不会运行*。
    nested()  # 将引发 "RuntimeWarning"。

    # 现在让我们等待它：
    print(await nested())  # 将打印 "42"。

asyncio.run(main())
```
### 任务

任务是用来并发地调度协程

使用asyncio.create()的协程任务，它可以被等待：
```python
import asyncio

async def nested():
    return 42

async def main():
    # 调度 nested() 与 "main()" 并发运行。
    task = asyncio.create_task(nested())

    # 现在可以通过 "task" 来取消 "nested()"，
    # 也可以等待 "task" 直到它被完成：
    await task

asyncio.run(main())
```

#### 任务详解

##### 任务创建
`asyncio.create_task(coro, *, name=None, context=None, eager_start=None, **kwargs)`
将 coro 协程 封装为一个 Task 并调度其执行。返回 Task 对象。

##### 任务取消

任务可以便捷和安全地取消。当任务被取消时，asyncio.CancelledError 将在遇到机会时在任务中被引发。
推荐协程使用 try/finally 代码块来可靠地执行清理逻辑。对于 asyncio.CancelledError 被显式捕获的情况，它通常应当在清理完成时被传播。

##### 任务组
任务组合并了一套用于等待分组中所有任务完成的方便可靠方式的任务创建 API。

class asyncio.TaskGroup
持有一个任务分组的 异步上下文管理器。可以使用 create_task() 将任务添加到分组中。当该上下文管理器退出时所有任务都将被等待。

create_task(coro, *, name=None, context=None, eager_start=None, **kwargs)
在该任务组中创建一个任务。签名与 asyncio.create_task() 的签名相匹配。 如果该任务组未激活（例如尚未进入、已经结束或在关闭过程中），我们将关闭所给出的 coro。

```python
async def main():
    async with asyncio.TaskGroup() as tg:
        task1 = tg.create_task(some_coro(...))
        task2 = tg.create_task(another_coro(...))
    print(f"Both tasks have completed now: {task1.result()}, {task2.result()}")
```
当首次有任何属于分组的任务因 asyncio.CancelledError 以外的异常而失败时，分组中的剩余任务将被取消。 在此之后将无法添加更多任务到该分组中。在这种情况下，如果 async with 语句体仍然为激活状态（即 __aexit__() 尚未被调用），则直接包含 async with 语句的任务也会被取消。结果 asyncio.CancelledError 将中断一个 await，但它将不会跳出包含的 async with 语句。

一旦所有任务被完成，如果有任何任务因 asyncio.CancelledError 以外的异常而失败，这些异常会被组合在 ExceptionGroup 或 BaseExceptionGroup 中（选择其中较适合的一个；参见其文档）并将随后引发。

两个基础异常会被特别对待：如果有任何任务因 KeyboardInterrupt 或 SystemExit 而失败，任务分组仍然会取消剩余的任务并等待它们，但随后初始 KeyboardInterrupt 或 SystemExit 而不是 ExceptionGroup 或 BaseExceptionGroup 会被重新引发。

如果 async with 语句体因异常而退出（这样将调用 __aexit__() 并附带一个异常），此种情况会与有任务失败时一样对待：剩余任务将被取消然后被等待，而非取消类异常会被加入到一个异常分组并被引发。传入到 __aexit__() 的异常，除了 asyncio.CancelledError 以外，也都会被包括在该异常分组中。同样的特殊对待也适用于上一段所说的 KeyboardInterrupt 和 SystemExit.

对于任务组应当注意不要将用于“唤醒”其 __aexit__() 的内部取消请求与其他地方对其运行的任务提出的取消请求相混淆。 具体来说，当一个任务组在语法上嵌套于另一个任务组中，而两个任务组的某个子任务同时发生异常时，内层的任务组将处理其异常，然后外层的任务组将收到另一个取消请求并处理它自己的异常。

对于任务组在外部被取消同时必须引发 ExceptionGroup 的情况，它将调用父任务的 cancel() 方法。这样可以确保 asyncio.CancelledError 会在下一次 await 时被引发，因此取消操作不会丢失。

任务组将保留 asyncio.Task.cancelling() 所报告的取消次数。

### Futures

Future 是一种特殊的 低层级 可等待对象，表示一个异步操作的 最终结果。

当一个 Future 对象 被等待，这意味着协程将保持等待直到该 Future 对象在其他地方操作完毕。

在 asyncio 中需要 Future 对象以便允许通过 async/await 使用基于回调的代码。

通常情况下 没有必要 在应用层级的代码中创建 Future 对象。

Future 对象有时会由库和某些 asyncio API 暴露给用户，用作可等待对象:
```python
async def main():
    await function_that_returns_a_future_object()

    # 这样也可以：
    await asyncio.gather(
        function_that_returns_a_future_object(),
        some_python_coroutine()
    )
```
一个很好的返回 Future 对象的低层级函数的示例是 loop.run_in_executor()。

## 休眠

async asyncio.sleep(delay, result=None)
阻塞 delay 指定的秒数。
如果指定了 result，则当协程完成时将其返回给调用者。

sleep() 总是会挂起当前任务，以允许其他任务运行。

将 delay 设为 0 将提供一个经优化的路径以允许其他任务运行。这可供长期间运行的函数使用以避免在函数调用的全过程中阻塞事件循环。

## 并发地运行任务

如果想要协程/任务并发地执行，一个是使用create_task()创建任务，然后分别await()，但是细微差别为会一旦一个任务抛出错误，那么整个协程都会中断，如果使用gather()并发执行协程，那么他会等待所有结果返回后统一打包返回结果，即使有错误也不会中断。

awaitable asyncio.gather(*aws, return_exceptions=False)
并发 运行 aws 序列中的 可等待对象。

如果 aws 中的某个可等待对象为协程，它将自动被作为一个任务调度。

如果所有可等待对象都成功完成，结果将是一个由所有返回值聚合而成的列表。结果值的顺序与 aws 中可等待对象的顺序一致。

如果 return_exceptions 为 False (默认)，所引发的首个异常会立即传播给等待 gather() 的任务。aws 序列中的其他可等待对象 不会被取消 并将继续运行。

如果 return_exceptions 为 True，异常会和成功的结果一样处理，并聚合至结果列表。

如果 gather() 被取消，所有被提交 (尚未完成) 的可等待对象也会 被取消。

如果 aws 序列中的任一 Task 或 Future 对象 被取消，它将被当作引发了 CancelledError 一样处理 -- 在此情况下 gather() 调用 不会 被取消。这是为了防止一个已提交的 Task/Future 被取消导致其他 Tasks/Future 也被取消。
*备注 一个创建、并发地运行任务并等待它们完成的新选择是 asyncio.TaskGroup。 TaskGroup 针对嵌套子任务的调度提供比 gather 更强的安全保证：如果一个任务（或子任务，即由某个任务调度的任务）引发了异常，TaskGroup 将取消剩余的已排期任务，而 gather 不会这样做。*

```python
import asyncio

async def factorial(name, number):
    f = 1
    for i in range(2, number + 1):
        print(f"Task {name}: Compute factorial({number}), currently i={i}...")
        await asyncio.sleep(1)
        f *= i
    print(f"Task {name}: factorial({number}) = {f}")
    return f

async def main():
    # *并发地* 调度这三次调用：
    L = await asyncio.gather(
        factorial("A", 2),
        factorial("B", 3),
        factorial("C", 4),
    )
    print(L)

上述代码可以替换为分别asyncio.create_task()创建任务，然后分别await，但是上层main协程会分别在await处阻塞，与gather()有差别，且还有异常处理的不同。

asyncio.run(main())

# 预期的输出：
#
#     Task A: Compute factorial(2), currently i=2...
#     Task B: Compute factorial(3), currently i=2...
#     Task C: Compute factorial(4), currently i=2...
#     Task A: factorial(2) = 2
#     Task B: Compute factorial(3), currently i=3...
#     Task C: Compute factorial(4), currently i=3...
#     Task B: factorial(3) = 6
#     Task C: Compute factorial(4), currently i=4...
#     Task C: factorial(4) = 24
#     [2, 6, 24]
```

## 主动任务工厂
asyncio.eager_task_factory(loop, coro, *, name=None, context=None)
用于主动任务执行的任务工厂

当使用这个工厂函数时 (通过 loop.set_task_factory(asyncio.eager_task_factory))，协程将在 Task 被构造时同步开始执行。仅当任务面临阻塞时，它们才会被调度到事件循环上。这将避免可同步完成的协程被调度到事件循环所带来的开销，从而可以达成性能提升。

此特性会带来好处的一个常见例子是应用了缓存或记忆功能以便在可能的情况避免实际 I/O 的协程。

## 屏蔽取消操作

awaitable asyncio.shield(aw)
保护一个 可等待对象 防止其被 取消。

如果 aw 是一个协程，它将自动被作为任务调度。

以下语句:

task = asyncio.create_task(something())
res = await shield(task)
相当于:

res = await something()
不同之处 在于如果包含它的协程被取消，在 something() 中运行的任务不会被取消。从 something() 的角度看来，取消操作并没有发生。然而其调用者已被取消，因此 "await" 表达式仍然会引发 CancelledError。

如果通过其他方式取消 something() (例如在其内部操作) 则 shield() 也会取消。

如果希望完全忽略取消操作 (不推荐) 则 shield() 函数需要配合一个 try/except 代码段，如下所示:
```python
task = asyncio.create_task(something())
try:
    res = await shield(task)
except CancelledError:
    res = None
```
**重要 保存一个传给此函数的任务的引用，以避免任务在执行过程中消失。事件循环将只保留对任务的弱引用。 未在其他地方被引用的任务可能在任何时候被作为垃圾回收，即使是在它被完成之前。**

## 超时

asyncio.timeout(delay)
返回一个可被用于限制等待某个操作所耗费时间的 异步上下文管理器。

delay 可以为 None，或是一个表示等待秒数的浮点数/整数。如果 delay 为 None，将不会应用时间限制；如果当创建上下文管理器时无法确定延时则此设置将很适用。

在两种情况下，该上下文管理器都可以在创建之后使用 Timeout.reschedule() 来重新安排计划。

示例:

async def main():
    async with asyncio.timeout(10):
        await long_running_task()
如果 long_running_task 耗费 10 秒以上完成，该上下文管理器将取消当前任务并在内部处理所引发的 asyncio.CancelledError，将其转化为可被捕获和处理的 TimeoutError。

## 等待原语
async asyncio.wait(aws, *, timeout=None, return_when=ALL_COMPLETED)
并发地运行 aws 可迭代对象中的 Future 和 Task 实例并进入阻塞状态直到满足 return_when 所指定的条件。

aws 可迭代对象必须不为空。

返回两个 Task/Future 集合: (done, pending)。

用法:

done, pending = await asyncio.wait(aws)
如指定 timeout (float 或 int 类型) 则它将被用于控制返回之前等待的最长秒数。

请注意此函数不会引发 TimeoutError。当超时发生时尚未完成的 Future 或 Task 会在设定的秒数后被直接返回。

## 在线程中运行

async asyncio.to_thread(func, /, *args, **kwargs)

## 跨线程调度

asyncio.run_coroutine_threadsafe(coro, loop)
向指定事件循环提交一个协程。（线程安全）

## 内省

asyncio.current_task(loop=None)
返回当前运行的 Task 实例，如果没有正在运行的任务则返回 None。

如果 loop 为 None 则会使用 get_running_loop() 获取当前事件循环。

Added in version 3.7.

asyncio.all_tasks(loop=None)
返回事件循环所运行的未完成的 Task 对象的集合。

如果 loop 为 None，则会使用 get_running_loop() 获取当前事件循环。

Added in version 3.7.

asyncio.iscoroutine(obj)
如果 obj 是一个协程对象则返回 True。

## Task 对象
class asyncio.Task(coro, *, loop=None, name=None, context=None, eager_start=False)
一个运行 Python 协程 的 Future-like 对象。 非线程安全。

Task 对象被用来在事件循环中运行协程。如果一个协程在等待一个 Future 对象，Task 对象会挂起该协程的执行并等待该 Future 对象完成。当该 Future 对象 完成，被打包的协程将恢复执行。

事件循环使用协同日程调度：一个事件循环每次运行一个 Task 对象。而一个 Task 对象会等待一个 Future 对象完成，该事件循环会运行其他 Task、回调或执行 IO 操作。

使用高层级的 asyncio.create_task() 函数来创建 Task 对象，也可用低层级的 loop.create_task() 或 ensure_future() 函数。不建议手动实例化 Task 对象。

To cancel a running Task use the cancel() method. Calling it will cause the Task to throw a CancelledError exception into the wrapped coroutine. If a coroutine is awaiting on a Future object during cancellation, the Future object will be cancelled.

cancelled() 可被用来检测 Task 对象是否被取消。如果打包的协程没有抑制 CancelledError 异常并且确实被取消，该方法将返回 True。

asyncio.Task 从 Future 继承了其除 Future.set_result() 和 Future.set_exception() 以外的所有 API。

可选的仅限关键字参数 context 允许指定自定义的 contextvars.Context 供 coro 运行。 如果未提供 context，Task 将拷贝当前上下文并随后在拷贝的上下文中运行其协程。

可选的仅限关键字参数 eager_start 允许在任务创建时主动开始 asyncio.Task 的执行。如果设为 True 并且事件循环正在运行，任务将立即开始执行协程，直到该协程第一次阻塞。 如果协程未发生阻塞即返回或引发异常，任务将主动结束并将跳过向事件循环添加计划任务。
