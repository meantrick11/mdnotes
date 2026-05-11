from linked_list import ListNode

class ListStack:
    '''a stack implemented by a singly linked list'''
    def __init__(self):
        '''create the head node of the linked stack'''
        self.head = ListNode()#create a dummy node as_completed(fs, timeout=None)
        self._size = 0 # the number of items in the stack
      
    def is_empty(self):
        '''return True if the stack is empty, False otherwise'''
        return self._size == 0
    
    def push(self,value):
        """add a new node to the top of the stack"""
        new_node = ListNode(value)
        tem = self.head.next
        self.head.next = new_node
        new_node.next = tem
        self._size += 1
        
    def pop(self):
        '''remove and return the value which at the top of the stack'''
        if self.is_empty():
           #raise() will interrupt the program execution, 
           # while return() only returns an error without aborting the program.
            raise IndexError('Stack is empty')
        tem = self.head.next #head insertion
        self.head.next = tem.next
        self._size -= 1
        return tem.val
    
    def peek(self):
        '''return the value at the top of the stack '''
        if self.is_empty():
            raise IndexError('Stack is empty')
        return self.head.next.val
    
    def size(self):
        '''return the number of items in the stack'''
        return self._size
    def __len__(self):
        '''补充：支持内置 len() 函数（Python 最佳实践）'''
        return self._size
    
    
# 测试用例（补充：验证功能正确性）
if __name__ == "__main__":
    stack = ListStack()
    print("初始栈是否为空：", stack.is_empty())  # True
    
    stack.push(1)
    stack.push(2)
    stack.push(3)
    print("入栈后栈的状态：", stack)  # ListStack([3, 2, 1])
    print("栈的大小：", stack.size())  # 3
    print("栈顶元素：", stack.peek())  # 3
    
    print("出栈元素：", stack.pop())  # 3
    print("出栈后栈的状态：", stack)  # ListStack([2, 1])
    print("栈的大小（len）：", len(stack))  # 2
    
    # 测试空栈操作（验证异常）
    try:
        empty_stack = ListStack()
        empty_stack.pop()
    except IndexError as e:
        print("空栈pop异常：", e)  # Stack is empty