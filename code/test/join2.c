#include "syscall.h"

void loopbillion()
{
	int i ;
	for(i=0;i<1000000;i++)
	{
		;
	}
	PutString("User thread ending : ") ;
	PutInt(UserThreadId()) ;
	PutChar('\n') ;
	UserThreadExit() ;
}

void loophundred()
{
	int i ;
	for(i=0;i<100;i++)
	{
		;
	}
	PutString("User thread ending : ") ;
	PutInt(UserThreadId()) ;
	PutChar('\n') ;
	UserThreadExit() ;
}

void testdupl()
{
	int tid ;
	tid = UserThreadCreate(loopbillion,0) ;
	UserThreadJoin(tid) ;
	PutString("User thread ending : ") ;
	PutInt(UserThreadId()) ;
	PutChar('\n') ;
	UserThreadExit() ;
}

void testdupl2()
{
	int tid ;
	tid = UserThreadCreate(loophundred,0) ;
	UserThreadJoin(tid) ;
	PutString("User thread ending : ") ;
	PutInt(UserThreadId()) ;
	PutChar('\n') ;
	UserThreadExit() ;
}

int main()
{
	int a , b ;

	PutString("Starting main thread.\n") ;

	a = UserThreadCreate(testdupl2,0) ;

	b = UserThreadCreate(testdupl,0) ;

	UserThreadJoin(b) ;
	UserThreadJoin(a) ;

	PutString("Main thread ending.\n") ;
}
