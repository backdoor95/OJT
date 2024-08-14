#if 0
#include<stdio.h>

#define DBL(x) ((x)+(x))

int main()
{
	int result = DBL(2)+DBL(2);
	printf("result = %d\n", result);
	return 0;

}

#endif

#if 1
#include<stdio.h>
// 위험한 코드 2. 
#define INFO (-1)
#define WARN (-2)
#define ERR  (-3)

void log_print(int level, const char* msg)
{
	if(level INFO)
		printf("	[INFO] %s\n", msg);
	else if(level == WARN)
		printf("	[INFO] %s\n", msg);
	else if(level == ERR)
		printf("	[ERR] %s\n", msg);
}
int main()
{
	log_print(ERR, "out of service");
	return 0;
}
#endif

