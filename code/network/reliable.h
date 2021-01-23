#include "copyright.h"

#ifndef RELIABLE_H
#define RELIABLE_H

#include "network.h"
#include "synchlist.h"

#define TEMPO 3

#define MAXREEMISSIONS 10

void ReliableSend(PostOffice *postOffice, PacketHeader ouPktHdr, MailHeader ouMailHdr, const char* data, 
	int box, PacketHeader *inPktHdr, MailHeader *inMailHdr, char* buffer);

#endif