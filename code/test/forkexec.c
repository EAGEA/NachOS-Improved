#include "syscall.h"

int main()
{
	//int i;
	ForkExec("./userpages0") ;
	PutString("BIIITCH\n");
	Halt();
}
