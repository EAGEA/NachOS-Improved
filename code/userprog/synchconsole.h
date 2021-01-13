#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H

#include "copyright.h"
#include "utility.h"
#include "console.h"
#include "synch.h"

class SynchConsole 
{
	public:

		SynchConsole(char *readFile, char *writeFile); // initialize the hardware console device
		~SynchConsole(); // clean up console emulation
		void SynchPutChar(const char ch); // Unix putchar(3S)
		int SynchGetChar(bool isCalledFromGetString); // Unix getchar(3S)
		void SynchPutString(const char *s); // Unix puts(3S)
		void SynchGetString(char *s, int n); // Unix fgets(3S)
		void SynchPutInt(int i);
		void SynchGetInt(int *i);
		int feof() ;
		void CopyStringFromMachine(int from, char *to, unsigned size) ; // MIPS -> LINUX string conversion.
		void CopyStringToMachine(char *from, int to, unsigned size) ; // LINUX -> MIPS string conversion.

	private:

		Console *console;
		// Input/output thread protection.
		// -> allow only one thread to enter a "synch" function of read/write type.
		Semaphore *threadWrite ;
		Semaphore *threadRead ;
};

#endif
