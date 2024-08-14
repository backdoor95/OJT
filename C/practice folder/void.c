#include<stdio.h>
int main()
{
	int num1 =10;
	float num2 = 3.5f;
	char c1 = 'a';
	void *ptr;
	ptr = &num1;
	printf("%d\n", *(int*)ptr);
	return 0;
}
