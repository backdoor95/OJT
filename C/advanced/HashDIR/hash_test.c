#include<stdio.h>
#include<stdlib.h>

int main(void)
{
    int x = -3;
    int M = 11; // 배열의 용량
    int* pnum = &M;

    printf("x%M = %d\n", x%M);

    printf("size = %d\n", sizeof(M));
    printf("pointer size = %d\n", sizeof(pnum));

    return 0;
}
