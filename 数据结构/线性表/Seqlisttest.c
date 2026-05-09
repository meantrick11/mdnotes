#include <stdio.h>
#include "Seqlist.c"

//test2.c文件
int appendElemem(Seqlist *L,int e){
    //向顺序表依次插入元素
    if (L->length>=MAXSIZE){
        printf("顺序表已满，无法插入元素\n");
        return 0;
    }
    else{
        L->data[L->length]=e;
        L->length++;
        return 1;
    }
}

//test3.c文件
void printList(Seqlist *L){
    //打印顺序表中的元素
    for (int i=0;i<=L->length;i++){
        printf("%d",L->data[i]);
    }
    printf("\n");
}

//test4.c文件
int insertElem(Seqlist *L,int pos,ElemType e){
    //在顺序表中指定位置插入元素
    if (pos<0||pos>L->length){
        printf("插入位置不合法\n");
        return 0;
    }
    else{
        for (int i = (L->length-1);i>=pos-1;i++){
            L->data[i+1] = L->data[i];
        }
        L->data[pos-1] = e;
        L->length++;
        
        return 1;
    }
}

//test5.c文件
int deleteElem(Seqlist *L,int pos,ElemType *e){
    //在顺序表中删除指定位置的元素
    *e = L->data[pos-1];
    if (pos<0||pos>L->length){
        printf("删除位置不合法\n");
        return 0;
    }
    else{
        for (int i=pos;i<L->length;i++){
            L->data[i-1] = L->data[i];
        }
        L->length--;
        printf("删除的元素是%d\n",*e);
        return 1;
    }
}

int findElem(Seqlist *L,ElemType e){
    //循环遍历查找e在顺序表中首次出现的位置
    for(int i=0;i<L->length;i++){
        if (L->data[i]==e){
            printf("元素%d在顺序表中的位置是%d\n",e,i+1);            
            return 1;
    }
    return 0;
}