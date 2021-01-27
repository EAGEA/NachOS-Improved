#include "syscall.h"

void print(void* arg){
	int i;
	for(i=0;i<16;i++){
		PutString((char*) arg);
	}
	UserThreadExit();
} 



int main(){
	
	int i;
	for(i=0;i<4;i++){
		UserThreadCreate(print,"buenas tardes\n");
	}
	Exit(0);
}
