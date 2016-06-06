#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define BUFSIZE 512
#define MYPORT 8000
#define MAXNAME 100 

#define NTP_REQ 0x74160004
#define PLAY_TIME_REQ 0x7416004 + 0x1000
#define INITIAL_CONNECT_REQ 0x7416004 + 0x2000

struct stMsg{
	unsigned int ulMsgId;
	unsigned int Time;     // Reserved
	//char        cName[MAX_FILE_NAME];
};

int main(int argc,  char *argv[] )
{
	int	sd, numbytes,bytessent, ret, client_num=0,i;
	struct	sockaddr_in
		serveraddress,cliaddr[3];
	socklen_t length;
	char clientname[MAXNAME],datareceived[BUFSIZE];

	struct stMsg newstUdpMsg;
	struct stMsg *stUdpMsg;
	sd = socket( AF_INET, SOCK_DGRAM, 0 );
	if(0 > sd ) 
	{
		perror( "socket" );
		exit( 1 );
	}
	
	memset( &serveraddress, 0, sizeof(serveraddress) );
	for(i=0;i<3;i++)
	{
		memset( &cliaddr[i], 0, sizeof(struct sockaddr_in) );
	}
	serveraddress.sin_family = AF_INET;
	serveraddress.sin_port = htons(MYPORT);//PORT NO
	serveraddress.sin_addr.s_addr = htonl(INADDR_ANY);//IP ADDRESS
	ret=bind(sd,(struct sockaddr*)&serveraddress,sizeof(serveraddress));
	if(0 > ret)
	{
		perror("Bind Fails:");
		exit(1);
	}

	time_t timer;


//	while(1)
//	{
		printf("UDP Server:  Waiting for client data\n");
		length=sizeof(cliaddr);

		/*Received a datagram*/
		numbytes = recvfrom(sd,datareceived,BUFSIZE,0,
                                (struct sockaddr*)&cliaddr[0],&length);

		if (0 > numbytes)
		{
			perror("Error while receiving:");
			exit(1);
		}

		//printf("datarevevied : %s\n",datareceived);
		stUdpMsg = (struct stMsg *)datareceived;
		//printf("stMsg.Msg : %d\n",stUdpMsg->ulMsgId);
		datareceived[numbytes]='\0';

		if(stUdpMsg->ulMsgId == NTP_REQ )
		{
			//시간 동기화 과정 
		}
		else if(stUdpMsg->ulMsgId == PLAY_TIME_REQ )
		{
			memset( &newstUdpMsg, 0, sizeof(newstUdpMsg) );

			printf("debug PLAY_TIME_REQ\n");
			newstUdpMsg.ulMsgId = PLAY_TIME_REQ;
			timer = time(NULL);
			newstUdpMsg.Time = timer+3;
			for(i=0;i<client_num;i++)
			{
				printf("client [%d]\n",i);
				bytessent = sendto(sd,&newstUdpMsg,sizeof(struct stMsg),0,(struct sockaddr *)&cliaddr[i],length);
				if (0 > bytessent)
				{
					perror("Error while sending:");
					exit(1);
				}
			}
		}
		else if(stUdpMsg->ulMsgId == INITIAL_CONNECT_REQ )
		{
			printf("Data Received from %s\n",
				inet_ntop(AF_INET,&cliaddr[client_num].sin_addr,
					clientname,sizeof(clientname)));
			client_num++;
			printf("client number : %d\n",client_num);
		}
		else
		{
			//error
		}

		numbytes = recvfrom(sd,datareceived,BUFSIZE,0,
                                (struct sockaddr*)&cliaddr[1],&length);

		if (0 > numbytes)
                {
                        perror("Error while receiving:");
                        exit(1);
                }

		//printf("datarevevied : %s\n",datareceived);
                stUdpMsg = (struct stMsg *)datareceived;
                //printf("stMsg.Msg : %d\n",stUdpMsg->ulMsgId);
                datareceived[numbytes]='\0';
		
		if(stUdpMsg->ulMsgId == INITIAL_CONNECT_REQ )
                {
                        printf("Data Received from %s\n",
                                inet_ntop(AF_INET,&cliaddr[client_num].sin_addr,
                                        clientname,sizeof(clientname)));
                        client_num++;
                        printf("client number : %d\n",client_num);
                }

	

		numbytes = recvfrom(sd,datareceived,BUFSIZE,0,
                                (struct sockaddr*)&cliaddr[0],&length);

		if (0 > numbytes)
                {
                        perror("Error while receiving:");
                        exit(1);
                }
		
		 //printf("datarevevied : %s\n",datareceived);
                stUdpMsg = (struct stMsg *)datareceived;
                //printf("stMsg.Msg : %d\n",stUdpMsg->ulMsgId);
                datareceived[numbytes]='\0';

		
		if(stUdpMsg->ulMsgId == PLAY_TIME_REQ )
                {
                        memset( &newstUdpMsg, 0, sizeof(newstUdpMsg) );

                        printf("debug PLAY_TIME_REQ\n");
                        newstUdpMsg.ulMsgId = PLAY_TIME_REQ;
                        timer = time(NULL);
                        newstUdpMsg.Time = timer+3;
                        for(i=0;i<client_num;i++)
                        {
                                printf("client [%d]\n",i);
                                bytessent = sendto(sd,&newstUdpMsg,sizeof(struct stMsg),0,(struct sockaddr *)&cliaddr[i],length);
                                if (0 > bytessent)
                                {
                                        perror("Error while sending:");
                                        exit(1);
                                }
                        }
                }

//	}
}



