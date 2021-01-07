#include "syscall.h"

int main()
{
	int i ;

    GetInt(&i) ;
	PutInt(i) ;
	PutChar('\n') ;

    GetInt(&i) ;
	PutInt(i) ;
	PutChar('\n') ;

    GetInt(&i) ;
	PutInt(i) ;
	PutChar('\n') ;

	Exit(0) ;
}
