// system.h 
//      All global variables used in Nachos are defined here.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef SYSTEM_H
#define SYSTEM_H

#include "copyright.h"
#include "utility.h"
#include "thread.h"
#include "scheduler.h"
#include "interrupt.h"
#include "stats.h"
#include "timer.h"
#include "../userprog/allocate.h"


/* A unique identifier for an open Nachos file. */
typedef int OpenFileId;


// Initialization and cleanup routines
extern void Initialize (int argc, char **argv);	// Initialization,
						// called before anything else
extern void Cleanup ();		// Cleanup, called when
						// Nachos is done.

extern Thread *currentThread;	// the thread holding the CPU
extern Thread *threadToBeDestroyed;	// the thread that just finished
extern Scheduler *scheduler;	// the ready list
extern Interrupt *interrupt;	// interrupt status
extern Statistics *stats;	// performance metrics
extern Timer *timer;		// the hardware alarm clock

#ifdef USER_PROGRAM
#include "machine.h"
#include "synchconsole.h"
#include "frameprovider.h"
#include "synch.h"
extern Machine *machine;	// user program memory and registers
extern SynchConsole *synchConsole ;
extern FrameProvider *fprovider;
extern Semaphore* UserSemaphores[64];
extern Lock* ProcessLocks[64];
extern Condition* ProcessConds[64];
extern int pids[64];

#define MAX_EXEC_NAME_LEN 100

#endif

#ifdef FILESYS_NEEDED		// FILESYS or FILESYS_STUB
#include "filesys.h"
#include "synchdisk.h"
extern FileSystem *fileSystem;
extern SynchDisk *synchDisk;
#endif

#ifdef NETWORK
#include "post.h"
extern PostOffice *postOffice;
#endif

#endif // SYSTEM_H
