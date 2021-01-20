#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "userprocess.h"
#include "threadparams.h"
#include <stdio.h>

static void StartUserProcess(int f) 
{
	// Similar to userthread.startUserThread method.
	//printf("oh yes I'm again here\n");
	currentThread->space->InitRegisters() ;
	currentThread->space->RestoreState() ;
	// Start.
	machine->Run() ;
	// Should never be reached.
	ASSERT(FALSE) ;
}

int do_UserProcessCreate(char *execFile)
{
	// Similar to progtest.startProcess method.
	// Get the executable.
	OpenFile *exec = fileSystem->Open(execFile) ;
	//printf("and then here\n");
	if (! exec) 
	{
		//printf("Nope\n");
		// A wrong executable was specified.
		return -1 ;
	}
	// Create the associated addr space.
	AddrSpace *addrSpace ;
	addrSpace = new AddrSpace(exec) ;
	delete exec ;
	// Add this process to the process count.
	machine->ProcessesLockAcquire() ;
	machine->SetNbProcesses(machine->GetNbProcesses() + 1) ;
	machine->ProcessesLockRelease() ;
	// Create the params for the "Fork" function.
	ThreadParams *params = new ThreadParams(0, 0, 0, false) ;
	// Create the object with the same space as the current, and start it.
	Thread *thread = new Thread(execFile, 1, 0) ;
	thread->space = addrSpace ; 
	thread->Fork(StartUserProcess, (int) params) ;
	return 0 ;
}

void do_UserProcessExit()
{
	// Wait for all the children.
	AddrSpace *currentSpace = currentThread->space ;

	currentSpace->ThreadIDLockAcquire() ;

	while (currentSpace->GetTotalThreads() > 1)
	{
		// Exit when only the main thread is remaining.
		currentSpace->ThreadExitConditionWait() ;
	}

	currentSpace->ThreadIDLockRelease() ;

	// Remove this process from the process count.
	machine->ProcessesLockAcquire() ;
	int nbProcesses = machine->GetNbProcesses() - 1 ;
	machine->SetNbProcesses(nbProcesses) ;
	machine->ProcessesLockRelease() ;

	if (nbProcesses == 0)
	{
		// The last one needs to stop the machine.
		DEBUG('a', "The process ended up correctly.\n");
		interrupt->Halt() ;
	}
	else
	{
		// No needs to remove this thread from the living ones, and SP id. 
		// Finish the thread.
		currentThread->Finish();
		// Clean it.
		delete currentThread->space ;
	}
}

void do_UserProcessHalt()
{
	DEBUG('a', "Shutdown, initiated by user program.\n");
	interrupt->Halt() ;
}
