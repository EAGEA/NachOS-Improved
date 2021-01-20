#include "syscall.h"

void print(void *c)
{
	int i ;

	for (i = 0 ; i < 10 ; i ++)
	{
		PutChar(*((char *) c) + (i % 26)) ;
	}
}

int main()
{
	//char a = 'a' ;
	PutString("FUUUUCK\n");
	//UserThreadCreate(print, &a) ;
	//UserThreadCreate(print, &a) ;

	//PutString("End of userpages0.main.\n") ;
	Halt();
}
