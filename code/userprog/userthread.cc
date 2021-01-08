#include "copyright.h"
#include "system.h"
#include "syscall.h"


static void StartUserThread(int f)
{
	// Unpack the params.
	ThreadParams *params = (ThreadParams*) f ;
	// Initialize the registers.
	currentThread->space->InitRegisters() ;
	currentThread->space->RestoreState() ;
	// Write the argument of the function. 
	machine->WriteRegister(4, params->GetArg()) ;
	// Write PC to the start of the function. 
	machine->WriteRegister(PCReg, params->GetFun()) ;
	// Initialize PC to the start of the function. 
	machine->WriteRegister(NextPCReg, params->GetFun() + 4) ;
	// Initialize the stack pointer (3 pages below the main one, already init int "InitReg").
    machine->WriteRegister (StackReg, machine->ReadRegister(StackReg) - (3 * PageSize)) ; 
	// Start.
	machine->Run() ;
}

int do_UserThreadCreate(int f, int arg)
{
	// Create the params for the "Fork" function.
	ThreadParams *params = new ThreadParams(f, arg) ;
	// Then create the thread with the same pace as the current, and start it.
	Thread *thread = new Thread("User Thread") ;
	thread->space = currentThread->space ;
	thread->Fork(StartUserThread, (int) params) ;

	if (false)
	{
		// Error: the maximum of user threads created was reached.
		return -1 ;
	}

	return 0 ; 
}

void do_UserThreadExit()
{
	delete currentThread->space ;
	// Finish the thread.
	currentThread->Finish();
}

void do_UserThreadJoin() 
{
	// Use a condition variable to wait for the current thread to finish.
}
