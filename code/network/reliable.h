//reliable.h

/* reliable.h is for sending message with better reliability.
 * Using the reliable send, the machine will try to send the message MAXREEMISSIONS time with a delay of TEMPO
*/



#include "copyright.h"

#ifndef RELIABLE_H
#define RELIABLE_H

#include "network.h"
#include "synchlist.h"

#define TEMPO 2

#define MAXREEMISSIONS 5

/*
* Package of data to send to the ackno thread
*
*/

class AcknoData{
	public:
		AcknoData(PostOffice *post,int b,PacketHeader *pktH, MailHeader *mailH) ;
		PostOffice *postOffice;
		int box;
		PacketHeader *pktHdr;
        MailHeader *mailHdr;	
} ;

/*
* Function to send a message with better reliability
*/

void 
ReliableSend(PostOffice *postOffice, PacketHeader ouPktHdr, MailHeader ouMailHdr, const char* data, 
	int box, PacketHeader *inPktHdr, MailHeader *inMailHdr, char* buffer) ;

#endif
