#include "system.h"
#include "frameprovider.h"
#include "bitmap.h"
#include "synch.h"
#include "time.h"
#include <stdio.h>

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
	bool isFull = true ;

	bitmapLock->Acquire() ;

	// Random allocation:
	unsigned int emptyFrames[totalFrames] ;
	// Get the "free" frames.
	for (i = 0 ; i < totalFrames ; i ++)
	{
		if (! bitmap->Test(i))
		{
			emptyFrames[j ++] = i ;
			isFull = false ;
		}
	}
	// Check if the frame provider is not full.
	if (isFull)
	{
		return -1 ;
	}
	// Otherwise get a random frame among them.
	nFrame = emptyFrames[rand() % j] ;	
	//printf("%d\n", nFrame);
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

unsigned int FrameProvider::NumAvailFrame() 
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
