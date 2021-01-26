#include "syscall.h"


int main()
{
	if (CreateFile("cftest", 100))
	{
		PutString("File created.\n") ;

		if (RemoveFile("cftest"))
		{
			PutString("File removed.\n") ;
		}
		else
		{
			PutString("File not removed.\n") ;
		}
	}
	else
	{
		PutString("File not created.\n") ;
	}
}
