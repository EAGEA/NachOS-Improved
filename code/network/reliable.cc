#include "copyright.h"
#include "post.h"
#include "thread.h"
#include "reliable.h"
#include "synchlist.h"

#include <strings.h> /* for bzero */

void
DelayAck(int arg)
{
	PostOffice* postOffice = (PostOffice *) arg;
	Delay(TEMPO) ;
	Lock *lock = postOffice->GetBox(0).GetMessages()->GetLock();
	postOffice->GetBox(0).GetMessages()->GetlistEmpty()->Signal(lock) ;
	printf("SIGNAL !\n");
}

void 
ReliableSend(PostOffice *postOffice, PacketHeader ouPktHdr, MailHeader ouMailHdr, const char* data, 
	int box, PacketHeader *inPktHdr, MailHeader *inMailHdr, char* buffer)
{
	/*Thread *delay = new Thread("ack receiver", 1, 0);

    delay->Fork(DelayAck, (int) postOffice);*/

	//Message to be sent
	postOffice->Send(ouPktHdr,ouMailHdr,data) ;
	printf("Message sended\n");

	// Waiting for the aknowledgment coming from the receiver
	postOffice->Receive(box, inPktHdr, inMailHdr, buffer);
	printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr->from,inMailHdr->from);
	fflush(stdout);
}
