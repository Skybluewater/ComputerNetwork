// -*- coding: utf-8 -*-
// TCP通信

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>
#include<errno.h>


#define BUFF_LEN 1024

int main(int argc, char* argv[])//接收参数 目标IP、目标端口、待处理字符串
{
	int iSocketFD = 0; //socket句柄
	struct sockaddr_in stRemoteAddr = {0}; //即目标地址信息
	char buf[BUFF_LEN] = {0}; //存储接收到的数据
  char str[BUFF_LEN];
  strcpy(str, argv[3]);

  if(argc != 4)
    return 1;

	iSocketFD = socket(AF_INET, SOCK_STREAM, 0); //建立socket
	if(iSocketFD < 0)
    {
      printf("Create socket error: %s(errno: %d)\n",strerror(errno),errno);
      return 0;
    }

	stRemoteAddr.sin_family = AF_INET;
	stRemoteAddr.sin_port = htons(atoi(argv[2]));
	stRemoteAddr.sin_addr.s_addr = inet_addr(argv[1]);

	//连接方法： 传入句柄，目标地址，和大小
	if(connect(iSocketFD, (void *)&stRemoteAddr, sizeof(stRemoteAddr)) < 0)
    {
      printf("Connect error: %s(errno: %d)\n",strerror(errno),errno);
      return 0;
    }

  printf("Connection ready\n");
  printf("Sending Message\n");
  send(iSocketFD, str, BUFF_LEN, 0);
  printf("Message sent\n");
  printf("Waiting for Message\n");
  recv(iSocketFD, buf, BUFF_LEN, 0);
  buf[BUFF_LEN-1] = '\0';
  printf("Message recived\n");
  printf("toUpper:%s\n", buf);

	close(iSocketFD);//关闭socket
	return 0;
}

