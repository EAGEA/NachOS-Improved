#include "syscall.h"

int main()
{
	char string[512] ;

	GetString(string, 512) ;
	PutString(string) ;

	GetString(string, 512) ;
	PutString(string) ;

	GetString(string, 512) ;
	PutString(string) ;

	Exit(0) ;
}
