#include "syscall.h"

void print(void *c)
{
	PutString("Starting thread function.\n") ;

	int i ;

	for (i = 0 ; i < 15 ; i ++)
	{
		PutChar(*((char *) c) + i % 26) ;
	}

	PutString("\nEnding thread function.\n") ;

	UserThreadExit() ;
}

int main()
{
	char c = 'a' ;

	if (UserThreadCreate(print, &c) == -1) 
	{
		PutString("Can't create thread.\n") ;
	}
	else
	{
		PutString("Thread created.\n") ;
	}

	PutString("Thread exited.\n") ;

	Halt() ;
}
