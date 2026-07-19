# 字典

字典的键必须唯一，但值不必

## 创建

a = {}
b = dict()

## 赋值

## 修改

## 删除

del di["name"]

## 字典的特性

字典值可以是任何对象，但键不行
1. 不允许同一个键出现两次，创建时如果同一个键被赋值两次，则后一个值会被记住

2. 键必须为不可变类型，不可变类型有数字、string、tuple

## 内置方法

1. len(dict) 返回字典长度
2. str(dict) 转为string，可以答应字符串表示
3. type(dict) 返回为dict类型
4. dict.clear() 清空字典
5. dict.copy()
6. dict.fromkeys() 以序列seq中的元素作为键，值为原本字典的值，相当于按要求重排
7. dict.get(key,default=None) 获取key对应的值，否则返回默认值
8. key in dict 如果键在dict返回True，否则False
9. dict.items() 返回一个视图
10. dict.keys() 返回keys的列表
11. dict.setdefault(key,defalut=None) 获取键对应的值，否则添加键值对
12. dict.values() 获取值的视图
13. dict.pop(key,default=None) 删除对应的键值对并返回值，否则返回default
14. dict.popitem() 删除最后一对键值对
