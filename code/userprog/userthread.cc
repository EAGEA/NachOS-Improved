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
	// Get the total of threads currently existing in this addr space. 
	currentSpace->ThreadLockAcquire() ;
	int totalThreads = currentSpace->GetTotalThreads() + 1 ;
	// Check if we can create this thread:
	if (totalThreads > MAX_USER_THREADS)
	{
		// Error: the maximum of user threads created was reached.
		return -1 ;
	}
	// Add this thread to the living ones. 
	int tid = currentSpace->GetNextThreadID() ;
	currentSpace->AddThreadID(tid) ;
	currentSpace->ThreadLockRelease() ;
	// Create the params for the "Fork" function.
	ThreadParams *params = new ThreadParams(f, arg) ;
	// Then create thread name for debugging.
	char *name = (char *) malloc(sizeof(char) * 18) ;
	sprintf(name, "User thread n°%d", tid) ;
	// Create the object with the same space as the current, and start it.
	Thread *thread = new Thread(name, tid) ;
	thread->space = currentSpace ; 
	thread->Fork(StartUserThread, (int) params) ;

	return tid ; 
}

void do_UserThreadExit()
{
	AddrSpace *currentSpace = currentThread->space ;
	// Remove this thread from the living ones. 
	currentSpace->ThreadLockAcquire() ;
	currentSpace->RemoveThreadID(currentThread->getTid()) ;
	currentSpace->ThreadLockRelease() ;
	// Finish the thread.
	currentThread->Finish();
	// Clean it.
	delete currentThread->space ;
}

void do_UserThreadJoin(int t) 
{
	AddrSpace *currentSpace = currentThread->space ;

	currentSpace->ThreadLockAcquire() ;

	while (currentSpace->ContainThreadID(t))
	{
		// The thread is still living in this addr space.
		currentSpace->ThreadCondWait() ;
	}

	currentSpace->ThreadLockRelease() ;
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
