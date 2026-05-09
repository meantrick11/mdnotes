#define MAXSIZE 20
 typedef int ElemType;
 
 //定义顺序表
 typedef struct{
    ElemType data[MAXSIZE];
    int length;
 }Seqlist;

//初始化顺序表
void initList(Seqlist *L){
    L->length = 0;
}
