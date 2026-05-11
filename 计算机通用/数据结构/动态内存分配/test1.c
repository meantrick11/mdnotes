#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]){
    int *p;
    p = (int *)malloc(sizeof(int));
    *p = 10;
    printf("%d\n",*p);
    return 0;
}