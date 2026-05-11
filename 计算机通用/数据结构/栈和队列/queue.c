#include <stdio.h>
#include <stdlib.h>

#define MAXSIZE 100
typedef int ElemType;
//队列的顺序存储结构
//test1.c文件
typedef struct {
    ElemType data[MAXSIZE];//ElemType *data;
    int front;
    int rear;
}Queue;

//初始化队列
void initQueue(Queue *q){
    //Queue *q = (Queue*)malloc(sizeof(Queue));
    //q->data = (ElemType*)malloc(sizeof(ElemType)*MAXSIZE);
    q->front = 0;
    q->rear = 0;
}
//判断是否为空
//进队时，rear+1，出队时，front+1

int isEmpty (Queue *q){
    if (q->front ==q->rear){
        return 1;
    }else{
        return 0;
    }
}

//进队
int enQueue(Queue *q,ElemType e){
    if (q->rear >=MAXSIZE){
        printf("空队列已满，无法进队\n");
        return 0;
    }
    q->data[q->rear] = e;
    q->rear++;
    return 1;
}

//出队
int deQueue(Queue *q,ElemType *e){
    if (isEmpty(q)){
        printf("空队列，无法出队\n");
        return 0;
    }
    *e = q->data[q->front];
    q->front--;
    return 1;
}

//获取队头，直接访问front位置的元素


//test2.c文件
int equeue(Queue *Q,ElemType e){
    if ((Q->rear +1)%MAXSIZE == Q->front){
        printf("循环队列已满，无法进队\n");
        return 0;
    }
    Q->data[Q->rear]=e;//队尾增加元素
    Q->rear = (Q->rear +1)%MAXSIZE;
    
}

//test4.c文件
//对列的链式存储结构/数据+指针（链表节点
typedef struct{
    ElemType data;
    struct QNode *next;
}QNode;
//队列的结构。对头和队尾指针管理队列
typedef struct{
    QNode *front;
    QNode *rear;
}Queue1;

//链式结构的队列初始化
Queue1 * initQueue1(){
    //生成初始对头队尾
    Queue1 *q = (Queue1*)malloc(sizeof(Queue1));
    //创建头节点，data为0，next为空
    QNode *node = (QNode*)malloc(sizeof(QNode));
    node ->data =0;
    node ->next = NULL;
    //让对头队尾同时指向头节点
    q->front = node;
    q->rear = node;
    
    return q;
}

//入队（尾插入
void enQueue1(Queue1 *q,ElemType e){
    //创建新节点
    QNode *node = (QNode*)malloc(sizeof(QNode));
    node ->data = e;
    node ->next = NULL;
    //队尾插入新节点
    q->rear->next = node;
    //队尾指针后移到新节点
    q->rear = node;
}

//出队（头删除
int deQueue1(Queue1 *q,ElemType *e){
    //创建临时节点，并讲数据存储到e中
    QNode *node = q->front->next;
    *e = node->data;
    //对头指针指向后继节点
    q->front->next = node->next;
    //判断如果是尾节点，让rear指向front同一节点
    if (q->rear == node){
        q->rear = q->front;
    }
    //释放中间的节点（临时节点
    free(node);
    return 1;
}


//获取对头元素
ElemType getFront(Queue1 *q){
    //判断是否为空
    if (isEmpty1(q)){
        printf("队列为空，无法获取队头元素\n");
        return -1;
    }
    return q->front->next->data;

}
