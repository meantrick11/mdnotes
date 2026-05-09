#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    int *arr = (int *)malloc(sizeof(int)*5);
    for (int i = 0; i < 5 ; i++){
        arr[i] = i+1;
    }
    for (int j =0; j < 5; j++){
        printf("%d\n",arr[j]);
    }
    free(arr);
}