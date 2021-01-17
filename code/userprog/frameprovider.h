#ifndef FRAMEPROVIDER_H
#define FRAMEPROVIDER_H


#include "bitmap.h"
#include "synch.h"


class FrameProvider 
{
	public :

		FrameProvider(int nFrames) ;
		~FrameProvider() ;

		int GetEmptyFrame() ;
		void ReleaseFrame(int nFrame) ;
		int NumAvailFrame() ;
		bool IsFrameAvail() ;

	private :

		int totalFrames ;
		BitMap *bitmap ;
		Lock *bitmapLock ;  
} ;


#endif
