//
//  main.cpp
//  CRC
//
//  Created by 崔程远 on 2020/3/24.
//  Copyright © 2020 qc. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <string.h>

using namespace std;

char m[17];

unsigned int sum(char *ptr){
    unsigned int k = 0;
    long long int l = strlen(ptr);
    for(long long int i=0;i<l;i++){
        k=k*2+(ptr[i]-'0');
    }
    return k;
}

unsigned int crc_cal_by_bit(unsigned int ptr)
{
    unsigned int crc = 0;
    unsigned int CRC_CCITT = 0x1021;
    for(unsigned char i = 0x80; i != 0; i /= 2)
    {
        crc *= 2;
        if((crc&0x10000) !=0) //上一位CRC乘 2后，若首位是1，则除以 0x11021
            crc ^= 0x11021;
        if((ptr&i) != 0)    //如果本位是1，那么CRC = 上一位的CRC + 本位/CRC_CCITT
            crc ^= CRC_CCITT;
    }
    return crc;
}

void toBinary(unsigned int pr){
    memset(m, '0', sizeof(m));
    m[16]='\0';
    int g = 15;
    while(pr){
        m[g--]=pr%2+'0';
        pr/=2;
    }
}

int main(int argc, const char * argv[]) {
    char k[33],n[49];
    memset(k,0,sizeof(k));
    cin>>k;
    unsigned int l = sum(k);
    unsigned int g = crc_cal_by_bit(l);
    toBinary(g);
    memset(n, 0, sizeof(n));
    strcat(n,k);
    strcat(n,m);
    cout<<"CRC-CCITT生成多项式: 10001000000100001"<<endl<<"CRC-Code： "<<m<<endl<<"带校验和的发送帧: "<<n<<endl;
    return 0;
}
