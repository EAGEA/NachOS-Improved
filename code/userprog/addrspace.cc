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
#include "synch.h"

#include <strings.h>		/* for bzero */


/** Can't be declared as a private/public member of the class otherwise
 * include errors with synch.h ... **/
// To access/signal when 0 threads are running into this address space.
Semaphore *exitLock ; 
Semaphore *exitCond ; 
// To access/signal when a thread exited. 
Lock *joinLock ; 
Condition *joinCond ; 


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

    DEBUG ('a', "Initializing address space, num pages %d, size %d\n",
	   numPages, size);
// first, set up the translation 
    pageTable = new TranslationEntry[numPages];
    for (i = 0; i < numPages; i++)
      {
	  pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
	  pageTable[i].physicalPage = i;
	  pageTable[i].valid = TRUE;
	  pageTable[i].use = FALSE;
	  pageTable[i].dirty = FALSE;
	  pageTable[i].readOnly = FALSE;	// if the code segment was entirely on 
	  // a separate page, we could set its 
	  // pages to be read-only
      }

// zero out the entire address space, to zero the unitialized data segment 
// and the stack segment
    bzero (machine->mainMemory, size);

// then, copy in the code and data segments into memory
    if (noffH.code.size > 0)
      {
	  DEBUG ('a', "Initializing code segment, at 0x%x, size %d\n",
		 noffH.code.virtualAddr, noffH.code.size);
	  executable->ReadAt (&(machine->mainMemory[noffH.code.virtualAddr]),
			      noffH.code.size, noffH.code.inFileAddr);
      }
    if (noffH.initData.size > 0)
      {
	  DEBUG ('a', "Initializing data segment, at 0x%x, size %d\n",
		 noffH.initData.virtualAddr, noffH.initData.size);
	  executable->ReadAt (&
			      (machine->mainMemory
			       [noffH.initData.virtualAddr]),
			      noffH.initData.size, noffH.initData.inFileAddr);
      }

	// Init the total of threads created (count the main one).
	totalThreads = 1 ; 
	// And the synchronization mechanisms.
	exitLock = new Semaphore("AddrSpace threads exit lock", 1) ;
	exitCond = new Semaphore("AddrSpace threads exit cond", 0) ;
	joinLock = new Lock("AddrSpace threads join lock") ;
	joinCond = new Condition("AddrSpace threads join cond") ;
	// Already contains the main thread. 
	i_threadIDs = 1 ;
	threadIDs[0] = 1 ;
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
//      Deallocate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace ()
{
	// LB: Missing [] for delete
	// delete pageTable;
	delete [] pageTable;
	// And the synchronization mechanisms.
	delete exitLock ;
	delete joinLock ;
	delete exitCond ;
	delete joinCond ;
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
//
//      For now, nothing!
//----------------------------------------------------------------------

void
AddrSpace::SaveState ()
{
}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
//      On a context switch, restore the machine state so that
//      this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void
AddrSpace::RestoreState ()
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}


//----------------------------------------------------------------------
// Functions to manage the user threads. 
//		The one which access the "totalThreads" must be protected
//		by the lock.
//----------------------------------------------------------------------


void AddrSpace::JoinCondWait()
{
	joinCond->Wait(joinLock) ;
}

void AddrSpace::JoinCondSignal()
{
	joinCond->Broadcast(joinLock) ;
}

void AddrSpace::ExitCondWait()
{
	exitCond->P() ;
}

void AddrSpace::ExitCondSignal()
{
	exitCond->V() ;
}

void AddrSpace::JoinLockAcquire()
{
	joinLock->Acquire() ;
}

void AddrSpace::JoinLockRelease()
{
	joinLock->Release() ;
}

void AddrSpace::ExitLockAcquire()
{
	exitLock->P() ;
}

void AddrSpace::ExitLockRelease()
{
	exitLock->V() ;
}

void AddrSpace::SetTotalThreads(unsigned int val)
{
	totalThreads = val ;

	if (totalThreads == 1)
	{
		// Notify the "Halt"/"Exit" function if only the main thread is left.
		ExitCondSignal() ;
	}
}

int AddrSpace::GetTotalThreads()
{
	return totalThreads ;
}

void AddrSpace::AddThreadID(unsigned int ID)
{
	unsigned int i, j ; 

	for (i = 0 ; i < i_threadIDs ; i ++)
	{
		if (threadIDs[i] > ID)
		{
			for (j = i_threadIDs - 1 ; j >= i ; j --)
			{
				// Shift the others.
				threadIDs[j + 1] = threadIDs[j] ;
			}

			break ;
		}
		else if (threadIDs[i] == ID)
		{
			// Already contain the ID.
			DEBUG ('t', "Already contain user thread ID \"%d\" at position %d\n", ID, i) ;
			return ;
		}
	}

	threadIDs[i] = ID ;
	i_threadIDs ++ ;

	DEBUG ('t', "Add user thread ID \"%d\" at position %d\n", ID, i) ;
}

void AddrSpace::RemoveThreadID(unsigned int ID)
{
	unsigned int i, j ; 

	for (i = 0 ; i < i_threadIDs ; i ++)
	{
		if (threadIDs[i] == ID)
		{
			for (j = i ; j < i_threadIDs - 1 ; j ++)
			{
				// Shift the others.
				threadIDs[j] = threadIDs[j + 1] ;
			}

			i_threadIDs -- ;

			// Notify the "Join" function that a thread exited.
			JoinCondSignal() ;

			DEBUG ('t', "Remove user thread ID \"%d\" at position %d\n", ID, i) ;
			return ;
		}
		else if (threadIDs[i] > ID)
		{
			// Doesn't contain the ID.
			DEBUG ('t', "Can't remove user thread ID \"%d\"\n", ID) ;
			return ;
		}
	}
}

bool AddrSpace::ContainThreadID(unsigned int ID)
{
	unsigned int i ;

	for (i = 0 ; i < i_threadIDs ; i ++)
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
	unsigned int i ;

	for (i = 0 ; i < i_threadIDs ; i ++)
	{
		if (threadIDs[i] != i + 1)
		{
			DEBUG ('t', "Next user thread ID \"%d\"\n", i + 1) ;
			return i + 1 ;
		}
	}

	DEBUG ('t', "Next user thread ID \"%d\"\n", i_threadIDs + 1) ;
	return i_threadIDs + 1 ; 
}
