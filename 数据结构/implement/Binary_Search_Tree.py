from tree_node import TreeNode

class BinarySearchTree:
    def __init__(self,val=None): 
        '''initialize a binary search tree'''
        self.root = TreeNode(val) if val is not None else None
        self.size = 1 if val is not None else 0
    
    def is_empty(self):
        '''return True if the binary is empty'''
        return self.size == 0
    def search(self,val):
        '''search a value in the binary search tree'''
        current = self.root
        flag = False
        while current is not None:
            if val == current.val:
                flag = current
                break
            elif val < current.val:
                current = current.left
            else:
                current = current.right
        return flag
    
    ###how to find the parent node of a given value in the binary search tree with a loop?
    def LocationOfParent1(self,val):
        '''find the parent node of the given value in the binary search tree for insertion'''
        result = self.search(val)
        if result:
            _parent = None
            current = self.root
            while current is not None:
                _parent = current
                if current == result:
                    #print(f"The value {val} is in the binary search tree,and its parent is:")
                    return _parent
                elif val < current.val:
                    current = current.left
                else:
                    current = current.right
        else:
        
            current = self.root
            parent = None
            while current is not None:
                parent = current
                if val <current.val:
                    current = current.left
                elif val > current.val:
                    current = current.right
            #print(f"The value {val} is not in the binary search tree,and its parent is:")
            return parent
    
    def LocationOfParent2(self,val):
        '''find the parent node of the given value in the binary search tree for deletion'''
        result = self.search(val)
        if result:
            _parent = None
            current = self.root
            while current.val != val:
                _parent = current
                if val < current.val:
                    #print(f"The value {val} is in the binary search tree,and its parent is:")
                    current = current.left
                elif val > current.val:
                    current = current.right
            return _parent
        else:
            print(f"The value {val} is not in the binary search tree,and its parent is:")
            return 
    def insert(self,val):
        '''insert a value into the binary search tree'''
        #a empty tree
        if self.is_empty():
            self.root.val = val
            self.size += 1
            return 
        
        new_node = TreeNode(val)
        _parent = self.LocationOfParent1(val)
        if val < _parent.val:
            _parent.left = new_node
        else:
            _parent.right = new_node
        
        
        self.size += 1
    
    def preorder_traversal(self,root):
        '''preorder traversal of the binary search tree'''
        if root is None:
            return
        print(root.val,end='P')
        self.preorder_traversal(root.left)
        self.preorder_traversal(root.right)
    def inorder_traversal(self,root):
        '''inorder traversal of the binary search tree '''
        if root is None:
            return
        self.inorder_traversal(root.left)
        print(root.val,end='I')
        self.inorder_traversal(root.right)
    def postorder_traversal(self,root):
        '''postorder traversal of the binary search tree'''
        if root is None:
            return
        self.postorder_traversal(root.left)
        self.postorder_traversal(root.right)    
        print(root.val,end='H')  
        

    
    def delete(self,val):
        '''delete a value from the binary search tree'''
        if self.is_empty():
            raise ValueError("The binary search tree is empty")
        if self.search(val) == False:
            raise ValueError("The value is not in the binary search tree")
        else:
            _parent = self.LocationOfParent2(val)
            if val < _parent.val:#the left side of the parent node if val < _parent.val
                current = _parent.left
                if current.left is None and current.right is None:#leaf node
                    _parent.left = None
                elif current.left is not None and current.right is None:#only has left child
                    _parent.left = current.left
                elif current.left is None and current.right is not None:#only has right child
                    _parent.left = current.right
                else: #has both left and right child
                    temp = current.right
                    while temp.left is not None:
                        temp = temp.left
                    temp.left = current.left
                    _parent.left = current.right
                        
                    
            else:#the right side of the parent node if val > _parent.val
                current = _parent.right
                if current.left is None and current.right is None:#leaf node
                    _parent.right = None
                elif current.left is not None and current.right is None:#only has left child
                    _parent.right = current.left 
                elif current.left is None and current.right is not None:#only has right child
                    _parent.right = current.right
                else:#both left and right child
                    temp = current.right
                    while temp.left is not None:
                        temp = temp.left
                    temp.left = current.left
                    _parent.right = current.right
            
            self.size -= 1
                
    def max_num(self,root):
        '''find the maximum value in the binary search tree'''
        current = root
        while current.right is not None:
            current = current.right
        return current.val
    def min_num(self,root):
        '''find the minimum value in the binary search tree'''
        current = root
        while current.left is not None:
            current = current.left
        return current.val
    
    def height_tree(self,root):
        '''find the height of the binary search tree'''
        max_height = 0
        if root is None:
            return 0
        if self.root.left is not None:
            left_height = self.height_tree(root.left)
        else:
            return 0
        if self.root.right is not None:
            right_height = self.height_tree(root.right)
        else:
            return 0
        return max(max_height,left_height+right_height+1)
    
        
            
    
                      
if __name__ == '__main__':
    # 初始化BST并插入初始节点
    bst = BinarySearchTree(5)
    bst.insert(3)
    bst.insert(8)
    
    # 格式化输出初始状态
    print("=" * 60)
    print("初始二叉搜索树（插入5, 3, 8）:")
    print("-" * 30)
    preorder_initial = bst.preorder_traversal(bst.root)
    print(f"<--前序遍历结果")
    height_initial = bst.height_tree(bst.root)
    print(f"树的高度: {height_initial}")
    
    # 插入新节点并输出
    bst.insert(16)
    bst.insert(1)
    print("\n" + "=" * 60)
    print("插入新节点（1, 16）后:")
    print("-" * 30)
    preorder_after_insert = bst.preorder_traversal(bst.root)
    print(f"<--前序遍历结果")
    
    # 删除节点并输出
    bst.delete(8)
    print("\n" + "=" * 60)
    print("删除节点8后:")
    print("-" * 30)
    preorder_after_delete = bst.preorder_traversal(bst.root)
    print(f"<--前序遍历结果")
    height_after_delete = bst.height_tree(bst.root)
    print(f"树的高度: {height_after_delete}")
    print("\n" + "=" * 60)

            
            
    
           