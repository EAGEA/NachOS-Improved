#include "syscall.h"

void print()
{
	PutInt(1) ;
	PutChar('\n') ;
}

int main()
{
	PutString("Starting main thread.\n") ;

	int i ;

	for(i = 0 ; i < 16 ; i ++)
	{
		UserThreadCreate(print,0) ;
	}
	
}