# hashmap

> **相关笔记**: [[../数据结构/hash算法/hash|Hash算法]] | [[双指针|双指针]] | [[kadane算法|Kadane算法]] | [[贪心算法|贪心算法]]

使用hashmap临时存储数值个数等操作，便于下次遍历需要

## 一、只需一种映射关系


## 二、需要两个hashmap共同维护映射关系

必须保证s->t &t->s只有一种映射状况，不能有多对一，一对多的映射，那么就需要维护两个映射表，只能建立一种映射关系，其他条件则不符合。

例题：
leetcode 205.同构字符串

给定两个字符串 s 和 t ，判断它们是否是同构的。

如果 s 中的字符可以按某种映射关系替换得到 t ，那么这两个字符串是同构的。

每个出现的字符都应当映射到另一个字符，同时不改变字符的顺序。不同字符不能映射到同一个字符上，相同字符只能映射到同一个字符上，字符可以映射到自己本身

解决方法：

1. 不使用双hashmap的映射方法，直接强行双匹配：
```python
class Solution:
    def isIsomorphic(self, s: str, t: str) -> bool:
        #s能够映射到t所以t是目标字符串
        ##能够映射，表示字符串的顺序也不能映射。
        length = len(s)

        hashmap = {}#记录s中已经出现了的字符串

        for i in range(length):
            if s[i] not in hashmap.keys() and t[i] not in hashmap.values():
                hashmap[s[i]] = t[i]
            else:
                try:
                    if hashmap[s[i]] != t[i]:
                        return False
                except KeyError:
                    return False
        
        return True


```

2. 使用双hashmap的方法：

```python
class Solution:
    def isIsomorphic(self, s: str, t: str) -> bool:
        if len(s) != len(t):
            return False
        
        # 用两个字典，分别维护 s→t 和 t→s 的映射
        s2t = {}
        t2s = {}
        
        for sc, tc in zip(s, t):
            # 情况1：s的字符已经映射过，但对应的t字符不对
            if sc in s2t:
                if s2t[sc] != tc:
                    return False
            # 情况2：s的字符没映射过，但t的字符已经被别的s字符映射了（多对一）
            else:
                if tc in t2s:
                    return False
                s2t[sc] = tc
                t2s[tc] = sc
        
        return True
        
        return True
```