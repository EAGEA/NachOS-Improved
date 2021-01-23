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
	int a , b ;
	print(20) ;
	a = 20 ;
	b = 22 ;
	PutInt(a+b) ;
	PutChar('\n') ;
	Exit(0) ;
}
