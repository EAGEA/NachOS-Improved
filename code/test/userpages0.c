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
	//UserThreadCreate(print, &a) ;
	//UserThreadCreate(print, &a) ;
	int i;
	for(i=0;i<15;i++){
		PutString("Userpages: ");
		PutInt(i);
		PutChar('\n');
	}
	//PutString("End of userpages0.main.\n") ;
}
