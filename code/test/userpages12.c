#include "syscall.h"

void print()
{
	int tid ;
	tid = UserThreadId() ;
	PutInt(tid) ;
	PutChar('\n') ;
}

int main()
{
	int i;
	for(i=0;i<12;i++){
		PutString("Processus : Création d'un thread\n") ;
		UserThreadCreate(print,0) ;
	}
}
