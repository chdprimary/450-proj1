#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
	int sockfd = 0;
	char sendBuff[1025];
	struct sockaddr_in serv_addr;


	/*	zero out received message buffer	*/
	memset(sendBuff,'0',sizeof(sendBuff));


	/*      we take the first cmdline arg as the message to be passed       */
	if (argc!=2)
	{
		printf("You must enter a message as an argument!\n");
		return 0;
	}
	else
		strcpy(sendBuff,argv[1]);


	/*	set sockfd as a socket with TCP over internet	*/
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("Could not create socket \n");
		return 1;
	}


	/*	define the domain used by serv_addr
	        use port 5000
		set internet address as localhost	*/
	serv_addr.sin_family 		= AF_INET;
	serv_addr.sin_port 		= htons(5000);
	serv_addr.sin_addr.s_addr 	= inet_addr("127.0.0.1");


	/*	perform an ACTIVE connection over the socket with the machine at serv_addr	*/
	if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
	{
		printf("\nError: Connect Failed\n");
		return 1;
	}


	/*      write buffer into connection
                close connection		*/
	write(sockfd,sendBuff,strlen(sendBuff));
	sleep(1);
	close(sockfd);

	/*	ends program	*/
	return 0;
}
