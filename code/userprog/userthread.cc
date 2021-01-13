#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "userthread.h"



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
	currentSpace->ExitLockAcquire() ;
	int totalThreads = currentSpace->GetTotalThreads() + 1 ;
	currentSpace->SetTotalThreads(totalThreads) ;
	currentSpace->ExitLockRelease() ;
	// Create the params for the "Fork" function.
	ThreadParams *params = new ThreadParams(f, arg) ;
	// Then create thread name for debugging.
	char *name = (char *) malloc(sizeof(char) * 18) ;
	sprintf(name, "User thread n°%d", totalThreads) ;
	// To create the object with the same space as the current, and start it.
	Thread *thread = new Thread(name, totalThreads) ;
	thread->space = currentSpace ; 
	thread->Fork(StartUserThread, (int) params) ;

	return totalThreads ; 
}

void do_UserThreadExit()
{
	AddrSpace *currentSpace = currentThread->space ;
	// Remove this one from the total of threads existing.
	currentSpace->ExitLockAcquire() ;
	currentSpace->SetTotalThreads(currentSpace->GetTotalThreads() - 1) ;
	currentSpace->ExitLockRelease() ;
	// And set the current thread as the last to finish in this address space.
	currentSpace->JoinLockAcquire() ;
	currentSpace->SetLastid(currentThread->getTid());
	currentSpace->JoinLockRelease() ;
	// Finish the thread.
	currentThread->Finish();
	// Clean it.
	delete currentThread->space ;
}

void do_UserThreadJoin(int t) 
{
	AddrSpace *currentSpace = currentThread->space ;

	currentSpace->JoinLockAcquire() ;

	while (currentSpace->GetLastid() != t)
	{
		currentSpace->JoinLockRelease() ;
		currentSpace->JoinCondWait() ;
		currentSpace->JoinLockAcquire() ;
	}

	currentSpace->JoinLockRelease() ;
}

int do_UserThreadId()
{
	return currentThread->getTid() ;
}

/** 
 * Params of the thread for the "Start" function. 
 */

ThreadParams::ThreadParams(int f, int a)
{
	fun = f ;
	arg = a ;
}

void ThreadParams::SetFun(int f)
{
	fun = f ;
}

void ThreadParams::SetArg(int a)
{
	arg = a ;
}

int ThreadParams::GetFun()
{
	return fun ;
}

int ThreadParams::GetArg()
{
	return arg ;
}
