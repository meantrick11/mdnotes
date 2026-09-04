# 19. 删除链表的倒数第N个节点

## 题目

给你一个链表，删除链表的倒数第 n 个结点，并且返回链表的头结点。

**由于可能需要删除头节点，那么需要添加一个dummy节点**

## 思路一

首先使用dict记录每个节点对应的位置，然后只需要在最后交换指针就可以了，但是count多计数了1，退出时候那个count本应对应->None，所以代码应该如下：

```python
# Definition for singly-linked list.
# class ListNode:
#     def __init__(self, val=0, next=None):
#         self.val = val
#         self.next = next
class Solution:
    def removeNthFromEnd(self, head: Optional[ListNode], n: int) -> Optional[ListNode]:
        
        hashmap = {}
        dummy = ListNode(0,head)
        cur = dummy
        count = 0
        while cur:
            hashmap[count] = cur
            cur = cur.next
            count += 1
       

        hashmap[count-n-1].next = hashmap[count-n].next

        return dummy.next
```

## 思路二

由于需要定位到删除节点的前一个节点，所以应该让fast先走n+1个指针，如果让fast先走n个指针，那么最后slow指向的是删除节点，而不是删除节点的前置节点

```python
# Definition for singly-linked list.
# class ListNode:
#     def __init__(self, val=0, next=None):
#         self.val = val
#         self.next = next
class Solution:
    def removeNthFromEnd(self, head: Optional[ListNode], n: int) -> Optional[ListNode]:
        
        dummy = ListNode(val=0,next=head)
        slow = fast = dummy
        for i in range(n+1):
            fast = fast.next

        while fast :
            fast = fast.next 
            slow = slow.next

        slow.next = slow.next.next

        return dummy.next

```