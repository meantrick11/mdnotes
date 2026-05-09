# import time 
# import asyncio


# async def main():
#     print("hello")
#     print(time.time())
#     await asyncio.sleep(1)
    
#     print("world")
#     print(time.time())
    
# asyncio.run(main())

# Define a decorator function


# import asyncio

# async def call_gpt(prompt:str)-> str:
#     await asyncio.sleep(1)
#     return f'GPT:{prompt}'

# async def call_claude(prompt:str) -> str:
#     await asyncio.sleep(1)
#     return f'Claude:{prompt}'

# async def main():
#     prompt = "give me an example of test"
    
#     task1 = asyncio.create_task(call_gpt(prompt),name='task1')
#     task2 = asyncio.create_task(call_claude(prompt),name='task2')
#     task3 =  await call_gpt(prompt)
#     result = await asyncio.gather(task1,task2)
#     result.append(task3)
#     return result

# if __name__ == "__main__":
#     result = asyncio.run(main())
#     print(result)


import asyncio

async def call(prompt:str)->str:
    print(f"call {prompt}")
    await asyncio.sleep(1)
    return f"result of {prompt}"

async def main():
    print("Start ")
    task1 = asyncio.create_task(call("task1"))
    
    task2 = asyncio.create_task(call("task2"))
    
    result = await asyncio.gather(task1,task2)
    
    print(result)

    print("end")
    
asyncio.run(main())    