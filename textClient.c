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

    /*        Check args to ensure textClient has been called correctly.
            Then store hostname and port number specified by args         */
    check_args(argc,argv);
    const char *HOST = argv[1];
    const int PORT = atoi(argv[2]);

    /*        Zero out message buffer        to be sent.
            Then assign value of message from stdin or file        */
    memset(sendBuff,'\0',sizeof(sendBuff));
    sendBuff = get_message();

    /*        Create connection to server.
            Send message in sendBuff        */
    serv_addr.sin_family                 = AF_INET;
    serv_addr.sin_port                 = htons(PORT); 
    serv_addr.sin_addr.s_addr         = inet_addr(HOST);
    
    tcpSock = connect_serv(HOST,PORT,tcpSock,serv_addr);
    printf("\tTCP connection to %s : %d successful.\n",HOST,PORT); usleep(500 * 1000);
	
	printf("\nmain(): Initiating TCP Latency Test\n");
    tcp_latency_test(tcpSock, sendBuff);
    printf("\nmain(): Initiating UDP Latency Test\n");
    udp_latency_test(udpSock, sendBuff, serv_addr);

    /*        Clean up and exit        */
    //free(sendBuff); This causes an core dump for some reason
    disconnect_serv(tcpSock, udpSock); 
    printf("\nexiting textClient.\n\n"); usleep(500 * 1000);
    return 0;
}

//Checks for correct intialization of textClient
void check_args(int argc, char *argv[])
{
        printf("\ncheck_args(): checking program arguments..."); usleep(500 * 1000);
        if(strcmp(argv[1],"--lh") == 0)
        {
//                strcpy(argv[1],"127.0.0.1");        //this doesn't work for some reason
                argv[1] = "127.0.0.1";
        }
        /*        Print usage for --help flag OR invalid number of args        */
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

//Grabs the message
char *get_message()
{
        printf("\nget_message(): getting text message..."); usleep(500 * 1000);
        char *sendBuff = malloc(sizeof(char) * 1025);

        /*        Get message from stdin or file char by char.
                Store in sendBuff.
                Append a '\0'                        */
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
                /*        deletes newline control sequence from stdout - for readability
                        --------------------------------------------        */
                printf("\033[A");
                printf("\033[K");
                printf("\033[A");
                /*        -------------------------------------------        */

                printf("\n------------------------------\n");
        }
        printf("\n\ttext message ready to send."); usleep(500 * 1000);
        return sendBuff;
}

//creates the tcp socket and connects it to the server
int connect_serv(const char *HOST,int PORT,int sockfd,struct sockaddr_in serv_addr)
{
    printf("\nconnect_serv(): getting socket, establishing connection to server..."); usleep(500 * 1000);

    /*        Set sockfd as a socket with TCP over internet       */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n\tError: Could not create TCP socket."); usleep(500 * 1000);
        exit(1);
    }
		
    /*        define the domain used by serv_addr
              use port PORT
              set destination address as HOST               */
	
    serv_addr.sin_family                 = AF_INET;
    serv_addr.sin_port                 = htons(PORT); 
    serv_addr.sin_addr.s_addr         = inet_addr(HOST); 

    /*        perform an ACTIVE connection over the socket
              with the location of serv_addr                        */
    //TCP Socket
    if ( connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0 )
    {
		perror("\n\tError: TCP Connection to server failed.\n"); usleep(500 * 1000);
        exit(1);
    }
    printf("\tTCP connection to %s : %d successful.\n",HOST,PORT); usleep(500 * 1000);
        
    return sockfd;
}

//closes tcp and udp sockets
void disconnect_serv(int tcpSock, int udpSock)
{
        printf("\ndisconnect_serv(): Attempting to close connection to server..."); usleep(500 * 1000);
        /*        close connection        */
        if( close(tcpSock) < 0 )
        {
                printf("\n\tError: Could not close socket file descriptor -- sockfd: %d.",tcpSock); usleep(500 * 1000);

        }
        if( close(udpSock) < 0 )
        {
                printf("\n\tError: Could not close socket file descriptor -- sockfd: %d.",udpSock); usleep(500 * 1000);

        }
        printf("\n\tConnection closed successfully."); usleep(500 * 1000);

}

//tests the latency for the tcp socket
void tcp_latency_test(int sockfd, char *sendBuff)
{
	char *recvBuff = malloc(strlen(sendBuff));
	struct timeval tv, tv2;
	double total;
	int n;
	
	memset(recvBuff, '\0', sizeof(recvBuff));
	printf("\ntcp_latency_test(): Initiating...\n"); usleep(500 * 1000);
    //gets the time stamp
    gettimeofday(&tv, NULL);
    
    printf("tcp_latency_test(): Sending message of size %i bytes...\n", (int) strlen(sendBuff));
    if( write(sockfd,sendBuff,strlen(sendBuff)) < 0)
    {
		perror("\n\tError: Transmission of message failed"); usleep(500 * 1000);
        exit(1);
    }
    printf("tcp_latency_test(): Message sent\ntcp_latency_test(): Reading message...\n");
    
    if ((n = read(sockfd, recvBuff, sizeof(recvBuff))) < 1)
    {
		printf("\n\tError in socket: %i, errno: %s\n", n, strerror(errno));
		exit(0);
    }
    //second time stamp
    gettimeofday(&tv2, NULL);
    
    printf("\tMessage size %i bytes read\n", (int) strlen(recvBuff)); 
    
    //calculating latency
    total = ((double) (tv2.tv_usec - tv.tv_usec)) / CLOCKS_PER_SEC;
    printf("tcp_latency_test(): Round trip time for message size %i bytes was %f seconds\n", (int) strlen(recvBuff), total); usleep(500 * 1000);
               
}

//latency test for udp socket
void udp_latency_test(int sockfd, char *sendBuff, struct sockaddr_in serv_addr)
{
	struct timeval tv, tv2;
	socklen_t addrlen = sizeof(serv_addr);
	double total;
    char *recvBuff = malloc(strlen(sendBuff));
    int len, x;
	
	//socket creation
	printf("\nudp_latency_test(): Initializing...\nudp_latency_test(): Creating socket...\n");
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
            printf("\n\tError: Could not create UDP socket."); usleep(500 * 1000);
            exit(1);
    }
	printf("\tSuccess!\n");
	
	memset(recvBuff, '\0', sizeof(recvBuff));
	printf("\nudp_latency_test(): Initiating...\n"); usleep(500 * 1000);
    //first time stamp
    gettimeofday(&tv, NULL);
    
    printf("udp_latency_test(): Sending message of size %i bytes...\n", (int) strlen(sendBuff));
    //sending to server
    if((x = sendto(sockfd, sendBuff, strlen(sendBuff), 0, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr))) < 0)
    {
		perror("\n\tError: Transmission of UDP message failed");
		//-- sendBuff: %s",sendBuff); usleep(500 * 1000);
        exit(1);
    }
	printf("udp_latency_test(): Message sent\n");//latency_test(): Reading message...\n");
     
    len = recvfrom(sockfd, sendBuff, strlen(sendBuff), 0, 0, 0); //put in 0's because we know the server already
	
	//second time stamp
    gettimeofday(&tv2, NULL);
    
    printf("\tMessage size %i bytes read\n", len); 
    
    //calculating latency
    total = ((double) (tv2.tv_usec - tv.tv_usec)) / CLOCKS_PER_SEC;
    printf("udp_latency_test(): Round trip time for message size %i  bytes was %f seconds\n", len, total); usleep(500 * 1000);
}
