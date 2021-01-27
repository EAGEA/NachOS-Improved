#include "syscall.h"


int main(){
	int a=ForkExec("proc1");
	WaitPid(a);
	int b=ForkExec("proc2");
	WaitPid(b);
	int c=ForkExec("proc3");
	WaitPid(c);
	Exit(0);
}
