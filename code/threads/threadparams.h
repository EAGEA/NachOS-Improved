#ifndef THREADPARAMS_H
#define THREADPARAMS_H

// Class to pass the arguments to the "Fork" function.
class ThreadParams
{
	private :

		int fun ;
		int returnFun ;
		int arg ;
		bool needsToSetSpace ;

	public :
		
		ThreadParams(int fun, int arg, int returnFun, bool setSpace) ;

		int GetFun() ;
		int GetReturnFun() ;
		int GetArg() ;
		bool NeedsToSetSpace() ;
} ;

#endif
