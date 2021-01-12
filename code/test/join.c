#include "syscall.h"

void print(void *c)
{
	int i ;

	for (i = 0 ; i < 4 ; i ++)
	{
		PutChar(*((char *) c)) ;
	}

	PutString("User thread ending.\n") ;

	UserThreadExit() ;
}

void printb(void *c)
{
	int j ;

	for (j = 0 ; j < 4 ; j ++)
	{
		PutChar(*((char *) c)) ;
	}

	PutString("User thread ending.\n") ;

	UserThreadExit() ;
}

int main()
{
	PutString("Starting main thread.\n") ;

	char a = 'a' ;
	char b = 'b' ;


	UserThreadCreate(print, &a) ;

	int i ;
	for (i = 0 ; i < 0 ;  i++)
	{

	}
	UserThreadCreate(printb, &b) ;


	PutString("Main thread ending.\n") ;

	Halt() ;
}
