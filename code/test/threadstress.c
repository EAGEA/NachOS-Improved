#include "syscall.h"

void print()
{
	for (int i = 0 ; i < 10000 ; i ++)
	{
		// To delay.
	}

	PutString("End thread\n") ;
	UserThreadExit();
}

int main()
{
	PutString("Starting main thread.\n") ;

	int i ;
	for (i = 0 ; i < 16 ; i ++)
	{
		PutString("Create thread.\n") ;
		if(UserThreadCreate(print, 0)==-1){ PutString("Not possible to create\n") ; break; }
		
	}
	Exit(0);
}
