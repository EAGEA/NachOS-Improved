#include "syscall.h"

void print(void *c)
{
	PutString("Starting user thread function.\n") ;

	int i ;

	for (i = 0 ; i < 26 ; i ++)
	{
		PutChar(*((char *) c) + i % 26) ;
	}

	PutString("\nEnding user thread function.\n") ;

//	UserThreadExit() ;
}

int main()
{
	PutString("Starting main thread.\n") ;

	char c = 'a' ;

	if (UserThreadCreate(print, &c) == -1) 
	{
		PutString("Can't create user thread.\n") ;
	}
	else
	{
		PutString("User thread created.\n") ;
	}

	PutString("Waiting for ending main thread.\n") ;

	Halt() ;
}
