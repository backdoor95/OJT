#include "Common.h"
int f(int x);
int main()
{
	int n;
	scanf("%d", &n);
	int retval = f(n);
	if(retval < 0) err_quit("f()");
	else err_display("f()");
	return 0;
}
int f(int x)
{

	if(x>=0)
	{
		errno = 0;
		
		return 0;
	}
	else
	{
		errno = EINVAL;
		return -1;
	}
}
