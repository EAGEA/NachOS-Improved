#include "syscall.h"

void print(int n)
{
	char string[n + 1] ;
	int i ;

	for (i = 0 ; i < n - 1 ; i ++)
	{
		string[i] = 'a' + i % 26 ; 
	}

	string[n] = '\0' ;

	PutString(string) ;
}

void println(int n)
{
	char string[n + 2] ;
	int i ;

	for (i = 0 ; i < n ; i ++)
	{
		string[i] = 'a' + i % 26 ; 
	}

	string[n] = '\n' ;
	string[n + 1] = '\0' ;

	PutString(string) ;
}

int main()
{
	int i ;

	for (i = 1 ; i <= 512 ; i *= 2)
	{
		println(i) ;
	}

	Exit(0) ;
}
