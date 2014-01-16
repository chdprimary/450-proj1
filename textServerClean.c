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
        if ( ((argv[0] == "./textServer" || argv[0] == "textServer") && argv[1] == "--help") || argc != 2 )
        {
                printf("\nUsage: ./textServer PORT_NUM\n");
                printf("\tListens for text messages on PORT_NUM and displays them on stdout.\n\n");
                exit(1);
        }
}

int tcp_setup(int PORT, int sockfd, struct sockaddr_in serv_addr)
{
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
        {
            perror("setup(): TCP Socket error");
            exit(1);
        }
       
        serv_addr.sin_family                 = AF_INET;
        serv_addr.sin_addr.s_addr            = htonl(INADDR_ANY);
        serv_addr.sin_port                   = htons(PORT);
		bzero(&(serv_addr.sin_zero),8);
        
        if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
			exit(1);

        return sockfd;
}

void tcp_listen(int sockfd)
{
    if(listen(sockfd,10) == -1) { perror("\tFailed to listen\n"); exit(1); }
}

void tcp_latency_test(int sockfd)
{
	char recvBuff[1024];
	memset(recvBuff, '\0', sizeof(recvBuff));
    int n, m;
    int connfd = 0;
    int i, x;

	connfd = accept(sockfd, (struct sockaddr*)NULL,NULL);
	n = read(connfd, recvBuff, sizeof(recvBuff)-1) < 0;
	
	if (n < 0)
		exit(1);
	
	if (m = write(connfd, recvBuff, strlen(recvBuff)) < 0)
		printf("\ntcp_latency_test(): Error resending message\n"); 
		
	memset(recvBuff, '\0', sizeof(recvBuff));
}

int udp_setup(int PORT, int sockfd, struct sockaddr_in serv_addr)
{
	serv_addr.sin_family                 = AF_INET;
    serv_addr.sin_addr.s_addr            = htonl(INADDR_ANY);
    serv_addr.sin_port                   = htons(PORT);
	bzero(&(serv_addr.sin_zero),8);
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
        exit(1);
    
	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0) 
		exit(1);
		
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
	int addrlen = sizeof(struct sockaddr);
	len = recvfrom(sockfd, recvBuff, sizeof(recvBuff), 0, (struct sockaddr *)&client_addr, &addrlen);
	
	
	if(sendto(sockfd, recvBuff, strlen(recvBuff), 0, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0)
		exit(1);
}

