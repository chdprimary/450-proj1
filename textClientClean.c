/****************************************************************
 * Christian Dean                      |\      _,,,---,,_       *
 * Zachry Hjorth                 ZZZzz /,`.-'`'    -.  ;-;;,_   *
 * Due: 12/15/14                      |,4-  ) )-,_. ,\ (  `'-'  *
 * textClient2.c                      '---''(_/--'  `-'\_)      *
 *                                                              *
 * A program that programs a programmer to program a program 	*
 * that redoes the previously redone redos.      				*
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
#include <time.h>
#include <errno.h>

void check_args(int,char**);
char *get_message();
int connect_serv(const char*,int,int, struct sockaddr_in);
void disconnect_serv(int,int);
void tcp_latency_test(int, char*);
void udp_latency_test(int, char*, struct sockaddr_in);

int main(int argc, char *argv[])
{
	int tcpSock = 0; 
	int udpSock = 0;
    struct sockaddr_in serv_addr;
        
    char *sendBuff = malloc(sizeof(char) * 1025);

    check_args(argc,argv);
    const char *HOST = argv[1];
    const int PORT = atoi(argv[2]);

    memset(sendBuff,'\0',sizeof(sendBuff));
    sendBuff = get_message();

    serv_addr.sin_family                 = AF_INET;
    serv_addr.sin_port                 = htons(PORT); 
    serv_addr.sin_addr.s_addr         = inet_addr(HOST);
    
    tcpSock = connect_serv(HOST,PORT,tcpSock,serv_addr);
	
    tcp_latency_test(tcpSock, sendBuff);
    udp_latency_test(udpSock, sendBuff, serv_addr);

    //free(sendBuff); This causes a core dump for some reason
    disconnect_serv(tcpSock, udpSock); 
    return 0;
}

void check_args(int argc, char *argv[])
{
        if(strcmp(argv[1],"--lh") == 0)
                argv[1] = "127.0.0.1";

        if ( ((argv[0] == "./textClient" || argv[0] == "textClient") && argv[1] == "--help") || argc != 3 )
        {
                printf("\nUsage: [cat FILE_IN.TXT |] ./textClient HOST PORT_NUM\n");
                printf("\n\tFlags: --lh: sets HOST as '127.0.0.1' if specified in place of HOST argument.\n");
                printf("\tTakes a text message from stdin or from a pipe, and transmits it to HOST:PORT_NUM.\n\n");
                exit(1);
        }
        else
                printf("\n\ttextClient started successfully.");usleep(500 * 1000);
}

char *get_message()
{
        char *sendBuff = malloc(sizeof(char) * 1025);

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
                printf("\033[A");
                printf("\033[K");
                printf("\033[A");
                printf("\n------------------------------\n");
        }
        return sendBuff;
}

int connect_serv(const char *HOST,int PORT,int sockfd,struct sockaddr_in serv_addr)
{

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        exit(1);
		
    serv_addr.sin_family                 = AF_INET;
    serv_addr.sin_port                 = htons(PORT); 
    serv_addr.sin_addr.s_addr         = inet_addr(HOST); 

    if ( connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0 )
        exit(1);
        
    return sockfd;
}

void disconnect_serv(int tcpSock, int udpSock)
{
        if( close(tcpSock) < 0 )
                printf("\n\tError: Could not close socket file descriptor -- sockfd: %d.",tcpSock); usleep(500 * 1000);

        if( close(udpSock) < 0 )
                printf("\n\tError: Could not close socket file descriptor -- sockfd: %d.",udpSock); usleep(500 * 1000);
}

void tcp_latency_test(int sockfd, char *sendBuff)
{
	char *recvBuff = malloc(strlen(sendBuff));
	struct timeval tv, tv2;
	double total;
	int n;
	
	memset(recvBuff, '\0', sizeof(recvBuff));
  
    gettimeofday(&tv, NULL);
    printf("tcp_latency_test(): Sending message of size %i bytes...\n", (int) strlen(sendBuff));
    if (write(sockfd,sendBuff,strlen(sendBuff)) < 0)
        exit(1);

    if ((n = read(sockfd, recvBuff, sizeof(recvBuff))) < 1)
		exit(0);
	gettimeofday(&tv2, NULL);
    printf("\tMessage size %i bytes read\n", (int) strlen(recvBuff)); 
    
    total = ((double) (tv2.tv_usec - tv.tv_usec)) / CLOCKS_PER_SEC;
    printf("tcp_latency_test(): Round trip time for message size %i bytes was %f seconds\n", (int) strlen(recvBuff), total); usleep(500 * 1000);
               
}

void udp_latency_test(int sockfd, char *sendBuff, struct sockaddr_in serv_addr)
{
	struct timeval tv, tv2;
	socklen_t addrlen = sizeof(serv_addr);
	double total;
    char *recvBuff = malloc(strlen(sendBuff));
    int len, x;
	
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
            exit(1);

	memset(recvBuff, '\0', sizeof(recvBuff));

    gettimeofday(&tv, NULL);
    printf("udp_latency_test(): Sending message of size %i bytes...\n", (int) strlen(sendBuff));
    if((x = sendto(sockfd, sendBuff, strlen(sendBuff), 0, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr))) < 0)
        exit(1);
        
    len = recvfrom(sockfd, sendBuff, strlen(sendBuff), 0, 0, 0); 
    gettimeofday(&tv2, NULL);
    printf("\tMessage size %i bytes read\n", len); 
    
    total = ((double) (tv2.tv_usec - tv.tv_usec)) / CLOCKS_PER_SEC;
    printf("udp_latency_test(): Round trip time for message size %i  bytes was %f seconds\n", len, total); usleep(500 * 1000);
}

