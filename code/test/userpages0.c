#include "syscall.h"

void print(void *c)
{
	int i ;

	for (i = 0 ; i < 10 ; i ++)
	{
		PutChar(*((char *) c) + (i % 26)) ;
	}

	PutString("User thread ending.\n") ;

//	UserThreadExit() ;
}

int main()
{
	PutString("Starting main thread.\n") ;

	char a = 'a' ;

	UserThreadCreate(print, &a) ;
	UserThreadCreate(print, &a) ;

	PutString("Main thread ending.\n") ;
}
