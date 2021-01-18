#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "userprocess.h"
#include "threadparams.h"


Lock *lock = new Lock("User process lock") ;


static void StartUserProcess(int arg) 
{
	// Similar to userthread.startUserThread method.
	currentThread->space->InitRegisters();
	//currentThread->space->RestoreState();
	machine->Run();
	// Should never be reached.
	ASSERT(FALSE) ;
}

int do_UserProcessCreate(char *execFile, int returnFun)
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
	// Add this process to the process count.
	machine->ProcessesLockAcquire() ;
	machine->SetNbProcesses(machine->GetNbProcesses() + 1) ;
	machine->ProcessesLockRelease() ;
	// Create the params for the "Fork" function.
	ThreadParams *params = new ThreadParams(0, 0, returnFun, false) ;
	// Create the object with the same space as the current, and start it.
	Thread *thread = new Thread(execFile, 1, 0) ;
	thread->space = addrSpace ; 
	thread->Fork(StartUserProcess, (int) params) ;

	lock->Release();

	return 0 ;
}

void do_UserProcessExit()
{
	// Remove this process from the process count.
	machine->ProcessesLockAcquire() ;
	machine->SetNbProcesses(machine->GetNbProcesses() - 1) ;
	machine->ProcessesLockRelease() ;
	
	if (machine->GetNbProcesses() == 0)
	{
		// The last one needs to stop the machin.
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
