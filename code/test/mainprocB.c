#include "syscall.h"


int main(){
	ForkExec("proc1");
	//WaitPid(a);
	ForkExec("proc2");
	//WaitPid(b);
	ForkExec("proc3");
	//WaitPid(c);
	Exit(0);
}
