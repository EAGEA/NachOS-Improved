#include "copyright.h"
#include "post.h"
#include "thread.h"
#include "reliable.h"
#include "synchlist.h"
#include "list.h"

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
	int i ;
	PacketHeader outPktHdr;
	MailHeader outMailHdr;
	const char *buff = "Resend";

	AcknoData* ackdata = (AcknoData *) arg;

	outPktHdr.to = 0;		
	outMailHdr.to = 0;
	outMailHdr.from = 0;
	outMailHdr.length = strlen(buff) + 1;

	Delay(TEMPO) ;

	for(i = 0 ; i < MAXREEMISSIONS ; i ++)
	{

		ackdata->postOffice->Send(outPktHdr,outMailHdr,buff) ;

	}	

	printf("End of ackno thread\n") ;
}

void 
ReliableSend(PostOffice *postOffice, PacketHeader ouPktHdr, MailHeader ouMailHdr, const char* data, 
	int box, PacketHeader *inPktHdr, MailHeader *inMailHdr, char* buffer)
{
	int sent , reem ;
	reem = 0 ;
	sent = 0 ;

	PacketHeader tempoPktHdr ;
	MailHeader tempoMailHdr ;

	while(reem < MAXREEMISSIONS && sent == 0)
	{

		//Message to be sent
		postOffice->Send(ouPktHdr,ouMailHdr,data) ;
		printf("Message sended\n");

		AcknoData *ackdata = new AcknoData(postOffice,box,inPktHdr,inMailHdr);
			
		Thread *delay = new Thread("ack receiver", reem + 1, 0);

	    delay->Fork(DelayAck, (int) ackdata);

		// Waiting for the aknowledgment coming from the receiver
		postOffice->Receive(0, &tempoPktHdr, &tempoMailHdr, buffer);

		printf("Recu : %s\n",buffer);

		if(strcmp(buffer,"Resend") == 0)
		{
			printf("Not sended ! Retry ... \n") ;
		}

		else
		{
			sent = 1 ;
			printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr->from,inMailHdr->from);
			fflush(stdout);
		}
		reem ++ ;
	}
}
