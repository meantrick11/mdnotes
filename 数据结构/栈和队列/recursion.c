#include <stdio.h>

//test1.c文件
int sum1(int n){
    //非递归求和
    int sum = 0;
    for (int i =1;i<=n;i++){
        sum += i;
    }
    return sum;
}

//test2.c文件
int sum2(int n){
    //递归求和
    if (n==1){
        return 1;
    }
    else{
        return n+sum(n-1);
    }
}

//test3.c文件
int fibonacci(int n){
    //非递归求解斐波那契数列
    int last1 = 1;
    int last2 = 1;
    int result = 0;
    for (int i =3;i<=n;i++){
        result = last1 + last2;
        last1 = last2;
        last2 = result;
    }
    return result;
}

//test4.c文件
int fibonacci(int n){
    //递归求解斐波那契数列
    if (n==1 || n==2){
        return 1;
    }
    else{
        return fibonacci(n-1)+fibonacci(n-2);
    }
}