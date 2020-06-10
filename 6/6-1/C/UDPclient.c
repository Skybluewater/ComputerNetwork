// -*- coding: utf-8 -*-
// UDP通信

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFF_LEN 1024

int resvMsg(int fd, char** str, struct sockaddr_in *client) {
  socklen_t len;
  int count;
  struct sockaddr_in clent_addr;  //clent_addr用于记录发送方的地址信息

  len = sizeof(clent_addr);
  count = recvfrom(fd, *str, BUFF_LEN, 0, (struct sockaddr*)&clent_addr, &len);  //recvfrom是拥塞函数，没有数据就一直拥塞
  if(count == -1) {
    return -1;
  }
  *client = clent_addr;
  return 0;
}

int sendMsg(int fd, char* str, struct sockaddr_in client) {
  socklen_t len;
  struct sockaddr_in clent_addr = client;  //clent_addr用于记录发送方的地址信息
  len = sizeof(clent_addr);
  sendto(fd, str, BUFF_LEN, 0, (struct sockaddr*)&clent_addr, len);  //发送信息给client，注意使用了clent_addr结构体指针
  return 0;
}

int main(int argc, char* argv[])//接收参数 目标IP、本地开放端口、目标端口、待处理字符串
{
  int client_fd;
  struct sockaddr_in ser_addr, cli_addr;
  char *str;

  if(argc != 5)
    return 1;

  int cli_port = atoi(argv[2]);
  int ser_port = atoi(argv[3]);

  client_fd = socket(AF_INET, SOCK_DGRAM, 0); //AF_INET:IPV4;SOCK_DGRAM:UDP
  if(client_fd < 0)
    {
      printf("create socket fail!\n");
      return 1;
    }

  memset(&cli_addr, 0, sizeof(cli_addr));
  cli_addr.sin_family = AF_INET;
  cli_addr.sin_addr.s_addr = htonl(INADDR_ANY); //IP地址，需要进行网络序转换
  cli_addr.sin_port = htons(cli_port);  //端口号，需要网络序转换

  int ret = bind(client_fd, (struct sockaddr*)&cli_addr, sizeof(cli_addr));
  if(ret < 0)
    {
      printf("socket bind fail!\n");
      return 1;
    }

  str = malloc(BUFF_LEN);

  memset(&ser_addr, 0, sizeof(ser_addr));
  ser_addr.sin_family = AF_INET;
  ser_addr.sin_addr.s_addr = inet_addr(argv[1]); //IP地址，需要进行网络序转换
  ser_addr.sin_port = htons(ser_port);  //端口号，需要网络序转换

  printf("Sending message\n");
  sendMsg(client_fd, argv[4], ser_addr);
  printf("Message sent\n");

  printf("Waiting for message\n");
  resvMsg(client_fd, &str, &ser_addr);
  printf("Message recived\n");
  printf("toUpper:%s\n", str);

  close(client_fd);
  return 0;
}
