// addrspace.h 
//      Data structures to keep track of executing user programs 
//      (address spaces).
//
//      For now, we don't keep any information about address spaces.
//      The user level CPU state is saved and restored in the thread
//      executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"
#include "translate.h"

#define UserStackSize		1024	// increase this as necessary!
#define MAX_USER_THREADS    16

class Lock ;
class Condition ;

class AddrSpace
{
  public:
    AddrSpace (OpenFile * executable);	// Create an address space,
    // initializing it with the program
    // stored in the file "executable"
    ~AddrSpace ();		// De-allocate an address space

    void InitRegisters ();	// Initialize user-level CPU registers,
    // before jumping to user code

    void SaveState ();		// Save/restore address space-specific
    void RestoreState ();	// info on a context switch 

	// Manage the "Create"/"Join"/"Exit"/"Halt" functions.
	void ExitCondWait() ;
	void ExitCondBroadcast() ;
	void ThreadLockRelease() ;
	void ThreadLockAcquire() ;
	// Manage the thread IDs.
	int GetTotalThreads() ;
	void AddThreadID(unsigned int ID) ;
	void RemoveThreadID(unsigned int ID) ;
	bool ContainThreadID(unsigned int ID) ;
	unsigned int GetNextThreadID() ;
	void InitThreadConditions() ;
	void DeleteThreadConditions() ;
	void GetThreadConditionWait(unsigned int ID) ;
	void GetThreadConditionBroadcast(unsigned int ID) ;

  private:


	TranslationEntry * pageTable;	// Assume linear page table translation
    // for now!
    unsigned int numPages;	// Number of pages in the virtual 
    // address space
	
	// To access/signal when a thread exited. 
	Lock	  *threadLock ; 
	Condition *exitCond ; 
	// Current threads "living" in this addr space.
	unsigned int threadIDs[MAX_USER_THREADS] ; 
	Condition *threadConditions[MAX_USER_THREADS] ;
	unsigned int nbThreads ;
	unsigned int maxTIDGiven ;
};

#endif // ADDRSPACE_H
