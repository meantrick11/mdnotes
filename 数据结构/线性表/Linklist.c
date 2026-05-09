#include <stdio.h>
#include <stdlib.h>

typedef int ElemType;

typedef struct Node{
    ElemType data;
    struct Node *next;
}Node;

//test1.c文件
Node * initList(){
    //初始化链表
    Node *head = (Node*)malloc(sizeof(Node));
    head->data = 0;
    head->next = NULL;
    return head;
}

int main(){
    Node *head = initList();
    return 1;
}

//test2.c文件
int headinsert(Node *head,ElemType e){
    //头插法插入元素
    Node *p = (Node*)malloc(sizeof(Node));
    p->data = e;
    p->next = head->next;//必须是先让新的节点指向原来的节点
    head->next = p;
    return 1;
}

//test3.c文件
void printList(Node *head){
    //打印链表中的元素
    Node *p = head->next;//跳过头节点，直接指向第一个有数据的节点
    while (p!=NULL){
        printf("%d",p->data);
        p = p->next;
    }
    printf("\n");
}

Node * find_tail(Node *L){
    //找到链表的尾节点
    Node *p=L;
    while (p->next != NULL){
        p = p->next;
    }
    return p;
}

Node *tailinsert(Node *tail,ElemType e){
    //在尾部插入元素
    Node *p = (Node*)malloc(sizeof(Node));
    p->data = e;
    tail->next = p;
    p->next = NULL;
    return 1;
}

int intsertNode(Node *L,int pos,ElemType e){
    //在链表指定位置插入元素
    //检查长度是否合法,并找到插入位置的前驱节点
    Node *p = L;//指向头节点，不是有数据的第一节点

    int i=0;
    while(i<pos-1){
        p = p->next;//第一个循环之后，p指向第一个有数据的节点
        i++;
        if (p==NULL){
            printf("插入位置不合法\n");
            return 0;
        }
        Node *q = (Node*)malloc(sizeof(Node));
        q->data = e;
        q->next = p->next;
        p->next = q;
        return 1;
    }

}

int deleteNode(Node *L,int pos){
    //在链表中删除指定位置的元素
    Node *p = L;
    int i =0;
    while(i<pos-2){
        p = p->next;
        i++;
        if (p==NULL){
            printf("删除位置不合法\n");
            return 0;
    }

    //用q备份要删除的节点
    Node *q = p->next;
    //让被删除节点的前驱节点指向被删除节点的后继节点
    p->next = q->next;
    free(q);
    return 1;
}

//test7.c文件
int listLength(Node *L){
    //遍历链表
    Node *p = L->next;//跳过头节点
    ElemType count = 0;
    while (p!=NULL){
        p = p->next;
        count++;
    }
    return count;   
}

//test9.c文件
int findMiddle(Node *L,int k){
    //找到链表的倒数第k个元素
    Node *slow = L;
    Node *fast = L;
    for (int i=0;i<k;i++){
        fast = fast->next;
    }
    while (fast->next != NULL){
        slow = slow->next;
        fast = fast->next;
    }
    printf("倒数第%d个元素是：%d\n",k,slow->data);
    return 1;
}