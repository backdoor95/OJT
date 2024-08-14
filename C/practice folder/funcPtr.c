#include<stdio.h>
void hello()
{
	printf("Hello world func\n");
}
void bonjour()
{
	printf("bonjour le monde\n");
}
int main()
{
	// 반환값 자료형(*함수포인터 이름)();
	void (*fp)();
	
	fp = hello;
	fp();
	fp = &hello;
	fp();

	fp = bonjour;
	fp();
	fp = &bonjour;
	fp();
	return 0;
}
