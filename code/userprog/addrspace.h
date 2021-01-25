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


#define UserStackSize				1024 * 3
#define UserThreadStackSize  		PageSize * 3 	    
#define UserThreadMax 	 		    UserStackSize / UserThreadStackSize  


class AddrSpace
{
	public:

		// Create an address space, initializing it with the program
		// stored in the file "executable".
		AddrSpace (OpenFile * executable);	
		~AddrSpace ();		

		/* General */
		// Initialize user-level CPU registers, before jumping to user code
		void InitRegisters ();	
		// Save/restore address space-specific
		void SaveState ();		
		// info on a context switch 
		void RestoreState ();	

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
		bool IsCreated() ;

	private:



		/* Threads. */
		// To protect struct access.
		Lock	  *threadIDLock ; 
		Lock	  *threadStackLock ; 
		Condition *threadExitCond ; 
		// Current threads "living" in this addr space.
		unsigned int threadIDs[UserThreadMax] ; 
		Condition *threadConditions[UserThreadMax] ;
		bool threadStackPointer[UserThreadMax] ;
		unsigned int nbThreads ;
		unsigned int maxTIDGiven ;

		/* Virtual memory. */
		// True if the addrspace was created (i.e. enough frames).
		bool isCreated ;
		// Number of pages in the virtual addr space. 
		unsigned int numPages ; 	
		// Translation table.
		TranslationEntry *pageTable ;	
} ;

#endif // ADDRSPACE_H
