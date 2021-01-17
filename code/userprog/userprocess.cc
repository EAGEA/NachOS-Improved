#include "system.h"
#include "userprocess.h"
#include "userthread.h"


Lock *lock = new Lock("User process lock") ;


static void StartUserProcess(int arg) 
{
	// Similar to userthread.startUserThread method.
	currentThread->space->InitRegisters();
	currentThread->space->RestoreState();
	machine->Run();
	// Should never be reached.
	ASSERT(FALSE) ;
}

int do_UserProcessCreate(char *execFile)
{
	// Similar to progtest.startProcess method.
	lock->Acquire();

	// Get the executable.
	OpenFile *exec = fileSystem->Open(execFile) ;

	if (! exec) 
	{
		// A wrong executable was specified.
		lock->Release();
		return -1 ;
	}
	// Create the associated addr space.
	AddrSpace *addrSpace ;
	addrSpace = new AddrSpace(exec) ;
	delete exec ;
	// Create the associated thread.
	Thread *thread = new Thread(execFile, 1, 0) ;
	thread->space = addrSpace ;
	// Start the execution.
	thread->Fork(StartUserProcess, 0) ;

    lock->Release();

	return 0 ;
}
