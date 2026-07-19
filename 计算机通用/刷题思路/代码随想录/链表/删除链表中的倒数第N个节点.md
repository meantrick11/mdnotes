# 19. 删除链表中的倒数第N个节点

>给你一个链表，删除链表的倒数第 n 个结点，并且返回链表的头结点。

思路一（hashmap记录法）：由于可能删除头节点，所以需要添加dummy节点。使用hashmap的快速查找特性，循环遍历一次记录对应的num->node的映射，然后快速定位到所要删除的节点的前一个节点和后一个节点，然后更新指针指向就可以了

```python
class ListNode:
    def __init__(self,val=0,next=None):
        self.val = val
        self.next = next

class Solution:
    def removeNthFromEnd(self, head: Optional[ListNode], n: int) -> Optional[ListNode]:
        if not head:
            return
        #需要添加dummy节点，确保length-n有取值
        dummy = ListNode(None)
        dummy.next = head

        hashmap = {}
        current = dummy
        num = 0
        while current:
            hashmap[num] = current
            num += 1
            current = current.next
        hashmap[num-n-1].next = hashmap[num-n].next

        return dummy.next

```

思路二(快慢指针)：同思路一，需要引入dummy节点，然后让fast指针从dummy开始先走n+1步(因为要删除倒数第n个节点，如果走n步，循环结束时，slow刚好指向目标节点，所以需要走n+1步，刚好slow停留在目标节点点的前置节点)，然后slow&fast同时走，当fast走到链表末尾None时，slow刚好走到目标节点的前一个节点，随后更新指针指向即可

```python
# Definition for singly-linked list.
# class ListNode:
#     def __init__(self, val=0, next=None):
#         self.val = val
#         self.next = next
class Solution:
    def removeNthFromEnd(self, head: Optional[ListNode], n: int) -> Optional[ListNode]:
        
        if not head:
            return 

        dummy = ListNode(0,head)
        slow = fast = dummy
        for i in range(n+1):  
            fast = fast.next 

        while fast :
            fast = fast.next 
            slow = slow.next

        slow.next = slow.next.next 

        return dummy.next         
```



        
        