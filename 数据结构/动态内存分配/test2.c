#include <stdio.h>
#include <stdlib.h>//malloc函数所在
#include <string.h>

int main(int argc,char const *argv[]){
    char *s;
    s = (char *)malloc(sizeof(char)*10);
    strcpy(s,"hello");
    printf("%s\n",s);
}
//没有free(s);