// -*- coding: utf-8 -*-
// UDP通信

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include <ctype.h>

#define BUFF_LEN 1024

int main(int argc, char** argv){//接收参数 本地开放端口
    int listenfd, connfd;
    struct sockaddr_in servaddr;
    char buff[BUFF_LEN];
    int n;

    if(argc != 2)
      return 1;


    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
        printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
        return 0;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));//端口号

    if( bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
        printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
        return 0;
    }

    if( listen(listenfd, 10) == -1){
        printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
        return 0;
    }

    printf("Waiting for message\n");
    if( (connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1){
      printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
      return 0;
    }

    n = recv(connfd, buff, BUFF_LEN, 0);
    buff[n] = '\0';

    printf("Message recived\n");
    printf("Message:%s\n",buff);

    char str[BUFF_LEN];
    strcpy(str, buff);
    int len = strlen(str);
    for(int i = 0; i < len; i++) {
      str[i] = toupper(str[i]);
    }

    printf("Sending message\n");
    send(connfd,str,sizeof(str),0);
    printf("Message sent\n");

    close(connfd);
    close(listenfd);
    return 0;
}
