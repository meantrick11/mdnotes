#include <stdio.h>


typedef char ElemType;

//定义二叉树结点，包含数据以及左右子树的指针
typedef struct {
    ElemType data;
    TreeNode * lchild;
    TreeNode * rchild;
}TreeNode;

//定义二叉树指针
typedef TreeNode * BiTree;

//前序遍历

void preOrder(BiTree T){
    //递归从上到下，从左向右
    //遍历的时候先输出数据data(前序)
    if (T==NULL){
        return;        
    }
    printf("%c",T->data);
    preOrder(T->lchild);
    preOrder(T->rchild);
}

void inOrder(BiTree T){
    //遍历的时候先递归左边，中间输出数据data(中序)
    //中序遍历，先访问根结点，向树的左下角移动，直到遇到空结点为止
    //然后访问空结点的父节点，继续遍历该结点的右子树，如果右子树没有子树可以遍历，
    //那么继续遍历上一层最后一个未被访问的结点
    if (T==NULL){
        return;
    }
    inOrder(T->lchild);
    printf("%c",T->data);
    inOrder(T->rchild);
}

//后续遍历
void postOrder(BiTree T){
    //先递归左右，最后的时候再输出数据data
    if (T==NULL){
        return;
    }
    postOrder(T->lchild);
    postOrder(T->rchild);
    printf("%c",T->data);
}