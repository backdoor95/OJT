#include<stdio.h>
int main()
{
	int a;
	char* pointerArr[3] = {"zero to One","one two three four five six seven","too many code test"};
	for(int i=0;i<3;i++)
	{
		printf("%s\n", pointerArr[i]);
		printf("%d\n", sizeof(pointerArr[i]));
	}
	return 0;

}
