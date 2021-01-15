#include "copyright.h"
#include "system.h"
#include "synchconsole.h"
#include "synch.h"

// Input/output general protection.
// -> "readAvail" to wait before reading a char (=> so a string).
// -> "writeDone" to signal that a char/string was written.
static Semaphore *readAvail;
static Semaphore *writeDone;
static void ReadAvail(int arg) { readAvail->V(); }
static void WriteDone(int arg) { writeDone->V(); }
// Input/output thread protection.
// -> allow only one thread to enter a "synch" function of read/write type.
static Lock *threadWrite ;
static Lock *threadRead ;

SynchConsole::SynchConsole(char *readFile, char *writeFile)
{
	console = new Console(readFile, writeFile, ReadAvail, WriteDone, 0) ;
	threadWrite = new Lock("thread output avail") ;
	threadRead = new Lock("thread input avail") ;
	readAvail = new Semaphore("read avail", 0);
	writeDone = new Semaphore("write done", 0);
}

SynchConsole::~SynchConsole()
{
	delete console;
	delete threadWrite ;
	delete threadRead ;
	delete writeDone;
	delete readAvail;
}

void SynchConsole::SynchPutChar(const char ch)
{
	threadWrite->Acquire() ;

	console->PutChar(ch) ;
	writeDone->P() ;

	threadWrite->Release() ;
}

/* Updated version to detect only EOF on an end of file.
 * If is called in "GetString", we don't lock the semaphore
 * for multithreading, because "GetString" has already done
 * it.
*/
int SynchConsole::SynchGetChar(bool isCalledInGetString)
{
	if (! isCalledInGetString)
	{
		threadRead->Acquire() ;
	}

	readAvail->P() ;
	char c = console->GetChar() ; 

	if (! isCalledInGetString)
	{
		threadRead->Release() ;
	}

	return c ;  
}

void SynchConsole::SynchPutString(const char s[])
{
	if (! s || s[0] == '\0')
	{
		return ;
	}

	threadWrite->Acquire() ;

	console->PutString(s) ;	
	writeDone->P() ;

	threadWrite->Release() ;
}

void SynchConsole::SynchGetString(char *s, int n)
{
	if (! s || n == 0)
	{
		return ;
	}
	else if (n == 1)
	{
		s[0] = '\0' ;
		return ;
	}

	threadRead->Acquire() ;

	int i = 0 ;

	s[i] = SynchGetChar(true) ;

	while (i < n - 1 && s[i] != EOF && s[i] != '\n' && s[i] != '\0') 
	{
		s[++ i] = SynchGetChar(true);
	}

	s[i + 1] = '\0' ;

	threadRead->Release() ;
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
bool SynchConsole::Feof()
{
	return console->Feof() ;
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
