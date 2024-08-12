#include<stdio.h>
#define SQR(x) (x)*(x)
// 매크로에서는 매개 변수에 괄호를 사용하라.
int main()
{
	int result = SQR(1+2);
	printf("result = %d\n", result);
	return 0;
}
