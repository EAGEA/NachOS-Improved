#include "syscall.h"

int main()
{
	int i ;
	for(i = 0;i < 12;i++)
	{
		PutString("Main : Création d'un processus \n") ;
		ForkExec("./multithreads") ;
	}
}
