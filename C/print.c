#include<stdio.h>
int main()
{
	puts("hello world\n");
	fputs("hello world\n", stdout);
	printf("hello world\n");
	fprintf(stdout, "hello world \n");
	return 0;
}
