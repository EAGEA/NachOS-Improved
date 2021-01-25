// addrspace.cc 
//      Routines to manage address spaces (executing user programs).
//
//      In order to run a user program, you must:
//
//      1. link with the -N -T 0 option 
//      2. run coff2noff to convert the object file to Nachos format
//              (Nachos object code format is essentially just a simpler
//              version of the UNIX executable object code format)
//      3. load the NOFF file into the Nachos file system
//              (if you haven't implemented the file system yet, you
//              don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"

#include <strings.h>		/* for bzero */
#include <stdio.h>

static void ReadAtVirtual(OpenFile *executable, int virtualaddr, 
		int numBytes, int position, 
		TranslationEntry *pageTable, unsigned numPages) ; 

//----------------------------------------------------------------------
// SwapHeader
//      Do little endian to big endian conversion on the bytes in the 
//      object file header, in case the file was generated on a little
//      endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

	static void
SwapHeader (NoffHeader * noffH)
{
	noffH->noffMagic = WordToHost (noffH->noffMagic);
	noffH->code.size = WordToHost (noffH->code.size);
	noffH->code.virtualAddr = WordToHost (noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost (noffH->code.inFileAddr);
	noffH->initData.size = WordToHost (noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost (noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost (noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost (noffH->uninitData.size);
	noffH->uninitData.virtualAddr =
		WordToHost (noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost (noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
//      Create an address space to run a user program.
//      Load the program from a file "executable", and set everything
//      up so that we can start executing user instructions.
//
//      Assumes that the object code file is in NOFF format.
//
//      First, set up the translation from program memory to physical 
//      memory.  For now, this is really simple (1:1), since we are
//      only uniprogramming, and we have a single unsegmented page table
//
//      "executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace (OpenFile * executable)
{
	NoffHeader noffH;
	unsigned int i, size;

	executable->ReadAt ((char *) &noffH, sizeof (noffH), 0);
	if ((noffH.noffMagic != NOFFMAGIC) &&
			(WordToHost (noffH.noffMagic) == NOFFMAGIC))
		SwapHeader (&noffH);
	ASSERT (noffH.noffMagic == NOFFMAGIC);

	// how big is address space?
	size = noffH.code.size + noffH.initData.size + noffH.uninitData.size + UserStackSize;	// we need to increase the size
	// to leave room for the stack
	numPages = divRoundUp (size, PageSize);
	size = numPages * PageSize;

	ASSERT (numPages <= NumPhysPages);	// check we're not trying
	// to run anything too big --
	// at least until we have
	// virtual memory
	//printf("Num pages %d\n",numPages);
	//printf("NumPhysPages %d\n",NumPhysPages);
	DEBUG ('a', "Initializing address space, num pages %d, size %d\n",
			numPages, size);
	// First, set up the translation 
	pageTable = new TranslationEntry[numPages];
	// Check if enough frames are available to make the translation.
	if (fprovider->NumAvailFrame() < numPages)
	{
		// The address space can't be correctly created.
		isCreated = false ;
		return ; 
	}
	// And the page to frame translation.
	for (i = 0 ; i < numPages ; i ++)
	{
		if (fprovider->IsFrameAvail()) 
		{
			// Get a map on a physical frame.
			pageTable[i].physicalPage = fprovider->GetEmptyFrame() ;

			DEBUG ('a', "Address space %d frame(s) left\n", fprovider->NumAvailFrame());
		} 
		else 
		{
			// Error, no frame available despite the previous check.
			ASSERT(FALSE) ;
			return ;
		} 
		pageTable[i].virtualPage = i;
		pageTable[i].valid = TRUE;
		pageTable[i].use = FALSE;
		pageTable[i].dirty = FALSE;
		pageTable[i].readOnly = FALSE;	// if the code segment was entirely on 
		// a separate page, we could set its 
		// pages to be read-only
	}

	// then, copy in the code and data segments into memory
	if (noffH.code.size > 0)
	{
		DEBUG ('a', "Initializing code segment, at 0x%x, size %d\n",
				noffH.code.virtualAddr, noffH.code.size);
		ReadAtVirtual(executable, noffH.code.virtualAddr, noffH.code.size, 
				noffH.code.inFileAddr, pageTable, numPages) ;
	}
	if (noffH.initData.size > 0)
	{
		DEBUG ('a', "Initializing data segment, at 0x%x, size %d\n",
				noffH.initData.virtualAddr, noffH.initData.size);
		ReadAtVirtual(executable, noffH.initData.virtualAddr, noffH.initData.size, 
				noffH.initData.inFileAddr, pageTable, numPages);
	}

	// Init synchronization mechanisms.
	threadIDLock = new Lock("AddrSpace threads id lock") ;
	threadStackLock = new Lock("AddrSpace threads stack lock") ;
	threadExitCond = new Condition("AddrSpace threads exit cond") ;
	InitThreadJoinConditions() ;
	InitThreadStackPointer() ;
	// Already contains the main thread. 
	nbThreads = 1 ;
	threadIDs[0] = 1 ;
	maxTIDGiven = 1 ;
	// Correctly created.
	isCreated = true ;
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
//      Deallocate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace ()
{
	// Free virtual memory associated frames.
	FreeFrames() ;
	// LB: Missing [] for delete
	// delete pageTable;
	delete [] pageTable;
	// And the synchronization mechanisms.
	delete threadIDLock ;
	delete threadStackLock ;
	delete threadExitCond ;	
	// Delete the synch mechanisms for thread.join.
	DeleteThreadJoinConditions() ;
	// End of modification
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
//      Set the initial values for the user-level register set.
//
//      We write these directly into the "machine" registers, so
//      that we can immediately jump to user code.  Note that these
//      will be saved/restored into the currentThread->userRegisters
//      when this thread is context switched out.
//----------------------------------------------------------------------

	void
AddrSpace::InitRegisters ()
{
	int i;

	for (i = 0; i < NumTotalRegs; i++)
		machine->WriteRegister (i, 0);

	// Initial program counter -- must be location of "Start"
	machine->WriteRegister (PCReg, 0);

	// Need to also tell MIPS where next instruction is, because
	// of branch delay possibility
	machine->WriteRegister (NextPCReg, 4);

	// Set the stack register to the end of the address space, where we
	// allocated the stack; but subtract off a bit, to make sure we don't
	// accidentally reference off the end!
	machine->WriteRegister (StackReg, numPages * PageSize - 16);
	DEBUG ('a', "Initializing stack register to %d\n",
			numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
//      On a context switch, save any machine state, specific
//      to this address space, that needs saving.
//----------------------------------------------------------------------

	void
AddrSpace::SaveState ()
{
	pageTable = machine->pageTable ;
	numPages = machine->pageTableSize ;
}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
//      On a context switch, restore the machine state so that
//      this address space can run.
//----------------------------------------------------------------------

	void
AddrSpace::RestoreState ()
{
	machine->pageTable = pageTable;
	machine->pageTableSize = numPages;
}


//----------------------------------------------------------------------
// Functions to manage the user threads. 
//		Every access to a "get" or "set" function must be protected with
//		the following locks.
//----------------------------------------------------------------------


/* Thread general.
*/


int AddrSpace::GetTotalThreads()
{
	return nbThreads ;
}


/* Thread ID. 
*/


void AddrSpace::ThreadIDLockAcquire()
{
	threadIDLock->Acquire() ;
}

void AddrSpace::ThreadIDLockRelease()
{
	threadIDLock->Release() ;
}

void AddrSpace::AddThreadID(unsigned int ID)
{
	if (ContainThreadID(ID))
	{
		// Already contain the ID.
		DEBUG ('t', "Already contain user thread ID \"%d\"\n", ID) ;
		return ;
	}

	threadIDs[nbThreads] = ID ;
	nbThreads ++ ;

	DEBUG ('t', "Add user thread ID \"%d\" at position %d\n", ID, nbThreads) ;
}

void AddrSpace::RemoveThreadID(unsigned int ID)
{
	unsigned int i, j ; 

	for (i = 0 ; i < nbThreads ; i ++)
	{
		if (threadIDs[i] == ID)
		{
			for (j = i ; j < nbThreads - 1 ; j ++)
			{
				// Shift the others.
				threadIDs[j] = threadIDs[j + 1] ;
			}

			nbThreads -- ;

			// Notify the "Halt"/"Exit"/"Join" function that a thread exited.
			ThreadJoinConditionBroadcast(ID) ;
			ThreadExitConditionBroadcast() ;

			DEBUG ('t', "Remove user thread ID \"%d\" at position %d\n", ID, i) ;
			return ;
		}
	}

	DEBUG ('t', "Can't remove user thread ID \"%d\"\n", ID) ;
}

bool AddrSpace::ContainThreadID(unsigned int ID)
{
	unsigned int i ;

	for (i = 0 ; i < nbThreads ; i ++)
	{
		if (threadIDs[i] == ID)
		{
			DEBUG ('t', "Contain user thread ID \"%d\"\n", ID) ;
			return true ;
		}
	}

	DEBUG ('t', "Doesn't contain user thread ID \"%d\"\n", ID) ;
	return false ; 
}

unsigned int AddrSpace::GetNextThreadID()
{
	return ++ maxTIDGiven ;  
}


/* Thread join.
*/


void AddrSpace::InitThreadJoinConditions()
{
	unsigned int i ;

	for (i = 0 ; i < UserThreadMax ; i ++)
	{
		char *name = (char *) malloc(sizeof(char) * 35) ;
		sprintf(name, "Addrspace thread join condition n°%d", i) ;

		threadConditions[i] = new Condition(name) ;
	}
}

void AddrSpace::DeleteThreadJoinConditions()
{
	unsigned int i ;

	for (i = 0 ; i < UserThreadMax ; i ++)
	{
		delete threadConditions[i] ; 
	}
}

void AddrSpace::ThreadJoinConditionWait(unsigned int ID)
{
	threadConditions[ID]->Wait(threadIDLock) ;
}

void AddrSpace::ThreadJoinConditionBroadcast(unsigned int ID)
{
	threadConditions[ID]->Broadcast(threadIDLock) ;
}


/* Thread Halt / Exit.
*/


void AddrSpace::ThreadExitConditionWait()
{
	threadExitCond->Wait(threadIDLock) ;
}

void AddrSpace::ThreadExitConditionBroadcast()
{
	threadExitCond->Broadcast(threadIDLock) ;
}


/* Thread stack pointer.
*/


void AddrSpace::ThreadStackLockAcquire()
{
	threadStackLock->Acquire() ;
}

void AddrSpace::ThreadStackLockRelease()
{
	threadStackLock->Release() ;
}

void AddrSpace::InitThreadStackPointer()
{
	int i = 0 ;

	for (i = 0 ; i < UserThreadMax ; i ++)
	{
		threadStackPointer[i] = false ;
	}
}

int AddrSpace::GetThreadStackPointer()
{
	int i ;

	for (i = 0 ; i < UserThreadMax ; i ++)	
	{
		if (! threadStackPointer[i])
		{
			threadStackPointer[i] = true ;

			return numPages * PageSize - 16 - ((i + 1) * UserThreadStackSize) ;  
		}
	}
	// Should never be reached.
	ASSERT(FALSE) ;
	return 0 ;
}

void AddrSpace::RemoveThreadStackPointer(int sp)
{
	int i = (UserStackSize - sp) / UserThreadStackSize ; 
	threadStackPointer[i] = false ;
}


//----------------------------------------------------------------------
// Functions to manage the virtual memory. 
//----------------------------------------------------------------------


static void ReadAtVirtual(OpenFile *executable, int virtualaddr, 
		int numBytes, int position, 
		TranslationEntry *pageTable, unsigned numPages) 
{
	int i ;
	// Read physical memory.
	char buf[numBytes] ;
	int nbBytes = executable->ReadAt(buf, numBytes, position) ;
	// Keep the old page table version.
	TranslationEntry *oldTable = machine->pageTable ;
	int oldTableSize = machine->pageTableSize ;
	// Put the page table to the machine to be able to write.
	machine->pageTable = pageTable ;
	machine->pageTableSize = numPages ;
	// Write.
	for (i = 0 ; i < nbBytes ; i ++) 
	{
		machine->WriteMem(virtualaddr + i, 1, buf[i]) ;
	}
	// Restore previous state.
	machine->pageTable = oldTable ;
	machine->pageTableSize = oldTableSize ;
}

void AddrSpace::FreeFrames()
{
	unsigned int i ;

	for (i = 0 ; i < numPages ; i ++) 
	{
		if (pageTable[i].valid) 
		{
			fprovider->ReleaseFrame(pageTable[i].physicalPage) ;
		}
	}
}

void AddrSpace::RestoreFrames()
{
	unsigned int i ;

	for (i = 0 ; i < numPages ; i ++) 
	{
		if (pageTable[i].valid) 
		{
			fprovider->SetFrame(pageTable[i].physicalPage) ;
		}
	}
}

bool AddrSpace::IsCreated()
{
	return isCreated ;
}
