#include "syscall.h"

void process(void *c)
{
	int i ;

	for (i = 0 ; i < 50000 ; i ++)
	{
		// Process a task.
	}

	PutString("User thread 3 ending.\n") ;
}

void processAndJoin(void *c)
{
	int i ;

	for (i = 0 ; i < 50000 ; i ++)
	{
		// Process a task.
	}

	int tid = UserThreadCreate(process, 0) ;

	UserThreadJoin(tid) ;

	PutString("User thread 2 ending.\n") ;
}

void join(void *c)
{
	int tid = UserThreadCreate(processAndJoin, 0) ;

	UserThreadJoin(tid) ;

	PutString("User thread 1 ending.\n") ;
}

int main()
{
	PutString("Starting main thread.\n") ;

	int tid = UserThreadCreate(join, 0) ;

	UserThreadJoin(tid) ;

	PutString("Main thread ending.\n") ;
}
