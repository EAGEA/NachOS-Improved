#include "syscall.h"

int main()
{
	ForkExec("userpage0") ;

	int i;
	for(i=0;i<15;i++){
		PutString("Forkexec: ");
		PutInt(i);
		PutChar('\n');
	}
}
