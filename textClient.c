/****************************************************************
 * Christian Dean                      |\      _,,,---,,_       *
 * Zachry Hjorth                 ZZZzz /,`.-'`'    -.  ;-;;,_   *
 * Due: 12/15/14                      |,4-  ) )-,_. ,\ (  `'-'  *
 * textClient.c                      '---''(_/--'  `-'\_)       *
 *                                                              *
 * A program that programs programs to undo redos.              *
 ***************************************************************/

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

void check_args(int,char**);
char *get_message();
int connect_serv(const char*,int,int,struct sockaddr_in);
void send_message(int,char*);
void disconnect_serv(int);

int main(int argc, char *argv[])
{
	int sockfd = 0;
	struct sockaddr_in serv_addr;
	char *sendBuff = malloc(sizeof(char) * 1025);


	/*	Check args to ensure textClient has been called correctly.
		Then store hostname and port number specified by args 	*/
	check_args(argc,argv);
	const char *HOST = argv[1];
	const int PORT = atoi(argv[2]);


	/*	Zero out message buffer	to be sent.
		Then assign value of message from stdin or file	*/
	memset(sendBuff,'0',sizeof(sendBuff));
	sendBuff = get_message();


	/*	Create connection to server.
		Send message in sendBuff	*/
	sockfd = connect_serv(HOST,PORT,sockfd,serv_addr);
	send_message(sockfd,sendBuff);


	/*	Clean up and exit	*/
	free(sendBuff);
	disconnect_serv(sockfd);
	printf("\nexiting textClient.\n\n"); usleep(500 * 1000);
	return 0;
}

void check_args(int argc, char *argv[])
{
	printf("\ncheck_args(): checking program arguments..."); usleep(500 * 1000);
	if(strcmp(argv[1],"--lh") == 0)
	{
//		strcpy(argv[1],"127.0.0.1");	//this doesn't work for some reason
		argv[1] = "127.0.0.1";
	}
	/*	Print usage for --help flag OR invalid number of args	*/
	if ( ((argv[0] == "./textClient" || argv[0] == "textClient") && argv[1] == "--help") || argc != 3 )
	{
		printf("\nUsage: [cat FILE_IN.TXT |] ./textClient HOST PORT_NUM\n");
		printf("\n\tFlags: --lh: sets HOST as '127.0.0.1' if specified in place of HOST argument.\n");
		printf("\tTakes a text message from stdin or from a pipe, and transmits it to HOST:PORT_NUM.\n\n");
		exit(1);
	}
	else
	{
		printf("\n\ttextClient started successfully.");usleep(500 * 1000);
	}
}

char *get_message()
{
	printf("\nget_message(): getting text message..."); usleep(500 * 1000);
	char *sendBuff = malloc(sizeof(char) * 1025);

	/*	Get message from stdin or file char by char.
		Store in sendBuff.
		Append a '\0'			*/
	if (isatty(STDIN_FILENO))
		printf("\n\tPlease type a message to be sent (press ENTER twice to finish):\n\nMESSAGE\n------------------------------\n"); usleep(500 * 1000);
	char ch = getchar();
	int index = 0;
	while(ch != EOF && ch != '\0')
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
	if (isatty(STDIN_FILENO))
	{
		/*	deletes newline control sequence from stdout - for readability
			--------------------------------------------	*/
		printf("\033[A");
		printf("\033[K");
		printf("\033[A");
		/*	-------------------------------------------	*/

		printf("\n------------------------------\n");
	}
	printf("\n\ttext message ready to send."); usleep(500 * 1000);
	return sendBuff;
}

int connect_serv(const char *HOST,int PORT,int sockfd,struct sockaddr_in serv_addr)
{
	printf("\nconnect_serv(): getting socket, establishing connection to server..."); usleep(500 * 1000);

	/*	Set sockfd as a socket with TCP over internet	*/
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n\tError: Could not create socket."); usleep(500 * 1000);
		exit(1);
	}


	/*	define the domain used by serv_addr
		use port PORT
		set destination address as HOST		*/
	serv_addr.sin_family 		= AF_INET;
	serv_addr.sin_port 		= htons(PORT);
	serv_addr.sin_addr.s_addr 	= inet_addr(HOST);


	/*	perform an ACTIVE connection over the socket
		with the location of serv_addr			*/
	if ( connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0 )
	{
		printf("\n\tError: Connection to server failed.\n"); usleep(500 * 1000);
		exit(1);
	}
	printf("\n\tconnection to %s : %d successful.",HOST,PORT); usleep(500 * 1000);
	return sockfd;
}

void send_message(int sockfd, char *sendBuff)
{
	printf("\nsend_message(): Attempting to send message of size %d...",(int)strlen(sendBuff));
	/*	write buffer into connection	*/
	if( write(sockfd,sendBuff,strlen(sendBuff)) < 0)
	{
		printf("\n\tError: Transmission of message failed -- sendBuff: %s",sendBuff); usleep(500 * 1000);
		exit(1);
	}
	printf("\n\tMessage sent successfully."); usleep(500 * 1000);

}

void disconnect_serv(int sockfd)
{
	printf("\ndisconnect_serv(): Attempting to close connection to server..."); usleep(500 * 1000);
	/*	close connection	*/
	if( close(sockfd) < 0 )
	{
		printf("\n\tError: Could not close socket file descriptor -- sockfd: %d.",sockfd); usleep(500 * 1000);

	}
	printf("\n\tConnection closed successfully."); usleep(500 * 1000);

}

