#include "syscall.h"

void print()
{
	for (int i = 0 ; i < 10000 ; i ++)
	{
		// To delay.
	}

	PutString("End thread\n") ;
}

int main()
{
	PutString("Starting main thread.\n") ;

	int i ;

	for (i = 0 ; i < 16 ; i ++)
	{
		PutString("Create thread.\n") ;
		UserThreadCreate(print, 0) ;
	}
}
