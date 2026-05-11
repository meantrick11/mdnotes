from linked_list import ListNode

class ListQueue:
    '''a queue implemented by a singly linked list'''
    
    def __init__(self):
        '''create the head node of the linked queue'''
        self.head = self.tail = ListNode()#create a dummy node
        self._size = 0 # the number of items in the queue
        
    def is_empty(self):
        ''''return True if the queue is empty,False otherwise'''
        if self._size == 0:
            return True
        else:
            return False
        
    def enqueue(self,value):
        '''add a new item to the tail of the queue'''
        new_node = ListNode(value)
        self.tail.next = new_node
        self.tail = new_node
        self._size += 1
        
    def dequeue(self):
        '''remove and return the value which at the head of the queue'''        
        if self.is_empty():
            raise IndexError('Queue is empty')
        if self.head.next.next is None:
            tem = self.tail
            self.tail = self.head
            self.head.next = None
        else:
            tem = self.head.next
            self.head.next = tem.next
        self._size -= 1
        return tem.val
        
    
    def size(self):
        '''return the number of items in the queue'''
        return self._size
    
    def size(self):
        '''return the number of items in the queue'''
        return self._size
    def peek(self):
        '''return the value at the head of the queue without removing it '''
        if self.is_empty():
            raise IndexError('Queue is empty')
        return self.head.next.val
    
    def check_queue(self):
        '''print the values of all the items in the queue from head to tail'''
        if self.is_empty():
            raise IndexError('Queue is empty')
        tem = self.head.next
        while tem.next is not None:
            print(tem.val,end='|')
            tem = tem.next
if __name__ == "__main__":
    queue = ListQueue()
    queue.enqueue(5)
    queue.enqueue(14)
    queue.enqueue(29)
    queue.enqueue(58)
    print(queue.size())
    print(queue.peek())
    queue.check_queue()
    queue.dequeue()
    queue.check_queue()
    queue.dequeue()
    print(queue.peek())
    queue.dequeue()
    print(queue.is_empty())
    print(queue.size())
    queue.dequeue()
    
    