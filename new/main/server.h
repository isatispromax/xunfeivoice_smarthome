#ifndef _SERVER_H
#define _SERVER_H

#define MAXLINE		5000
#define OPEN_MAX	100
#define LISTENQ		20
#define SERV_PORT	34952
#define INFTIM		1000

#define BUFFERSIZE 5000

void setnonblocking(int sock);
int net_server();

#endif