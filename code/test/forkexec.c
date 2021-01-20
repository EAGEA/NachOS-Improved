#include "syscall.h"

int main()
{
	//int i;
	ForkExec("./userpages0") ;
	PutString("BIIITCH\n");
	int i;
	for(i=0;i<15;i++){
		PutString("Forkexec: ");
		PutInt(i);
		PutChar('\n');
	}
	Halt();
}
