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
#include "frameprovider.h"

#define UserStackSize				1024 * 2			// increase this as necessary!
#define STACK_SIZE_USER_THREAD		PageSize * 2 	    // increase this as necessary!
#define MAX_USER_THREADS			UserStackSize / STACK_SIZE_USER_THREAD


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

	/* Threads. */
	// Thread IDs.
	void ThreadIDLockRelease() ;
	void ThreadIDLockAcquire() ;
	int GetTotalThreads() ;
	void AddThreadID(unsigned int ID) ;
	void RemoveThreadID(unsigned int ID) ;
	bool ContainThreadID(unsigned int ID) ;
	unsigned int GetNextThreadID() ;
	// Thread Join.
	void InitThreadJoinConditions() ;
	void DeleteThreadJoinConditions() ;
	void ThreadJoinConditionWait(unsigned int ID) ;
	void ThreadJoinConditionBroadcast(unsigned int ID) ;
	// Thread Halt / Exit.
	void ThreadExitConditionWait() ;
	void ThreadExitConditionBroadcast() ;
	// Thread Stack pointer.
	void ThreadStackLockRelease() ;
	void ThreadStackLockAcquire() ;
	void InitThreadStackPointer() ;
	int GetThreadStackPointer() ;
	void RemoveThreadStackPointer(int i) ;

	/* Virtual memory. */
	void FreeFrames() ;
	void RestoreFrames() ;

  private:


	TranslationEntry * pageTable;	// Assume linear page table translation
    // for now!
    unsigned int numPages;	// Number of pages in the virtual 
    // address space
	
	/* Threads. */
	// To protect struct access.
	Lock	  *threadIDLock ; 
	Lock	  *threadStackLock ; 
	Condition *threadExitCond ; 
	// Current threads "living" in this addr space.
	unsigned int threadIDs[MAX_USER_THREADS] ; 
	Condition *threadConditions[MAX_USER_THREADS] ;
	bool threadStackPointer[MAX_USER_THREADS] ;
	unsigned int nbThreads ;
	unsigned int maxTIDGiven ;

	/* Virtual memory. */
	FrameProvider *frameProvider ;
};

#endif // ADDRSPACE_H
