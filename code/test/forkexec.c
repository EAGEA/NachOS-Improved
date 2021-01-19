#include "syscall.h"

int main()
{
	ForkExec("build/userpages0") ;
	PutString("Fork 1 done.\n") ;
	ForkExec("build/userpages1") ;
	PutString("Fork 2 done.\n") ;
}
