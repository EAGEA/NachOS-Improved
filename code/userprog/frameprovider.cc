#include "system.h"
#include "frameprovider.h"
#include "bitmap.h"
#include "synch.h"
#include "time.h"


FrameProvider::FrameProvider(int nFrames) 
{
	srand(time(NULL)) ;

	bitmap = new BitMap(nFrames) ;
	bitmapLock = new Lock("FrameProvider bitmap lock") ;

	bitmap->Mark(0) ;

	totalFrames = nFrames ;
}

FrameProvider::~FrameProvider() 
{
	delete bitmap ;
	delete bitmapLock ;
}

int FrameProvider::GetEmptyFrame() 
{
	int nFrame ;
	int i, j = 0 ;
	unsigned int emptyFrames[totalFrames] ;

	bitmapLock->Acquire() ;
	// Random allocation:
	// Get the "free" frames.
	for (i = 0 ; i < totalFrames ; i ++)
	{
		if (! bitmap->Test(i))
		{
			emptyFrames[j ++] = i ;
		}
	}
	// Get a random frame among them.
	nFrame = emptyFrames[rand() % j] ;	
	bitmap->Mark(nFrame) ;
	// Set the frame to "zero" ('\0').
	bzero(&(machine->mainMemory[nFrame * PageSize]), PageSize) ;

	bitmapLock->Release() ;

	return nFrame ;
}

void FrameProvider::SetFrame(int nFrame) 
{
	bitmap->Mark(nFrame) ;
}

void FrameProvider::ReleaseFrame(int nFrame) 
{
	bitmap->Clear(nFrame) ;
}

int FrameProvider::NumAvailFrame() 
{
	bitmapLock->Acquire() ;

	int n = bitmap->NumClear() ;

	bitmapLock->Release() ;

	return n ;
}

bool FrameProvider::IsFrameAvail()
{
	return NumAvailFrame() > 0 ;
}
