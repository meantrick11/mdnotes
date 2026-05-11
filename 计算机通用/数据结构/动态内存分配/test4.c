#include <stdio.h>
#include <stdlib.h>

typedef struct{
    int x;
    int y;
}point;

int main(int argc,char *argv[]){
    point *p;
    p = (point *)malloc(sizeof(point));
    p->x = 10;
    p->y = 20;
    printf("x=%d,y=%d\n",p->x,p->y);
    free(p);
    return 0;
}
