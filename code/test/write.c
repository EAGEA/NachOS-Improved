#include "syscall.h"


int main()
{
	OpenFileId id = Open("testwrite", 'w') ;
	Write("abcdefghijklmnopqrstuvwxyz", 26, id) ;
}
