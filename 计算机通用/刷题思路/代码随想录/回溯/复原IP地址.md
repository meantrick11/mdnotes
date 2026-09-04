# 93. 复原IP地址

## 题目

有效 IP 地址 正好由四个整数（每个整数位于 0 到 255 之间组成，且不能含有前导 0），整数之间用 '.' 分隔。

例如："0.1.2.201" 和 "192.168.1.1" 是 有效 IP 地址，但是 "0.011.255.245"、"192.168.1.312" 和 "192.168@1.1" 是 无效 IP 地址。
给定一个只包含数字的字符串 s ，用以表示一个 IP 地址，返回所有可能的有效 IP 地址，这些地址可以通过在 s 中插入 '.' 来形成。你 不能 重新排序或删除 s 中的任何数字。你可以按 任何 顺序返回答案。

 

示例 1：

输入：s = "25525511135"
输出：["255.255.11.135","255.255.111.35"]
示例 2：

输入：s = "0000"
输出：["0.0.0.0"]
示例 3：

输入：s = "101023"
输出：["1.0.10.23","1.0.102.3","10.1.0.23","10.10.2.3","101.0.2.3"]

## 思路（递归回溯法）
**所有的回溯优化，都是在中间进行判断，然后直接continue跳过一些遍历的值，进行减少递归或者循环的次数的优化**
首先声明一个result存储结果数组，一个curpath的当前路径递归遍历数组，然后使用for递归法进行判断，不过需要在每次递归之前进行符合ip的判断

1. 递归输入：startindex：当前起始的索引(后续递归的开始值)以及当前递归分割次数(剪枝可用,快速缩减范围) 递归输出：由于最后将符合ip分割的结果添加到result中，所以无需返回值

2. 递归终止条件：当分割到末尾结束，那么表示将整个字符串都遍历分割了，那么进行结果判断添加与否，然后return

3. 每次递归操作：按照当前可能的分割，进行ip判断(主要是0<=ip<=255,且当ip段>1时,不能以0为起始)，如果满足条件，那么进行下一层的递归。

```python
class Solution:
    def restoreIpAddresses(self, s: str) -> List[str]:
        result = [] #结果记录数组

        curpath = []    #当前路径数组
        n = len(s)  #字符串长度

        def findip(startindex:int,times:int):
            #从上层命名空间中获取上层变量定义
            nonlocal result
            nonlocal curpath
            nonlocal n
            nonlocal s

            if startindex == n: #如果当前索引来到了n，也就刚好分割完。
                if len(curpath) == 4:   #符合ip条件的四段大小
                    result.append(".".join(curpath)) #将结果添加到result中
            for i in range(startindex,n):#进行当前层的遍历
                cursubip = s[startindex:i+1]   #切割出当前要判断的ip子段
                if times<5 and int(cursubip) >= 0 and int(cursubip)<=255: #符合ip单个字段的大小,且k也就是分的段数<= 4
                    if len(cursubip) > 1 and cursubip.startswith("0"):   #排除前导0的情况
                        continue
                    curpath.append(str(cursubip))    #将结果添加到curpath中，记录
                    findip(i+1,times+1) #进行下一层的递归:后续startindex，分割次数times+=1
                    curpath.pop()   #当前层的当前元素的回溯
                else:
                    continue
            
        findip(0,1) #第一次递归调用，且从0开始分割
        return result
```

