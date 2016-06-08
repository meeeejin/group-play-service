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
#include <unistd.h>

#define BUFSIZE 512
#define MYPORT 10122
#define MAXNAME 100 

#define NTP_REQ 0x74160004
#define PLAY_TIME_REQ 0x7416004 + 0x1000
#define INITIAL_CONNECT_REQ 0x7416004 + 0x2000

struct stMsg{
	unsigned int ulMsgId;
	unsigned int Time;
};

int main(int argc,  char *argv[] )
{
	int	sd, numbytes, bytessent, ret, client_num=0, i;
	struct	sockaddr_in serveraddress, cliaddr[3];
	socklen_t length;
	char clientname[MAXNAME], datareceived[BUFSIZE];
	time_t timer;

	struct stMsg newstUdpMsg;
	struct stMsg *stUdpMsg;

	// Create a socket
	sd = socket( AF_INET, SOCK_DGRAM, 0 );
	if(0 > sd ) 
	{
		perror( "socket" );
		exit( 1 );
	}
	
	// Initialize the server address
	memset( &serveraddress, 0, sizeof(serveraddress) );
	for(i=0;i<3;i++)
	{
		memset( &cliaddr[i], 0, sizeof(struct sockaddr_in) );
	}
	serveraddress.sin_family = AF_INET;
	serveraddress.sin_port = htons(MYPORT);//PORT NO
	serveraddress.sin_addr.s_addr = htonl(INADDR_ANY);//IP ADDRESS

	// Bind
	ret = bind(sd,(struct sockaddr*)&serveraddress,sizeof(serveraddress));
	if(0 > ret)
	{
		perror("Bind Fails:");
		exit(1);
	}
	printf("bind ok\n");

	printf("UDP Server:  Waiting for client data\n");

	length = sizeof(cliaddr);

	// Receive a datagram from client 0
	numbytes = recvfrom(sd, datareceived, BUFSIZE, 0,
							(struct sockaddr*)&cliaddr[0], &length);
	printf("received datagram from client 0, numbytes = %d\n", numbytes);
	if (0 > numbytes)
	{
		perror("Error while receiving:");
		exit(1);
	}

	stUdpMsg = (struct stMsg *) datareceived;
	datareceived[numbytes] = '\0';
	printf("MSG TYPE: %x\n", ntohl(stUdpMsg->ulMsgId));

	if(ntohl(stUdpMsg->ulMsgId) == INITIAL_CONNECT_REQ )
	{
		printf("Data Received from %s, %d\n",
			inet_ntop(AF_INET,&cliaddr[client_num].sin_addr,
				clientname,sizeof(clientname)), cliaddr[client_num].sin_port);
		printf("client number : %d\n", client_num);
		client_num++;
	}
	else
	{
		// error
		printf("MSG TYPE ERROR\n");
	}

	// Receive a datagram from client 1
	numbytes = recvfrom(sd, datareceived, BUFSIZE, 0,
							(struct sockaddr*)&cliaddr[1], &length);
	printf("received datagram from client 1, numbytes = %d\n", numbytes);
	if (0 > numbytes)
	{
		perror("Error while receiving:");
		exit(1);
	}

	stUdpMsg = (struct stMsg *) datareceived;
	datareceived[numbytes] = '\0';
	printf("MSG TYPE: %x\n", ntohl(stUdpMsg->ulMsgId));

	if(ntohl(stUdpMsg->ulMsgId) == INITIAL_CONNECT_REQ )
	{
			printf("Data Received from %s, %d\n",
					inet_ntop(AF_INET,&cliaddr[client_num].sin_addr,
							clientname,sizeof(clientname)), cliaddr[client_num].sin_port);
			printf("client number : %d\n", client_num);
			client_num++;
	}
	else
	{
		// error
		printf("MSG TYPE ERROR\n");		
	}

	printf("connection complete\n");
	printf("sleep start\n");
	usleep(20 * 1000 * 1000);
	printf("sleep end\n");

	// Send the response message to the clients
	printf("Send the response to the clients..\n");

	memset( &newstUdpMsg, 0, sizeof(newstUdpMsg) );
	newstUdpMsg.ulMsgId = htonl(PLAY_TIME_REQ);
	timer = time(NULL);
	newstUdpMsg.Time = htonl(timer+3);

	for(i=0;i<client_num;i++)
	{
		printf("client [%d], port = %d\n", i, cliaddr[i].sin_port);
		bytessent = sendto(sd, &newstUdpMsg, sizeof(struct stMsg), 0, (struct sockaddr *)&cliaddr[i], length);
		printf("bytessent = %d\n", bytessent);
		if (bytessent < 0)
		{
			printf("sendto error\n");
			exit(1);
		}
		printf("sendto complete %d\n", i);
	}

	printf("end\n");
}
