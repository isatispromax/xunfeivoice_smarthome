
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "msp_cmn.h"
#include "msp_errors.h"
#include "cJSON.h"
#include "server.h"
#include "codecon.h"


void setnonblocking(int sock)
{
	int opts;
	opts = fcntl(sock, F_GETFL);

	if(opts < 0) {
		perror("fcntl(sock, GETFL)");
		exit(1);
	}

	opts = opts | O_NONBLOCK;

	if(fcntl(sock, F_SETFL, opts) < 0) {
		perror("fcntl(sock, SETFL, opts)");
		exit(1);
	}
}

int net_server()
{
	printf("epoll socket begins.\n");
	int 			i, maxi, listenfd, connfd, sockfd, epfd, nfds,homesockfd=0;
	ssize_t 		n;
	char			line[MAXLINE];
	socklen_t	    clilen;
	unsigned int 	str_len       		 = 0;
	struct 			epoll_event ev, events[20];
	int 			ret 				 = 0;
	char			*rec_text  			 = NULL;
	int 			sendValue 			 = 0;
	

	struct 			sockaddr_in clientaddr;
	struct 			sockaddr_in serveraddr;
	
	char 			*recive_buff = NULL;
	char 			*utf8_buff = NULL;
	char 			*send_buff = NULL;
	char			*be		   = NULL;
	char			*ed		   = NULL;
	
	FILE* 			fp_test;
	unsigned int 	MspStatic = 0; //0空闲，1为正在使用；
	epfd = epoll_create(1024);
	recive_buff = (char *)malloc(BUFFERSIZE);
	send_buff = (char *)malloc(BUFFERSIZE);
	utf8_buff = (char *)malloc(BUFFERSIZE);
	be = (char *)malloc(4);
	ed = (char *)malloc(4);
	
	memset(recive_buff,0,BUFFERSIZE);
	memset(send_buff,0,BUFFERSIZE);
	memset(utf8_buff,0,BUFFERSIZE);
	
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd <0){
		perror("creat socket error");
		return -1;
	}
	setnonblocking(listenfd);

	ev.data.fd = listenfd;
	ev.events = EPOLLIN | EPOLLET;

	epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
//	char *local_addr = "192.168.234.139";
//	inet_aton(local_addr, &(serveraddr.sin_addr));
	serveraddr.sin_addr.s_addr  = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERV_PORT);
	
	
	ret = bind(listenfd, (struct sockaddr *)(&serveraddr), sizeof(serveraddr));
	if(ret == -1){
		perror("bind error");
		return -1;
	}
	
	listen(listenfd, LISTENQ);

	maxi = 0;
	clilen = sizeof(struct sockaddr_in);
	for(; ;) {
		nfds = epoll_wait(epfd, events, 500, 500);

		for(i = 0; i < nfds; ++i) {
			if(events[i].data.fd == listenfd) {
				printf("accept connection, fd is %d\n", listenfd);
				connfd = accept(listenfd, (struct sockaddr *)(&clientaddr), &clilen);
				if(connfd < 0) {
					perror("connfd < 0");
					exit(1);
				}

				setnonblocking(connfd);

				char *str = inet_ntoa(clientaddr.sin_addr);
				printf("connect from %s\n", str);

				ev.data.fd = connfd;
				ev.events = EPOLLIN | EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
			}
			else if(events[i].events & EPOLLIN) {
				if((sockfd = events[i].data.fd) < 0) continue;
				if((n = read(sockfd, recive_buff, MAXLINE)) < 0) {
					if(errno == ECONNRESET) {
						close(sockfd);
						events[i].data.fd = -1;
					} else {
						printf("readline error");
					}
				} else if(n == 0) {
					close(sockfd);
					events[i].data.fd = -1;
				}
				printf("received n: %d\n", n);
				//printf("received data: %x\n", recive_buff);
				str_len = (unsigned int)n;
				strncpy(be,recive_buff,3);
				if(strcmp(be,"127") == 0){
					homesockfd = sockfd;
				}
				//recive_buff[n] = "\0";
				
				//g2u(recive_buff,(size_t)str_len,utf8_buff,(size_t)BUFFERSIZE);
				
				//fp_test = fopen("recivebuff.txt","wb");
	
				//utf8_buff[strlen(utf8_buff)]="\0";
				//fwrite(utf8_buff,1,strlen(utf8_buff),fp_test);
				//printf("utf8_text %s\n",utf8_buff);
				//fclose(fp_test);
				/*
				if(!MspStatic)
				{
					MspStatic = 1;
					rec_text = MSPSearch("nlp_version=2.0",recive_buff,&str_len,&ret);
					
					if(MSP_SUCCESS !=ret)
					{
						printf("MSPSearch failed ,error code is:%d\n",ret);
					}
					else{
						sendValue = json_to_code(rec_text);
					}
					MspStatic = 0;
				}else
				{
					printf("Msp Busy!\n");
					
				}*/
				
				
				sprintf(send_buff,"%d",sendValue);
				printf("sendValue data: %d\n", sendValue);
				
				//recive_buff[1] = (char)(sendValue);
				//设置服务器为发送状态
				if(homesockfd !=0){
					ev.data.fd = homesockfd;
					ev.events = EPOLLOUT | EPOLLET;
					epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
				}
			}
			else if(events[i].events & EPOLLOUT) {
				sockfd = events[i].data.fd;
				write(sockfd, (void *)&recive_buff, sizeof(recive_buff));

				printf("written data: %s\n", recive_buff);

				ev.data.fd = sockfd;
				ev.events = EPOLLIN | EPOLLET;
				epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
			}
		}
	}
}


