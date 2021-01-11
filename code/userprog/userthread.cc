#include "copyright.h"
#include "system.h"
#include "syscall.h"

static void StartUserThread(int f)
{
	// Unpack the params.
	ThreadParams *params = (ThreadParams*) f ;
	// Clean the registers.
	currentThread->space->InitRegisters() ;
	// Write the argument of the function. 
	machine->WriteRegister(4, params->GetArg()) ;
	// Write PC to the start of the function. 
	machine->WriteRegister(PCReg, params->GetFun()) ;
	// Write next PC to the start of the function (because of branch delay). 
	machine->WriteRegister(NextPCReg, params->GetFun() + 4) ;
	// Write the stack pointer (3 pages below the main one, already init int "InitReg").
    machine->WriteRegister(StackReg, machine->ReadRegister(StackReg) - (3 * PageSize)) ; 
	// Initialize page table. 
	currentThread->space->RestoreState() ;
	// Start.
	machine->Run() ;
	// machine->Run never returns.
	ASSERT (FALSE);		
}

int do_UserThreadCreate(int f, int arg)
{
	AddrSpace *currentSpace = currentThread->space ;
	// Check if we can create this thread:
	if (false)
	{
		// Error: the maximum of user threads created was reached.
		return -1 ;
	}
	// Create the thread:
	// Add this one to the total of threads existing.
	currentSpace->MutexLock() ;
	currentSpace->SetTotalThreads(currentSpace->GetTotalThreads() + 1) ;
	currentSpace->MutexUnlock() ;
	// Create the params for the "Fork" function.
	ThreadParams *params = new ThreadParams(f, arg) ;
	// Then create the thread with the same pace as the current, and start it.
	Thread *thread = new Thread("User Thread") ; 
	thread->space = currentSpace ; 
	thread->Fork(StartUserThread, (int) params) ;

	return 0 ; 
}

void do_UserThreadExit()
{
	AddrSpace *currentSpace = currentThread->space ;
	// Remove this one from the total of threads existing.
	currentSpace->MutexLock() ;
	currentSpace->SetTotalThreads(currentSpace->GetTotalThreads() - 1) ;
	currentSpace->MutexUnlock() ;
	// Finish the thread.
	currentThread->Finish();
	// Clean it.
	delete currentThread->space ;
}

void do_UserThreadJoin() 
{
}
