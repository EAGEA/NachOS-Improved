/* syscalls.h 
 * 	Nachos system call interface.  These are Nachos kernel operations
 * 	that can be invoked from user programs, by trapping to the kernel
 *	via the "syscall" instruction.
 *
 *	This file is included by user programs and by the Nachos kernel. 
 *
 * Copyright (c) 1992-1993 The Regents of the University of California.
 * All rights reserved.  See copyright.h for copyright notice and limitation 
 * of liability and disclaimer of warranty provisions.
 */

#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "copyright.h"

/* system call codes -- used by the stubs to tell the kernel which system call
 * is being asked for
 */
#define SC_Halt				0
#define SC_Exit				1
#define SC_Exec				2
#define SC_Join				3
#define SC_CreateFile		4
#define SC_CreateDir		5
#define SC_RemoveFile		6
#define SC_RemoveDir		7
#define SC_List		  		8
#define SC_Open				9
#define SC_Read				10
#define SC_Write			11	
#define SC_Close			12
#define SC_Fork				13
#define SC_Yield			14
#define SC_PutChar			15
#define SC_GetChar			16
#define SC_PutString		17
#define SC_GetString		18
#define SC_PutInt			19
#define SC_GetInt			20
#define SC_Feof			    21
#define SC_ThreadCreate		22
#define SC_ThreadExit		23
#define SC_ThreadJoin       24
#define SC_ThreadId         25
#define SC_ForkExec         26
#define SC_SemCreate		27
#define SC_SemPost			28
#define SC_SemWait			29
#define SC_SemDelete		30
#define SC_WaitPid			31
#define SC_GetPid			32

#ifdef IN_USER_MODE

// LB: This part is read only on compiling the test/*.c files.
// It is *not* read on compiling test/start.S


/* The system call interface.  These are the operations the Nachos
 * kernel needs to support, to be able to run user programs.
 *
 * Each of these is invoked by a user program by simply calling the 
 * procedure; an assembly language stub stuffs the system call code
 * into a register, and traps to the kernel.  The kernel procedures
 * are then invoked in the Nachos kernel, after appropriate error checking, 
 * from the system call entry point in exception.cc.
 */

/* Stop Nachos, and print out performance stats */
void Halt () __attribute__((noreturn));


/* Address space control operations: Exit, Exec, and Join */

/* This user program is done (status = 0 means exited normally). */
void Exit (int status) __attribute__((noreturn));

/* A unique identifier for an executing user program (address space) */
typedef int SpaceId;

/* Run the executable, stored in the Nachos file "name", and return the 
 * address space identifier
 */
SpaceId Exec (char *name);

/* Only return once the the user program "id" has finished.  
 * Return the exit status.
 */
int Join (SpaceId id);


/* File system operations: Create, Open, Read, Write, Close
 * These functions are patterned after UNIX -- files represent
 * both files *and* hardware I/O devices.
 *
 * If this assignment is done before doing the file system assignment,
 * note that the Nachos file system has a stub implementation, which
 * will work for the purposes of testing out these routines.
 */

/* A unique identifier for an open Nachos file. */
typedef int OpenFileId;

/* when an address space starts up, it has two open files, representing 
 * keyboard input and display output (in UNIX terms, stdin and stdout).
 * Read and Write can be used directly on these, without first opening
 * the console device.
 */

#define ConsoleInput	0
#define ConsoleOutput	1

/* Create a Nachos file, with "name" */
_Bool CreateFile (char *name, int bytes) ;

/* Create a Nachos directory, with "name" */
_Bool CreateDir (char *name) ; 

/* Remove a Nachos file, with "name" */
_Bool RemoveFile (char *name) ;

/* Remove a Nachos directory, with "name" */
_Bool RemoveDir (char *name) ; 

/* Print the current directory content */
void List () ; 

/* Open the Nachos file "name", and return an "OpenFileId" that can 
 * be used to read and write to the file.
 */
OpenFileId Open (char *name, char mode);

/* Write "size" bytes from "buffer" to the open file. */
void Write (char *buffer, int size, OpenFileId id);

/* Read "size" bytes from the open file into "buffer".  
 * Return the number of bytes actually read -- if the open file isn't
 * long enough, or if it is an I/O device, and there aren't enough 
 * characters to read, return whatever is available (for I/O devices, 
 * you should always wait until you can return at least one character).
 */
int Read (char *buffer, int size, OpenFileId id);

/* Close the file, we're done reading and writing to it. */
void Close (OpenFileId id);


/* User-level thread operations: Fork and Yield.  To allow multiple
 * threads to run within a user program. 
 */

/* Fork a thread to run a procedure ("func") in the *same* address space 
 * as the current thread.
 */
void Fork (void (*func) ());

/* Yield the CPU to another runnable thread, whether in this address space 
 * or not. 
 */
void Yield ();

/* Output a character to a stream.
 */
void PutChar(char c) ;

/* Get a character from the input buffer. 
 */
char GetChar() ;

/* Output a sequence of characters to a stream. There is no limit on the size of 
 * the string s.
 */
void PutString(const char s[]) ;

/* Get a sequence of characters from the input buffer.
 * There is a limit on the size of the string s which is : MAX_STRING_SIZE ( 4096 )
 * MAX_STRING_SIZE is defined in the file "threads/system.h".
 */
void GetString(char *s, int n) ;

/* Output an int to a stream. 
 */
void PutInt(int i) ;

/* Get a int from the input buffer.
 */
void GetInt(int *i) ;

/* Return true if EOF was reached. 
 */
int Feof() ;

/* Create a user level thread. 
 */
int UserThreadCreate(void f(void *arg), void *arg) ;

/* Destroy a user lvel thread.
 */
void UserThreadExit() ;

/* Wait for the user thread to finish.
 */
int UserThreadJoin(int t) ;

/* Return the Thread Id of the current thread (caller).
 */
int UserThreadId() ;

/* Launch the executable "s" concurrently. 
 */
int ForkExec(char *s) ;

/* Waits for process pid to finish*/

int WaitPid(int pid);

/* Return the pid of the current process */

int GetPid();

/*Creates a semaphore*/

int SemCreate(int val);

/*Semaphore->V()*/

int SemPost(int sid);

/*Semaphore->P()*/

int SemWait(int sid);

/*Deletes semaphore with id sid*/

void SemDelete(int sid);


#endif // IN_USER_MODE

#endif /* SYSCALL_H */
