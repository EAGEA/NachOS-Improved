#include "copyright.h"
#include "system.h"
#include "threadparams.h"

/** 
 * Params of the thread for the "Start" function. 
 */

ThreadParams::ThreadParams(int f, int a, int rF, bool setSpace)
{
	fun = f ;
	arg = a ;
	returnFun = rF ;
	needsToSetSpace = setSpace ;
}

int ThreadParams::GetFun()
{
	return fun ;
}

int ThreadParams::GetReturnFun()
{
	return returnFun ;
}

int ThreadParams::GetArg()
{
	return arg ;
}

bool ThreadParams::NeedsToSetSpace()
{
	return needsToSetSpace ;
}
