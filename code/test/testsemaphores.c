#include "syscall.h"


int count=0;
int sid;


void increment(void* arg){
	int i=0;
	while(i<1000){
		SemWait(sid);
		count++;
		SemPost(sid);
		i++;
	}
	UserThreadExit();
	return;
}

int main(){
	sid=SemCreate(1);
	int a = UserThreadCreate(increment,0) ;
	int b = UserThreadCreate(increment,0) ;
	UserThreadJoin(a);
	UserThreadJoin(b);
	PutInt(count);
	PutChar('\n');
	SemDelete(sid);
	Halt();
}
