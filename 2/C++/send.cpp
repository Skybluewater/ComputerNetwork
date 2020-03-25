/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std;

int CRC_CCITT[16]={0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,1};
int r[16];

void CRC(int p[],int len){
    int c=1;
    do{
        if(r[0]==1){
            for(int i=0;i<15;i++){
                r[i]=r[i+1];
            }
            r[15]=p[15+c];
            for(int i=0;i<16;i++){
                r[i]=r[i]^CRC_CCITT[i];
            }
        } else{
            for(int i=0;i<15;i++){
                r[i]=r[i+1];
            }
            r[15]=p[15+c];
        }
        c++;
    }while(c<len+1);
}

int main()
{
    int p1[32];
    memset(p1,0,sizeof(p1));
    for(int i=0;i<32;i++){
        char temp;
        cin>>temp;
        p1[i]=temp-'0';
    }
    int p2[48];
    memset(p2, 0, sizeof(p2));
    for(int i=0;i<32;i++){
        p2[i]=p1[i];
    }
    for(int i=0;i<16;i++){
        r[i]=p2[i];
    }
    cout<<"CRC-CCITT生成多项式: 10001000000100001"<<endl<<"CRC-Code： ";
    int len = 32;
    CRC(p2, len);
    for(int i=0;i<16;i++){
        cout<<r[i];
    }
    for(int i=len;i<48;i++){
        p2[i]=r[i-len];
    }
    cout<<endl<<"带校验和的发送帧: ";
    for(int i=0;i<48;i++){
        cout<<p2[i];
    }
    cout<<endl;
}*/
