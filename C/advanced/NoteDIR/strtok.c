#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(void)
{
    char str[] ="- This, a sample string.";
    char * pch;
    printf ("Splitting string \"%s\" into tokens:\n",str);
    pch = strtok (str," ,.-");
    printf("pch string : %s , pch 주소 : %p\n", pch, pch);
    while (pch != NULL)
    {
        printf("str 원본 = <  %s  > \n", str);
        printf("pch string : [  %s  ] , pch 주소 : %p\n", pch, pch);
        pch = strtok (NULL, " ,.-");
    }




    return 0;
}
