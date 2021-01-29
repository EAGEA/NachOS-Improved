#include "syscall.h"


int main()
{
	char alphabet[26] ;

	OpenFileId id = Open("testread", 'r') ;
	Read(alphabet, 26, id) ;

	PutString(alphabet) ;
	PutChar('\n') ;
}
