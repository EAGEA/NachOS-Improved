#include "copyright.h"
#include "post.h"
#include "thread.h"
#include "reliable.h"
#include "synchlist.h"

#include <strings.h> /* for bzero */


AcknoData::AcknoData(PostOffice *post,int b,PacketHeader *pktH, MailHeader *mailH)
{
	postOffice = post ;
	box = b ;
	pktHdr = pktH ;
	mailHdr = mailH ;
}

void
DelayAck(int arg)
{
	PacketHeader outPktHdr;
	MailHeader outMailHdr;
	const char *buff = "Resend";

	AcknoData* ackdata = (AcknoData *) arg;

	outPktHdr.to = 0;		
	outMailHdr.to = 0;
	outMailHdr.from = 0;
	outMailHdr.length = strlen(buff) + 1;

	Delay(TEMPO) ;

	ackdata->postOffice->Send(outPktHdr,outMailHdr,buff) ;
	printf("Box : %d\n",ackdata->box);
}

void 
ReliableSend(PostOffice *postOffice, PacketHeader ouPktHdr, MailHeader ouMailHdr, const char* data, 
	int box, PacketHeader *inPktHdr, MailHeader *inMailHdr, char* buffer)
{
	int sended , reem ;
	reem = 0 ;
	sended = 0 ;

	while(reem < MAXREEMISSIONS && sended == 0)
	{

		//Message to be sent
		postOffice->Send(ouPktHdr,ouMailHdr,data) ;
		printf("Message sended\n");


		AcknoData *ackdata = new AcknoData(postOffice,box,inPktHdr,inMailHdr);
		
		Thread *delay = new Thread("ack receiver", 1, 0);

	    delay->Fork(DelayAck, (int) ackdata);

		// Waiting for the aknowledgment coming from the receiver
		postOffice->Receive(box, inPktHdr, inMailHdr, buffer);
		
		if(strcmp(buffer,"Resend") == 0)
		{
			printf("Not sended ! Retry ... \n") ;
			reem ++ ;
		}

		else
		{
			sended = 1 ;
			printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr->from,inMailHdr->from);
			fflush(stdout);
		}
	}
}
