# 集合

集合是一个无序的不重复元素序列

## 创建

1. a = {1,...}
2. b = set([4,5,6,7])

**空集合必须要用set()来创建，否则为空字典**

## 添加元素

set.add(x)

set.update(x),x可以为list，tuple，dict等

## 移除元素

set.remove(x) 从set中移除元素，如果不存在会发生错误

set.discard(x) 从set中移除元素，如果没有不会发生错误

set.clear() 清空set

set.pop() 删除并返回元素，无参无序弹出，空集会报错

set.copy()

z = x.difference(y) 返回差集

x.difference_update(y) 如果元素在y中也存在则删除

x.intersection(y) 返回交集

x.symmetric_difference(y) 返回两个集合中不重复元素的集合
symmetric_difference_update()	移除当前集合中在另外一个指定集合相同的元素，并将另外一个指定集合中不同的元素插入到当前集合中。

x.union(y) 返回并集
