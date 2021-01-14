#include "syscall.h"

void process(void *c)
{
	int i ;

	for (i = 0 ; i < 50000 ; i ++)
	{
		// Process a task.
	}

	PutString("User thread 3 ending.\n") ;

//	UserThreadExit() ;
}

void processAndJoin(void *c)
{
	int i ;

	for (i = 0 ; i < 50000 ; i ++)
	{
		// Process a task.
	}

	int tid = UserThreadCreate(process, &i) ;

	UserThreadJoin(tid) ;

	PutString("User thread 2 ending.\n") ;

//	UserThreadExit() ;
}

void join(void *c)
{
	int i = 2 ;

	int tid = UserThreadCreate(processAndJoin, &i) ;

	UserThreadJoin(tid) ;

	PutString("User thread 1 ending.\n") ;

//	UserThreadExit() ;
}

int main()
{
	PutString("Starting main thread.\n") ;

	int i = 1 ;

	int tid = UserThreadCreate(join, &i) ;

	UserThreadJoin(tid) ;

	PutString("Main thread ending.\n") ;

	Halt() ;
}
