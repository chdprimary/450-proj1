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
void setup(int,struct sockaddr_in);
void tcp_latency_test(int);
void udp_latency_test(int, struct sockaddr_in, const int);
int tcp_setup(const int,int,struct sockaddr_in);
int udp_setup(int, int, struct sockaddr_in);
void tcp_listen(int);

int main(int argc, char *argv[])
{
    int tcpSock = 0;
    int udpSock = 0;
    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;

    /*        zero out serv_addr struct        */
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(&client_addr, '0', sizeof(client_addr));

    check_args(argc,argv);
    const int PORT = atoi(argv[1]);

    tcpSock = tcp_setup(PORT, tcpSock, serv_addr); usleep(500 * 1000);
    tcp_listen(tcpSock); usleep(500 * 1000);
    udpSock = udp_setup(PORT, udpSock, serv_addr); usleep(500 * 1000);
    
    
    int i;
    for(i = 0; i < 11; i++)
	{
		tcp_latency_test(tcpSock);
		usleep(500 * 1000);
		udp_latency_test(udpSock, client_addr, PORT);
	}
	
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
        printf("\n\ttextServer started successfully.\n");
}

int tcp_setup(int PORT, int sockfd, struct sockaddr_in serv_addr)
{
        printf("\ntcp_setup(): Getting socket, binding socket to server port..."); usleep(500 * 1000);
        /*        set listenfd as a socket with TCP over internet
                print success message                                */
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
        {
            perror("setup(): TCP Socket error");
            exit(1);
        }
       
        /*        define the domain used by serv_addr
                permit any IP address with INADDR_ANY
                use port 5000                                */
        serv_addr.sin_family                 = AF_INET;
        serv_addr.sin_addr.s_addr         = htonl(INADDR_ANY);
        serv_addr.sin_port                 = htons(PORT);
		bzero(&(serv_addr.sin_zero),8);
        
        /*        bind socket to serv_addr        */
        if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        {
			perror("tcp_setup(): TCP Socket failed to bind.");
			
			exit(1);
		}

        printf("\n\tSetup successful."); usleep(500 * 1000);
        return sockfd;
}

void tcp_listen(int sockfd)
{
    printf("\nstart_listening(): Attempting to begin listening for connections...\n");
    /*        start listening with 10 maximum possible simultaneous requests        */
    if(listen(sockfd,10) == -1) { perror("\tFailed to listen\n"); exit(1); }
    printf("\tNow listening for connections.\n");
}

void tcp_latency_test(int sockfd)
{
    printf("\ntcp_latency_test(): Initiating...\n");
	char recvBuff[1024];
	memset(recvBuff, '\0', sizeof(recvBuff));
    int n, m;
    int connfd = 0;
    /*        accept incoming connection from client
            reads the message, places in recvBuff, appends a zero   */
    int i, x;

	//for(i = 0; i < 11; i++)
	{
		printf("tcp_latency_test(): Waiting for client...\n");
		connfd = accept(sockfd, (struct sockaddr*)NULL,NULL);
		printf("\tConnection successful\ntcp_latency_test(): Reading message...\n");
		n = read(connfd, recvBuff, sizeof(recvBuff)-1) < 0;
		
		if (n < 0)
		{
			perror("Failed to read");
			exit(1);
		}
		
		printf("\tMessage size %i bytes read.\nlatency_test(): Returning Message\n", (int) strlen(recvBuff));
		if (m = write(connfd, recvBuff, strlen(recvBuff)) < 0)
			printf("\ntcp_latency_test(): Error resending message\n"); 
			
		printf("tcp_latency_test(): Sent message of size %i bytes\n\n", (int) strlen(recvBuff));
		memset(recvBuff, '\0', sizeof(recvBuff));
	}
}

int udp_setup(int PORT, int sockfd, struct sockaddr_in serv_addr)
{
	serv_addr.sin_family                 = AF_INET;
    serv_addr.sin_addr.s_addr         = htonl(INADDR_ANY);
    serv_addr.sin_port                 = htons(PORT);
	bzero(&(serv_addr.sin_zero),8);
    
    printf("udp_setup(): Initializing...\n");
    printf("udp_setup(): Creating Socket...\n"); usleep(500 * 1000);
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
    {
        perror("udp_setup(): UDP Socket error");
        exit(1);
    } 
    
	printf("\tSuccess!\nudp_setup(): Binding socket...\n"); usleep(500 * 1000);
	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0) 
    {
		perror("udp_setup(): UDP Socket failed to bind.");
		exit(1);
	}
	printf("\tSuccess!\nWaiting for Client message...\n");
	return sockfd;
}

void udp_latency_test(int sockfd, struct sockaddr_in serv_addr, const int PORT)
{
	int len;
    struct sockaddr_in client_addr;
    char recvBuff[1024];
    
	memset(&recvBuff, '\0', sizeof(recvBuff));
	memset(&client_addr, '0', sizeof(struct sockaddr));
	
	int i;
	//for(i = 0; i < 11; i++)
	{
		int addrlen = sizeof(struct sockaddr);
		len = recvfrom(sockfd, recvBuff, sizeof(recvBuff), 0, (struct sockaddr *)&client_addr, &addrlen);
		
		printf("\tMessage size %i bytes read.\nudp_latency_test(): Returning Message\n", (int) strlen(recvBuff)); 
		
		if(sendto(sockfd, recvBuff, strlen(recvBuff), 0, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0)
		{
			printf("\n\tError: Transmission of UDP message failed -- sendBuff: %s",recvBuff); usleep(500 * 1000);
			exit(1);
		} 
		
		printf("udp_latency_test(): Sent message of size %i bytes\n\n", (int) strlen(recvBuff));
	}
}