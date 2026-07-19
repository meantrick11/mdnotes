# string的内置方法

## split()

按照给定字符分割字符串，并返回列表，可选参数:分隔字符(默认空格符)，最大分隔次数
```python
s = "a$b$c$d$e$f"
splited_s = s.split("$",4)#按照,,最大分割四次
print(splited_s)   #输出结果是['a', 'b', 'c', 'd', 'e$f']
```

