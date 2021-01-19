#include "allocate.h"

////////////////////////////////////////////////////////////////////////////
//  To show that our processes are freeing memory.                        //
////////////////////////////////////////////////////////////////////////////

void operator delete(void *p)
{
	DEBUG('a', "Deleting %p.\n", p) ;
	free(p) ;
}

void operator delete[](void *p)
{
	DEBUG('a', "Deleting array %p.\n", p) ;
	free(p) ;
}

void *operator new(size_t s)
{
	DEBUG('a', "Allocating %u bytes.\n", s) ;
	return malloc(s) ;
}

void *operator new[](size_t s)
{
	DEBUG('a', "Allocating array of size %u bytes.\n", s) ;
	return malloc(s) ;
}
