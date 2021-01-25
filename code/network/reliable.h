#include "copyright.h"

#ifndef RELIABLE_H
#define RELIABLE_H

#include "network.h"
#include "synchlist.h"

#define TEMPO 2

#define MAXREEMISSIONS 5


class AcknoData{
	public:
		AcknoData(PostOffice *post,int b,PacketHeader *pktH, MailHeader *mailH) ;
		PostOffice *postOffice;
		int box;
		PacketHeader *pktHdr;
        MailHeader *mailHdr;	
} ;

void 
ReliableSend(PostOffice *postOffice, PacketHeader ouPktHdr, MailHeader ouMailHdr, const char* data, 
	int box, PacketHeader *inPktHdr, MailHeader *inMailHdr, char* buffer) ;

#endif
