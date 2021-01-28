#include "syscall.h"

int main(){
	int i=0;
	while(1){
		if(ForkExec("proc")==-1){
			break;
		}
		i++;
	}
	Exit(0);
}
