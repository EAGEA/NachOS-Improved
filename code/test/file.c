#include "syscall.h"


int main()
{
	if (CreateFile("cftest", 100))
	{
		PutString("File created.\n") ;

		List() ;

		if (RemoveFile("cftest"))
		{
			PutString("File removed.\n") ;

			List() ;
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
