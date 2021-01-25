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
		void SetFrame(int nFrame) ;
		void ReleaseFrame(int nFrame) ;
		unsigned int NumAvailFrame() ;
		bool IsFrameAvail() ;

	private :

		int totalFrames ;
		BitMap *bitmap ;
		Lock *bitmapLock ;  
} ;


#endif
