/*
	TODO:
	* Encapsulate everything into functions
*/

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

void checkargs(int,char**);

int main(int argc, char *argv[])
{
	int sockfd = 0;
	char sendBuff[1025];
	struct sockaddr_in serv_addr;

	checkargs(argc,argv);

	const char *HOST = argv[1];
	const int PORT = atoi(argv[2]);


	/*	Zero out message buffer	to be sent	*/
	memset(sendBuff,'0',sizeof(sendBuff));


	/*	Get message from stdin or file char by char. Store in sendBuff. Append a '\0'.	*/
	if (isatty(STDIN_FILENO))
		printf("\nPlease enter your message:\n");
	char ch = getchar();
	int index = 0;
	//can type EOF and \0 ?
	while(ch != -1 && ch != 0)
	{
		if (ch == '\n')
		{
			if ( (ch = getchar()) == '\n')
			{
				ch = -1;
				break;
			}
			else
			{
				ungetc(ch,stdin);
				ch = '\n';
			}
		}
		sendBuff[index] = ch;
		++index;
		ch = getchar();
	}
	sendBuff[index] = '\0';


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
	serv_addr.sin_port 		= htons(PORT);
	serv_addr.sin_addr.s_addr 	= inet_addr(HOST);


	/*	perform an ACTIVE connection over the socket with the machine
		at serv_addr	*/
	if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
	{
		printf("\nError: Connect Failed\n");
		return 1;
	}


	/*      write buffer into connection
                close connection		*/
	write(sockfd,sendBuff,strlen(sendBuff));
//	sleep(1);
	close(sockfd);

	/*	ends program	*/
	return 0;
}

void checkargs(int argc, char *argv[])
{
	/*	Print usage for --help flag OR invalid number of args	*/
	if ( ((argv[0] == "./textClient" || argv[0] == "textClient") && argv[1] == "--help") || argc != 3 )
	{
		printf("\nUsage: [cat FILE_IN.TXT |] ./textClient HOST PORT_NUM\n");
		printf("\tTakes a text message from stdin or from a pipe, and transmits it to HOST:PORT_NUM.\n\n");
		exit(1);
	}
}
