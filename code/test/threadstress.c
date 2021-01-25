#include "syscall.h"

void print()
{
	for (int i = 0 ; i < 50000 ; i ++)
	{
		// To delay.
	}

	PutString("Thread\n") ;
}

int main()
{
	PutString("Starting main thread.\n") ;

	int i ;

	for (i = 0 ; i < 16 ; i ++)
	{
		UserThreadCreate(print, 0) ;
	}
}
