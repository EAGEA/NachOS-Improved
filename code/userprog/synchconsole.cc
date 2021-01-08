#include "copyright.h"
#include "system.h"
#include "synchconsole.h"
#include "synch.h"

static Semaphore *readAvail;
static Semaphore *writeDone;
static void ReadAvail(int arg) { readAvail->V(); }
static void WriteDone(int arg) { writeDone->V(); }

SynchConsole::SynchConsole(char *readFile, char *writeFile)
{
	readAvail = new Semaphore("read avail", 0);
	writeDone = new Semaphore("write done", 0);
	console = new Console(readFile, writeFile, ReadAvail, WriteDone, 0) ;
}

SynchConsole::~SynchConsole()
{
	delete console;
	delete writeDone;
	delete readAvail;
}

void SynchConsole::SynchPutChar(const char ch)
{
	console->PutChar(ch) ;
	writeDone->P() ;
}

/* Updated version to detect only EOF on an end of file.
 */
int SynchConsole::SynchGetChar()
{
	readAvail->P() ;
	return console->GetChar() ;
}

void SynchConsole::SynchPutString(const char s[])
{
	if (! s || s[0] == '\0')
	{
		return ;
	}

	console->PutString(s) ;	
	writeDone->P() ;
}

void SynchConsole::SynchGetString(char *s, int n)
{
	if (! s || n == 0)
	{
		return ;
	}

	char inputBuffer[MAX_STRING_SIZE] ;
	int i = 0 ;
	// Fill the buffer with input.
	while (i < MAX_STRING_SIZE 
			&& (i == 0 || (inputBuffer[i - 1] != EOF && inputBuffer[i - 1] != '\n' && inputBuffer[i - 1] != '\0')))
	{
		inputBuffer[i ++] = SynchGetChar() ;
	}

	i = 0 ;
	// Fill the return value with the buffer.
	while (i < n)
	{
		char c = inputBuffer[i] ;

		// Fgets behavior.
		if (c == '\n')
		{
			s[i ++] = '\n' ; 
			s[i] = '\0' ;
			return ;
		}
		else if (c == EOF || c == '\0' || i == n - 1)
		{
			s[i] = '\0' ;
			return ;
		}

		s[i] = inputBuffer[i] ; 
		i ++ ;
	}
}

void SynchConsole::SynchPutInt(int i)
{
	 char n[MAX_STRING_SIZE] ;
	 
	 snprintf(n, MAX_STRING_SIZE, "%i", i) ;
	 SynchPutString(n) ;
}

void SynchConsole::SynchGetInt(int *i)
{
	char n[MAX_STRING_SIZE] ;

	SynchGetString(n, MAX_STRING_SIZE) ;
	sscanf(n, "%i", i) ;
}

/* Return true if EOF reached previously.
 */
int SynchConsole::feof()
{
	return console->feof() ;
}	

/* MIPS -> Linux string conversion.
 */
void SynchConsole::CopyStringFromMachine(int from, char *to, unsigned size)
{
	unsigned i ;
	int ch = 1 ;

	for (i = 0 ; i < size - 1 && ch != '\0'; i ++)
	{
		// Read the string in the machine main memory.
		if (machine->ReadMem(from + i, 1, &ch))
		{
			to[i] = (char) ch ; 
		}
	}

	to[i] = '\0' ;
}

/* Linux -> MIPS string conversion.
 */
void SynchConsole::CopyStringToMachine(char *from, int to, unsigned size)
{
	unsigned i ;

	for (i = 0 ; i < size - 1 && from[i] != '\0'; i ++)
	{
		// Write the string in the machine main memory.
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
