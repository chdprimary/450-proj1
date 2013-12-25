/*
	TODO:
	* upgrade sockaddr to addr_info
	* figure out how to display usage from --help flag
	* encapsulate everything into functions
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

void check_args(int,char**);
int setup(int,int,struct sockaddr_in);
void start_listening(int);
void print_messages(int);

int main(int argc, char *argv[])
{
	int i;
	int listenfd = 0;
	struct sockaddr_in serv_addr;

	/*	zero out serv_addr struct	*/
	memset(&serv_addr, '0', sizeof(serv_addr));

	check_args(argc,argv);
	const int PORT = atoi(argv[1]);

	listenfd = setup(PORT,listenfd,serv_addr);

	start_listening(listenfd);

	print_messages(listenfd);

	printf("\nNow exiting.");
	return 0;
}

void check_args(int argc, char *argv[])
{
        printf("\ncheck_args(): checking program arguments..."); usleep(500 * 1000);
        /*      Print usage for --help flag OR invalid number of args   */
        if ( ((argv[0] == "./textServer" || argv[0] == "textServer") && argv[1] == "--help") || argc != 2 )
        {
                printf("\nUsage: ./textServer PORT_NUM\n");
                printf("\tListens for text messages on PORT_NUM and displays them on stdout.\n\n");
                exit(1);
        }
	printf("\n\ttextServer started successfully.");
}

int setup(int PORT,int listenfd,struct sockaddr_in serv_addr)
{
        printf("\nsetup(): Getting socket, binding socket to server port..."); usleep(500 * 1000);
	/*	set listenfd as a socket with TCP over internet
		print success message				*/
	listenfd = socket(AF_INET,SOCK_STREAM,0);

	/*	define the domain used by serv_addr
		permit any IP address with INADDR_ANY
		use port 5000				*/
	serv_addr.sin_family 		= AF_INET;
	serv_addr.sin_addr.s_addr 	= htonl(INADDR_ANY);
	serv_addr.sin_port 		= htons(PORT);

	/*	bind socket to serv_addr	*/
	bind(listenfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));

	printf("\n\tSetup successful."); usleep(500 * 1000);
	return listenfd;
}

void start_listening(int listenfd)
{
	printf("\nstart_listening(): Attempting to begin listening for connections...");
	/*	start listening with 10 maximum possible simultaneous requests	*/
	if(listen(listenfd,10) == -1) { printf("Failed to listen\n"); }
	printf("\n\tNow listening for connections.");
}

void print_messages(int listenfd)
{
	printf("\nprint_messages(): Now accepting incoming text messages...\n");

	int n;
	int connfd = 0;
	char recvBuff[1024];

	memset(recvBuff, '0', sizeof(recvBuff));

	/*	accept incoming connection from client
                reads the message, places in recvBuff, appends a zero   */
	while(1)
	{
		connfd = accept(listenfd, (struct sockaddr*)NULL,NULL);
		printf("\nMessage\n----------------------\n");
		while((n = read(connfd, recvBuff, sizeof(recvBuff)-1)) > 0)
		{
			recvBuff[n] = 0;
                	if(fputs(recvBuff,stdout) == EOF)
                	        printf("\nError: Fputs error");
        	        printf("\n----------------------\n");
		}


		/*      if read() returned a -1, relay the error to user        */
		if(n < 0)
		{
			printf("\n\tError in socket\n");
			exit(0);
		}

		sleep(1);
	}
}

