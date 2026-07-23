# ansyncio

## 协程

协程（Coroutine）是一种程序组件，它允许执行流程在特定位置挂起（暂停）和恢复，并且支持多次进入和退出，而每次恢复时都能保留上一次挂起时的状态（局部变量、程序计数器等）。

## 创建协程

### 普通单协程

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

### 创建多个协程

#### 普通的多个协程封装

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

#### asyncio.TaskGroup()

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



