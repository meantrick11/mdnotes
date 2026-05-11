class Li_Stack:
    def __init__(self):
        '''create an empty queue'''
        self.items = []
        
    def is_empty(self):
        '''return True if the stack is empty, False otherwise'''
        return self.items == []
    
    def push(self,value):
        '''add a value to the end of the stack'''
        self.items.append(value)
        
    def pop(self):
        '''remove and return the value at the top of the stack'''
        if self.is_empty():
            return IndexError('Stack is empty')
        tem = self.items[-1]
        self.items.pop()
        return tem
    
    def peek(self):
        '''return the value at the end of the stacke'''
        if self.is_empty():
            return IndexError('Stack is empty')    
        return self.items[0]
    
    def size(self):
        '''return the number of items in the stack'''
        return len(self.items)
