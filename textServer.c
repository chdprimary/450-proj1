/*
	TODO:
	* upgrade sockaddr to addr_info
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
	int i;
	int n;
	int listenfd=0, connfd=0;

	struct sockaddr_in serv_addr;
	char recvBuff[1024];
	int numrv;

	/*	set listenfd as a socket with TCP over internet
		print success message				*/
	listenfd = socket(AF_INET,SOCK_STREAM,0);
	printf("socket retrieve success\n");


	/*	zero out serv_addr struct
		zero out recvBuff		*/
	memset(&serv_addr, '0', sizeof(serv_addr));
	memset(recvBuff, '0', sizeof(recvBuff));


	/*	define the domain used by serv_addr
		permit any IP address with INADDR_ANY
		use port 5000				*/
	serv_addr.sin_family 		= AF_INET;
	serv_addr.sin_addr.s_addr 	= htonl(INADDR_ANY);
	serv_addr.sin_port 		= htons(5000);


	/*	bind socket to serv_addr	*/
	bind(listenfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));


	/*	start listening with 10 maximum possible simultaneous requests	*/
	if(listen(listenfd,10) == -1) { printf("Failed to listen\n"); }


	/*	accept incoming connection from client
                reads the message, places in recvBuff, appends a zero   */
	while(1)
	{
		connfd = accept(listenfd, (struct sockaddr*)NULL,NULL);

		while((n = read(connfd, recvBuff, sizeof(recvBuff)-1)) > 0)
		{
			recvBuff[n] = 0;
                	if(fputs(recvBuff,stdout) == EOF)
                	        printf("\nError: Fputs error");
        	        printf("\n");
		}


	    /*      if read() returned a -1, relay the error to user        */
		if(n < 0)
		{
			printf("\nError in socket\n");
			exit(0);
		}

		sleep(1);
	}

	return 0;
}

