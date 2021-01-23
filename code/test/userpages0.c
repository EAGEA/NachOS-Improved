#include "syscall.h"

int main()
{
	int i;
	for(i=0;i<15;i++){
		PutString("Userpages: ");
		PutInt(i);
		PutChar('\n');
	}
	PutString("End of userpages0.main.\n") ;
}
