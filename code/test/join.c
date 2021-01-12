#include "syscall.h"

void print(void *c)
{
	int i ;

	for (i = 0 ; i < 100 ; i ++)
	{
		PutChar(*((char *) c)) ;
	}

	PutChar('\n') ;

	UserThreadExit() ;
}

int main()
{
	PutString("Starting main thread.\n") ;

	char c = 'a' ;

	int t1 = UserThreadCreate(print,&c) ;

	int res = UserThreadJoin(t1) ;

	PutInt(res) ;

	PutString("\nHey\n") ;

	Halt() ;
}
