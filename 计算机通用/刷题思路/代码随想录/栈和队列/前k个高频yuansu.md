# 前k个高频元素

## 题目



## 思路一

题目的主要解决思路为：
- 统计list中每个元素出现的次数
- 根据出现的次数由高->排序
- 返回前k频率的元素

所以统计可以直接使用collections中的Counter尽心统计，然后直接通过其most_common(k)，返回前k频率的元素以及出现的次数，返回结果类似[(1,3),(2,1)],表示1出现了3次，2出现了1次，最后整理返回元素就可以了。

```python
from collections import Counter
class Solution:
    def topKFrequent(self, nums: List[int], k: int) -> List[int]:
        c = Counter(nums).most_common(k)

        res = []
        for i in range(len(c)):
            res.append(c[i][0])
        

        return res
```

## 思路二(优先队列)

不大明白，怎么还用堆去实现

```python
#时间复杂度：O(nlogk)
#空间复杂度：O(n)
import heapq
class Solution:
    def topKFrequent(self, nums: List[int], k: int) -> List[int]:
        #要统计元素出现频率
        map_ = {} #nums[i]:对应出现的次数
        for i in range(len(nums)):
            map_[nums[i]] = map_.get(nums[i], 0) + 1
        
        #对频率排序
        #定义一个小顶堆，大小为k
        pri_que = [] #小顶堆
        
        #用固定大小为k的小顶堆，扫描所有频率的数值
        for key, freq in map_.items():
            heapq.heappush(pri_que, (freq, key))
            if len(pri_que) > k: #如果堆的大小大于了K，则队列弹出，保证堆的大小一直为k
                heapq.heappop(pri_que)
        
        #找出前K个高频元素，因为小顶堆先弹出的是最小的，所以倒序来输出到数组
        result = [0] * k
        for i in range(k-1, -1, -1):
            result[i] = heapq.heappop(pri_que)[1]
        return result
```



## 思路三(桶排序法)

首先对list中的数字进行统计，然后放入字典中{number->times}，对于每个字典中的number，然后使用桶方法{times->list[number]}，相同出现次数的数，放到同一个list中，然后对于key=times进行排序，然后获取最大的出现的那几个次数的数

```python
class Solution:
    def topKFrequent(self, nums: List[int], k: int) -> List[int]:
        # 使用字典统计数字出现次数
        time_dict = defaultdict(int)
        for num in nums:
            time_dict[num] += 1
        # 更改字典，key为出现次数，value为相应的数字的集合
        index_dict = defaultdict(list)
        for key in time_dict:
            index_dict[time_dict[key]].append(key)
        # 排序
        key = list(index_dict.keys())
        key.sort()
        result = []
        cnt = 0
        # 获取前k项
        while key and cnt != k:
            result += index_dict[key[-1]]
            cnt += len(index_dict[key[-1]])
            key.pop()

        return result[0: k]
    ```
