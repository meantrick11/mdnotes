typedef int ElemType;
typedef struct{
    ElemType *data;
    int length;
}Seqlist;

Seqlist * initList(Seqlist *L,int maxsize){
    //动态初始化顺序表
    Seqlist *L = (Seqlist*)malloc(sizeof(Seqlist));
    L->data = (ElemType*)malloc(sizeof(ElemType)*maxsize);
    L->length = 0;
    return L;
}