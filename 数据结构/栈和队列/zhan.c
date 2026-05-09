#include <stdio.h>

#define MAXSIZE 100
typedef int ElemType;

//test1.c文件
typedef struct {
    ElemType data[MAXSIZE];
    int top;
}Stack;

//test2.c文件
typedef struct{
    ElemType *data;
    int top;
}Stack;
Stack * initStack(){
    Stack *s = (Stack*)malloc(sizeof(Stack));
    s->data = (ElemType*)malloc(sizeof(ElemType)*MAXSIZE);
    s->top =  -1;
    return s;
}
//test3.c文件
typedef struct {
    //栈顶就是头节点的下一个节点，栈底是尾节点
    //判断是否为空，头节点是否为NULL
    //进栈和出栈就是在头节点后面插入和删除节点
    
    ElemType data;
    struct stack *next;
}Stack1;

//初始化栈
void initStack(Stack *s){
    s->top = -1;
}

//判断栈是否为空
int isEmpty(Stack *s){
    if (s->top == -1){
        return 1;
    }else{
        return 0;
    }
}

//进栈和压栈
int push(Stack *s,ElemType e){
    if (s->top == MAXSIZE -1){
        printf("栈满，无法进栈\n");
        return 0;
    }
    else{
        s->top++;
        s->data[s->top] = e;
        return 1;
    }
}

//出栈和弹栈
int pop(Stack *s,ElemType *e){
    if (isEmpty(s)){
        printf("栈空，无法出栈\n");
        return 0;
    }
    else{
        *e = s->data[s->top];
        s->top--;
        return 1;
    }
}

//获取栈顶元素
int getTop(Stack *s,ElemType *e){
    if (isEmpty(s)){
        printf("栈空，无法获取栈顶元素\n");
        return 0;
    }
    else{
        *e = s->data[s->top];
        return 1;
    }
}