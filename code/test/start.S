/* Start.s 
 *	Assembly language assist for user programs running on top of Nachos.
 *
 *	Since we don't want to pull in the entire C library, we define
*	what we need for a user program here, namely Start and the system
*	calls.
 */

#include "syscall.h"

        .text   
        .align  2

/* -------------------------------------------------------------
 * __start
 *	Initialize running a C program, by calling "main". 
 *
 * 	NOTE: This has to be first, so that it gets loaded at location 0.
 *	The Nachos kernel always starts a program by jumping to location 0.
 * -------------------------------------------------------------
 */

	.globl __start
	.ent	__start
__start:
	jal	main
	move	$4,$0		
	jal	Exit	 /* if we return from main, exit(0) */
	.end __start

/* -------------------------------------------------------------
 * System call stubs:
 *	Assembly language assist to make system calls to the Nachos kernel.
 *	There is one stub per system call, that places the code for the
 *	system call into register r2, and leaves the arguments to the
 *	system call alone (in other words, arg1 is in r4, arg2 is 
 *	in r5, arg3 is in r6, arg4 is in r7)
 *
 * 	The return value is in r2. This follows the standard C calling
 * 	convention on the MIPS.
 * -------------------------------------------------------------
 */

	.globl Halt
	.ent	Halt
Halt:
	addiu $2,$0,SC_Halt
	syscall
	j	$31
	.end Halt

	.globl Exit
	.ent	Exit
Exit:
	addiu $2,$0,SC_Exit
	syscall
	j	$31
	.end Exit

	.globl Exec
	.ent	Exec
Exec:
	addiu $2,$0,SC_Exec
	syscall
	j	$31
	.end Exec

	.globl Join
	.ent	Join
Join:
	addiu $2,$0,SC_Join
	syscall
	j	$31
	.end Join

	.globl CreateFile
	.ent	CreateFile
CreateFile:
	addiu $2,$0,SC_CreateFile
	syscall
	j	$31
	.end CreateFile

	.globl CreateDir
	.ent	CreateDir
CreateDir:
	addiu $2,$0,SC_CreateDir
	syscall
	j	$31
	.end CreateDir

	.globl RemoveFile
	.ent	RemoveFile
RemoveFile:
	addiu $2,$0,SC_RemoveFile
	syscall
	j	$31
	.end RemoveFile

	.globl RemoveDir
	.ent	RemoveDir
RemoveDir:
	addiu $2,$0,SC_RemoveDir
	syscall
	j	$31
	.end RemoveDir

	.globl List
	.ent	List
List:
	addiu $2,$0,SC_List
	syscall
	j	$31
	.end List

	.globl Open
	.ent	Open
Open:
	addiu $2,$0,SC_Open
	syscall
	j	$31
	.end Open

	.globl Read
	.ent	Read
Read:
	addiu $2,$0,SC_Read
	syscall
	j	$31
	.end Read

	.globl Write
	.ent	Write
Write:
	addiu $2,$0,SC_Write
	syscall
	j	$31
	.end Write

	.globl Close
	.ent	Close
Close:
	addiu $2,$0,SC_Close
	syscall
	j	$31
	.end Close

	.globl Fork
	.ent	Fork
Fork:
	addiu $2,$0,SC_Fork
	syscall
	j	$31
	.end Fork

	.globl Yield
	.ent	Yield
Yield:
	addiu $2,$0,SC_Yield
	syscall
	j	$31
	.end Yield

	.globl PutChar
	.ent	PutChar
PutChar:
	addiu $2,$0,SC_PutChar
	syscall
	j	$31
	.end PutChar

	.globl GetChar
	.ent	GetChar
GetChar:
	addiu $2,$0,SC_GetChar
	syscall
	j	$31
	.end GetChar

	.globl PutString
	.ent	PutString
PutString:
	addiu $2,$0,SC_PutString
	syscall
	j	$31
	.end PutString

	.globl GetString
	.ent	GetString
GetString:
	addiu $2,$0,SC_GetString
	syscall
	j	$31
	.end GetString

	.globl PutInt
	.ent	PutInt
PutInt:
	addiu $2,$0,SC_PutInt
	syscall
	j	$31
	.end PutInt

	.globl GetInt
	.ent	GetInt
GetInt:
	addiu $2,$0,SC_GetInt
	syscall
	j	$31
	.end GetInt

	.globl Feof
	.ent	Feof
Feof:
	addiu $2,$0,SC_Feof
	syscall
	j	$31
	.end Feof

	.globl UserThreadCreate
	.ent	UserThreadCreate
UserThreadCreate:
	addiu $2,$0,SC_ThreadCreate
	addiu $6,$0,UserThreadExit // Auto-parameter (default).
	syscall
	j	$31
	.end UserThreadCreate

	.globl UserThreadExit
	.ent	UserThreadExit
UserThreadExit:
	addiu $2,$0,SC_ThreadExit
	syscall
	j	$31
	.end UserThreadExit

	.globl UserThreadJoin
	.ent	UserThreadJoin
UserThreadJoin:
	addiu $2,$0,SC_ThreadJoin
	syscall
	j	$31
	.end UserThreadJoin

	.globl UserThreadId
	.ent   UserThreadId
UserThreadId:
	addiu $2,$0,SC_ThreadId
	syscall
	j   $31
	.end UserThreadId

	.globl ForkExec
	.ent   ForkExec
ForkExec:
	addiu $2,$0,SC_ForkExec
	syscall
	j   $31
	.end ForkExec
	

	.globl SemCreate
	.ent   SemCreate
SemCreate:
	addiu $2,$0,SC_SemCreate
	syscall
	j   $31
	.end SemCreate
	
	.globl SemPost
	.ent   SemPost
SemPost:
	addiu $2,$0,SC_SemPost
	syscall
	j   $31
	.end SemPost
	
	.globl SemWait
	.ent   SemWait
SemWait:
	addiu $2,$0,SC_SemWait
	syscall
	j   $31
	.end SemWait

	.globl SemDelete
	.ent   SemDelete
SemDelete:
	addiu $2,$0,SC_SemDelete
	syscall
	j   $31
	.end SemDelete
	
	.globl WaitPid
	.ent   WaitPid
WaitPid:
	addiu $2,$0,SC_WaitPid
	syscall
	j   $31
	.end WaitPid
	
	.globl GetPid
	.ent   GetPid
GetPid:
	addiu $2,$0,SC_GetPid
	syscall
	j   $31
	.end GetPid


/* dummy function to keep gcc happy */
        .globl  __main
        .ent    __main
__main:
        j       $31
        .end    __main

