#include<stdio.h> // printf()
#include<stdlib.h> // exit()
#include<string.h> // strerror()
#include<errno.h> // errno
#include<sys/types.h>
#include<sys/socket.h>// socket()
#include<unistd.h>// close()
#include<netinet/in.h>
#include<arpa/inet.h> // htons, htonl, ntohs, ntohl , inet_pton() 
#include<netdb.h>


typedef int SOCKET;
#define SOCKET_ERROR   -1
#define INVALID_SOCKET -1



void err_quit(const char*msg)
{
	char *msgbuf = strerror(errno);
	printf("[%s] %s\n", msg, msgbuf);
	exit(1);
}

void err_display(const char* msg)
{
	char *msgbuf = strerror(errno);	
	printf("[%s] %s\n", msg, msgbuf);
}

	
	
