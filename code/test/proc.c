#include "syscall.h"

int main(){
	int k=0;
	while((k++)<(10000)){};
	PutInt(GetPid());
	PutChar('\n');
	Exit(0);
}
