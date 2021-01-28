#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "userthread.h"
#include "threadparams.h"



static void StartUserThread(int f)
{
	// Unpack the params.
	ThreadParams *params = (ThreadParams*) f ;
	// Clean the registers.
	currentThread->space->InitRegisters() ;
	// Init page table.
	currentThread->space->RestoreState() ;
	// Write the argument of the function. 
	machine->WriteRegister(4, params->GetArg()) ;
	// Write PC to the start of the function. 
	machine->WriteRegister(PCReg, params->GetFun()) ;
	// Write next PC to the start of the function (because of branch delay). 
	machine->WriteRegister(NextPCReg, params->GetFun() + 4) ;
	// Write the function which will be executed at the end of the thread.
	machine->WriteRegister(RetAddrReg, params->GetReturnFun()) ;
	// Write the stack pointer. 
    machine->WriteRegister(StackReg, currentThread->getSP()) ;
	// Start.
	machine->Run() ;
	// machine->Run never returns.
	ASSERT(FALSE) ;	 	
}

int do_UserThreadCreate(int fun, int arg, int returnFun)
{
	AddrSpace *currentSpace = currentThread->space ;
	// Get the total of threads currently existing in this addr space. 
	currentSpace->ThreadIDLockAcquire() ;
	int totalThreads = currentSpace->GetTotalThreads() + 1 ;
	// Check if we can create this thread:
	if (totalThreads > UserThreadMax)
	{
		// Error: the maximum of user threads created was reached.
		currentSpace->ThreadIDLockRelease() ;
		return -1 ;
	}
	// Add this thread to the living ones. 
	int tid = currentSpace->GetNextThreadID() ;
	currentSpace->AddThreadID(tid) ;
	currentSpace->ThreadIDLockRelease() ;
	// Get the stack pointer.
	currentSpace->ThreadStackLockAcquire() ;
    int sp = currentSpace->GetThreadStackPointer() ;
	currentSpace->ThreadStackLockRelease() ;
	// Create the params for the "Fork" function.
	ThreadParams *params = new ThreadParams(fun, arg, returnFun, true) ;
	// Then create thread name for debugging.
	char *name = (char *) malloc(sizeof(char) * 18) ;
	sprintf(name, "User thread n°%d", tid) ;
	// Create the object with the same space as the current, and start it.
	Thread *thread = new Thread(name, tid, sp) ;
	thread->space = currentSpace ; 
	thread->Fork(StartUserThread, (int) params) ;
	return tid ; 
}

void do_UserThreadExit()
{
	AddrSpace *currentSpace = currentThread->space ;
	// Remove this thread from the living ones. 
	currentSpace->ThreadIDLockAcquire() ;
	currentSpace->RemoveThreadID(currentThread->getTid()) ;
	currentSpace->ThreadIDLockRelease() ;
	// Remove the stack pointer of this thread.
	currentSpace->ThreadStackLockAcquire() ;
	currentSpace->RemoveThreadStackPointer(currentThread->getSP()) ;
	currentSpace->ThreadStackLockRelease() ;
	// Finish the thread.
	currentThread->Finish();
	// Clean it.
	delete currentThread->space ;
}

int do_UserThreadJoin(int t) 
{
	AddrSpace *currentSpace = currentThread->space ;
	
	if(currentThread->getTid()==t){
		return -1;
	}
	
	currentSpace->ThreadIDLockAcquire() ;

	if (! currentSpace->ContainThreadID(t))
	{
		currentSpace->ThreadIDLockRelease() ;

		return -1 ; 
	}

	while (currentSpace->ContainThreadID(t))
	{
		// The thread is still living in this addr space.
		currentSpace->ThreadJoinConditionWait(t) ;
	}

	currentSpace->ThreadIDLockRelease() ;

	return 0 ;
}

int do_UserThreadId()
{
	return currentThread->getTid() ;
}
