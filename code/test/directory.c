#include "syscall.h"


int main()
{
	if (CreateDir("cftest"))
	{
		PutString("Dir created.\n") ;

		if (RemoveDir("cftest"))
		{
			PutString("Dir removed.\n") ;
		}
		else
		{
			PutString("Dir not removed.\n") ;
		}
	}
	else
	{
		PutString("Dir not created.\n") ;
	}
}
