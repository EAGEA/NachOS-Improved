#include "syscall.h"

void print(void *c)
{
	int i ;
	char ch = *((char *) c) ;

	for (i = 0 ; i < 4 ; i ++)
	{
		PutChar(ch) ;
	}

	PutString("User thread print ending.\n") ;

//	UserThreadExit() ;
}

void printb(void *c)
{
	int j ;
	char ch = *((char *) c) ;

	for (j = 0 ; j < 4 ; j ++)
	{
		PutChar(ch) ;
	}

	PutString("User thread print bis ending.\n") ;

//	UserThreadExit() ;
}

int main()
{
	PutString("Starting main thread.\n") ;

	char a = 'a' ;
	char b = 'b' ;

	UserThreadCreate(print, &a) ;
	UserThreadCreate(printb, &b) ;

	Halt() ;
}
