// -*- coding: utf-8 -*-
// 模拟CHAP校验

#include <openssl/md5.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MD5len 16
#define StrLen 32
typedef unsigned char Byte;
typedef unsigned char* Bytearr;

//整数转化为4bytes的byteArray
Bytearr itobarr(int64_t in) {
  Bytearr arr = (Bytearr)malloc(sizeof(in));
  int i;
  for(i=0; i < (int)sizeof(int64_t); i++)
    {
      arr[i]=(Byte)in;
      in>>=8;
    }
  return arr;
}

//生成随机的附加段
Bytearr genRandNum() {
  unsigned int r;
  srand((unsigned)time(NULL));
  r = rand();
  Bytearr randBA = itobarr(r);
  return randBA;
}

//检测MD5是否匹配 是1 否0
int checkMD5key(Bytearr MD5key, Bytearr Rightkey) {
  int i;
  for(i=0; i<MD5len; i++) {
    if(MD5key[i] != Rightkey[i])
      return 0;
  }
  return 1;
}

//生成口令和附加段组合后的MD5
Bytearr genMD5(Bytearr RandNum, char* key) {
  MD5_CTX ctx;
  Bytearr md5 = (Bytearr)malloc(MD5len);
  MD5_Init(&ctx);
  MD5_Update(&ctx, key, strlen(key));
  MD5_Update(&ctx, RandNum, sizeof(int64_t));
  MD5_Final(md5, &ctx);

  return md5;
}

//16进制打印ByteArray
int printBytearrHex(Bytearr arr, int len) {
  int i;
  for(i=0; i<len; i++) {
    printf("%02x",arr[i]);
  }
  return i;
}

int main(int argc, char *argv[]) {
  if(argc != 2) {
    printf("Wrong Args Num!\n");
    return 0;
  }

  char str[StrLen+1];
  Bytearr md5,md5key,randNum = genRandNum();

  printf("Key:\t\t%s\n", argv[1]);//预设口令

  printf("Please input key:\n");
  scanf("%s",str);//输入口令
  printf("Input key:\t%s\n", str);

  printf("Random Num:\t");
  printBytearrHex(randNum, sizeof(int64_t));//随机附加段
  printf("\n");

  md5 = genMD5(randNum, str);//模拟被验证方生成输入口令和附加段MD5
  md5key = genMD5(randNum, argv[1]);//验证方生成预设口令和附加段MD5
  printf("MD5\t\t");
  printBytearrHex(md5, MD5len);//打印输入口令和附加段MD5
  printf("\n");

  printf("Check Answer:\t");
  if(checkMD5key(md5, md5key))//口令检测结果
    printf("True\n");
  else printf("False\n");

  free(md5);
  free(md5key);
  free(randNum);
  return 0;
}
