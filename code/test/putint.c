#include "syscall.h"

void print(int n)
{
	int i ;

	for (i = 0; i < n; i++) 
	{
		PutInt(i) ;
		PutChar('\n') ;
	}
}
	
int main()
{
	print(20) ;
	Exit(0) ;
}
