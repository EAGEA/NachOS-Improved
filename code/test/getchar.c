#include "syscall.h"

int main()
{
	char c = GetChar() ;
	PutChar(c) ;

	Exit(0) ;
}
