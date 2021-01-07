#include "copyright.h"
#include "system.h"
#include "syscall.h"

/* MIPS -> LINUX string conversion.
 */
void CopyStringFromMachine(int from, char *to, unsigned size)
{
	unsigned i ;
	int ch = 1 ;

	for (i = 0 ; i < size - 1 && ch != '\0'; i ++)
	{
		if (machine->ReadMem(from + i, 1, &ch))
		{
			to[i] = (char) ch ; 
		}
	}

	to[i] = '\0' ;
}

/* LINUX -> MIPS string conversion.
 */
void CopyStringToMachine(char *from, int to, unsigned size)
{
	unsigned i ;

	for (i = 0 ; i < size - 1 && from[i] != '\0'; i ++)
	{
		if (! machine->WriteMem(to + i, sizeof(char), from[i]))
		{
			printf("Can't write in memory string.");
		}
	}

	if (! machine->WriteMem(to + i, sizeof(char), '\0')) 
	{
		printf("Can't write in memory end of string.");
	}
}
