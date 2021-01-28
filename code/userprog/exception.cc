// exception.cc 
//      Entry point into the Nachos kernel from user programs.
//      There are two kinds of things that can cause control to
//      transfer back to here from user code:
//
//      syscall -- The user code explicitly requests to call a procedure
//      in the Nachos kernel.  Right now, the only function we support is
//      "Halt".
//
//      exceptions -- The user code does something that the CPU can't handle.
//      For instance, accessing memory that doesn't exist, arithmetic errors,
//      etc.  
//
//      Interrupts (which can also cause control to transfer from user
//      code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "userthread.h"
#include "userprocess.h"
#include "synch.h"
#include <stdio.h>

//----------------------------------------------------------------------
// UpdatePC : Increments the Program Counter register in order to resume
// the user program immediately after the "syscall" instruction.
//----------------------------------------------------------------------
static void
UpdatePC ()
{
    int pc = machine->ReadRegister (PCReg);
    machine->WriteRegister (PrevPCReg, pc);
    pc = machine->ReadRegister (NextPCReg);
    machine->WriteRegister (PCReg, pc);
    pc += 4;
    machine->WriteRegister (NextPCReg, pc);
}

//----------------------------------------------------------------------
// ExceptionHandler
//      Entry point into the Nachos kernel.  Called when a user program
//      is executing, and either does a syscall, or generates an addressing
//      or arithmetic exception.
//
//      For system calls, the following is the calling convention:
//
//      system call code -- r2
//              arg1 -- r4
//              arg2 -- r5
//              arg3 -- r6
//              arg4 -- r7
//
//      The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//      "which" is the kind of exception.  The list of possible exceptions 
//      are in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler (ExceptionType which)
{
    int type = machine->ReadRegister (2);

	if (which == SyscallException) 
	{
		switch (type) 
		{
			case SC_Exit: 
				{
					do_UserProcessExit() ;
					break;
				}
			case SC_Halt: 
				{
					do_UserProcessHalt() ;
					break;
				}
#ifdef FILESYS
			case SC_CreateFile: 
				{
					// Params.
					int from = machine->ReadRegister(4) ;
					int b = machine->ReadRegister(5) ;
					unsigned size = MAX_STRING_SIZE ; 
					char string[size] ;
					// Execution.
					synchConsole->CopyStringFromMachine(from, string, size) ;
					bool res = fileSystem->CreateFile(string, b) ;
					// Return.
					machine->WriteRegister(2, res) ;
					break;
				}
			case SC_CreateDir: 
				{
					// Params.
					int from = machine->ReadRegister(4) ;
					unsigned size = MAX_STRING_SIZE ; 
					char string[size] ;
					// Execution.
					synchConsole->CopyStringFromMachine(from, string, size) ;
					bool res = fileSystem->CreateDir(string) ;
					// Return.
					machine->WriteRegister(2, res) ;
					break;
				}
			case SC_RemoveFile: 
				{
					// Params.
					int from = machine->ReadRegister(4) ;
					unsigned size = MAX_STRING_SIZE ; 
					char string[size] ;
					// Execution.
					synchConsole->CopyStringFromMachine(from, string, size) ;
					bool res = fileSystem->RemoveFile(string) ;
					// Return.
					machine->WriteRegister(2, res) ;
					break;
				}
			case SC_RemoveDir: 
				{
					// Params.
					int from = machine->ReadRegister(4) ;
					unsigned size = MAX_STRING_SIZE ; 
					char string[size] ;
					// Execution.
					synchConsole->CopyStringFromMachine(from, string, size) ;
					bool res = fileSystem->RemoveDir(string) ;
					// Return.
					machine->WriteRegister(2, res) ;
					break;
				}
			case SC_Open: 
				{
					// Params.
					int from = machine->ReadRegister(4) ;
					unsigned size = MAX_STRING_SIZE ; 
					char string[size] ;
					char mode = machine->ReadRegister(5) ;
					// Execution.
					synchConsole->CopyStringFromMachine(from, string, size) ;
					OpenFileId res = fileSystem->Open(string, mode) ;
					// Return.
					machine->WriteRegister(2, res) ;
					break;
				}
			case SC_Write: 
				{
					// Params.
					int from = machine->ReadRegister(4) ;
					unsigned size = MAX_STRING_SIZE ; 
					char string[size] ;
					int n = machine->ReadRegister(5) ;
					OpenFileId i = machine->ReadRegister(6) ;
					// Execution.
					synchConsole->CopyStringFromMachine(from, string, size) ;
					fileSystem->Read(i, string, n) ;
					break;
				}
			case SC_Read: 
				{
					// Params.
					int from = machine->ReadRegister(4) ;
					unsigned size = MAX_STRING_SIZE ; 
					char string[size] ;
					int n = machine->ReadRegister(5) ;
					OpenFileId i = machine->ReadRegister(6) ;
					// Execution.
					synchConsole->CopyStringFromMachine(from, string, size) ;
					int res = fileSystem->Read(i, string, n) ;
					// Return.
					machine->WriteRegister(2, res) ;
					break;
				}
			case SC_Close: 
				{
					// Params.
					OpenFileId i = machine->ReadRegister(4) ;
					// Execution.
					fileSystem->Close(i) ;
					break;
				}
#endif
			case SC_PutChar: 
				{
					// Params.
					char c = machine->ReadRegister(4) ;
					// Execution.
					synchConsole->SynchPutChar(c) ;
					break;
				}
			case SC_GetChar: 
				{
					// Params + Execution.
					char c = synchConsole->SynchGetChar(false) ;
					// Return.
					machine->WriteRegister(2, c) ;
					break;
				}
			case SC_PutString: 
				{
					// Params.
					int from = machine->ReadRegister(4) ;
					unsigned size = MAX_STRING_SIZE ; 
					char string[size] ;
					// Execution.
					synchConsole->CopyStringFromMachine(from, string, size) ;
					synchConsole->SynchPutString(string) ; 
					break;
				}
			case SC_GetString: 
				{
					// Params.
					int to = machine->ReadRegister(4) ;
					unsigned size = machine->ReadRegister(5) ;
					char string[size] ;
					// Execution.
					synchConsole->SynchGetString(string, size) ; 
					synchConsole->CopyStringToMachine(string, to, size) ;
					break;
				}
			case SC_PutInt: 
				{
					// Params.
					int i = machine->ReadRegister(4) ;
					// Execution.
					synchConsole->SynchPutInt(i) ;
					break;
				}
			case SC_GetInt: 
				{
					// Params.
					int to = machine->ReadRegister(4) ;
					int i ;
					// Execution.
				   	synchConsole->SynchGetInt(&i) ;
					machine->WriteMem(to, sizeof(int), i) ;
					break;
				}
			case SC_Feof: 
				{
					// Execution.
					int res = synchConsole->Feof() ;
					// Return.
					machine->WriteRegister(2, res) ;
					break;
				}
			case SC_ThreadCreate: 
				{
					// Params.
					int f = machine->ReadRegister(4) ; 
					int arg = machine->ReadRegister(5) ; 
					int returnF = machine->ReadRegister(6) ; // Get it thanks to start.S. 
					// Execution.
					int res = do_UserThreadCreate(f, arg, returnF) ; 
					// Return.
					machine->WriteRegister(2, res) ;
					break ;
				}
			case SC_ThreadExit: 
				{
					// Execution.
					do_UserThreadExit() ;
					break ;
				}
			case SC_ThreadJoin:
				{
					// Params.
					int t = machine->ReadRegister(4) ;
					// Execution.
					int res = do_UserThreadJoin(t) ;
					// Return.
					machine->WriteRegister(2, res) ;
					break ;
				}
			case SC_ThreadId:
				{
					// Execution.
					int res = do_UserThreadId() ;
					// Return.
					machine->WriteRegister(2, res) ;
					break ;
				}
			case SC_ForkExec:
				{
					// Params.
					int a = machine->ReadRegister(4) ;
					char exec[MAX_EXEC_NAME_LEN] ;
					// Execution.
					synchConsole->CopyStringFromMachine(a, exec, MAX_EXEC_NAME_LEN) ;
					int res = do_UserProcessCreate(exec) ;
					// Return.
					machine->WriteRegister(2, res) ;
					break ;
				}
			case SC_WaitPid:
				{
					
					int a = machine->ReadRegister(4);
					if(a > 64 || a < 0 || a == currentThread->space->pid){
						machine->WriteRegister(2,-1);
					}
					do_WaitPid(a);	
					machine->WriteRegister(2,1);			
					break;
				}
			case SC_GetPid:
				{
					//We return the value of the pid of the addrSpace of the currentThread
					machine->WriteRegister(2,currentThread->space->pid);			
					break;
				}
			case SC_SemCreate:
				{
					// Params.
					int val = machine->ReadRegister(4) ;
					int i, res = -1 ; // -> -1 if error.
					// Execution.
					//we find the first non initialized kernel semaphore
					for (i = 0 ; i < 64 ; i ++)
					{
						if (! UserSemaphores[i])
						{
							res = i ;
							UserSemaphores[i] = new Semaphore("User semaphore", val) ;
							break ;
						}
					}
					// Return.
					machine->WriteRegister(2, res) ;
					break ;
				}
			case SC_SemPost:
				{
					// Params. sid is the semaphore identifier known by the user thread
					int sid = machine->ReadRegister(4);
					// Execution.
					//If sid is not a valid value
					if(sid <0 || sid>64 || UserSemaphores[sid]==NULL){
						machine->WriteRegister(2,-1);
						break;
					}
					//
					UserSemaphores[sid]->V();
					machine->WriteRegister(2,1);
					break ;
				}
			case SC_SemWait:
				{
					// Params.
					int sid = machine->ReadRegister(4);
					//If sid is not a valid value
					if(sid <0 || sid>64 || UserSemaphores[sid]==NULL){
						machine->WriteRegister(2,-1);
						break;
					}
					//
					// Execution.
					UserSemaphores[sid]->P();
					machine->WriteRegister(2,1);
					break;
				}
			case SC_SemDelete:
				{
					// Params.
					int sid = machine->ReadRegister(4);
					// Execution.
					delete UserSemaphores[sid];					
					UserSemaphores[sid] = NULL ; // Force.
					break;
				}
			default:	
				{
					printf("Unexpected user mode exception %d | %d\n", which, type);
					ASSERT(FALSE);
				}
		}
	}
	// LB: Do not forget to increment the pc before returning!
	UpdatePC();
}
