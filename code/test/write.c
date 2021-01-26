#include "syscall.h"


int main()
{
	OpenFileId id = Open("test/testwrite.txt", 'w') ;
	Write("abcdefghijklmnopqrstuvwxyz", 26, id) ;
}
