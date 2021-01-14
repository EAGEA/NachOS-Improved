#ifndef USERTHREAD_H
#define USERTHREAD_H

int do_UserThreadCreate(int fun, int arg, int returnFun) ;

void do_UserThreadExit() ;

void do_UserThreadJoin(int t) ;

int do_UserThreadId() ;

// Class to pass the arguments to the "Fork" function.
class ThreadParams
{
	private :

		int fun ;
		int returnFun ;
		int arg ;

	public :
		
		ThreadParams(int fun, int arg, int returnFun) ;

		int GetFun() ;
		int GetReturnFun() ;
		int GetArg() ;
} ;

#endif
