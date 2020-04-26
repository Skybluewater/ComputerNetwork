//
//  main.cpp
//  Bytestuffing
//
//  Created by Ccy on 2020/3/26.
//  Copyright © 2020 qc. All rights reserved.
//

#include <iostream>
#include <string.h>
#include <string>

using namespace std;

int main(int argc, const char * argv[]) {
    char k[8][3];
    memset(k,0,sizeof(k));
    string a="7E",b="7E",c;
    cout<<"帧起始标志: "<<b<<" 帧数据信息: ";
    for(int i=0;i<16;i++){
        cin>>k[i/2][i%2];
    }
    char m[20][3];
    memset(m,0,sizeof(m));
    int j=0;
    for(int i=0;i<8;i++){
        if(k[i][0]=='7'&&k[i][1]=='E'){
            m[j][0]='7';m[j][1]='D';
            m[++j][0]='5';m[j][1]='E';
            j++;
        }
        else if(k[i][0]=='7'&&k[i][1]=='D'){
            m[j][0]='7';m[j][1]='D';
            m[++j][0]='5';m[j][1]='D';
            j++;
        }
        else if(k[i][0]<'2'){
            m[j][0]='7';m[j][1]='D';
            m[++j][0]=k[i][0]+2;m[j][1]=k[i][1];
            j++;
        }else{
            m[j][0]=k[i][0];m[j][1]=k[i][1];
            j++;
        }
    }
    for(int i=0;i<j;i++){
        c=c+m[i][0]+m[i][1];
    }
    a.append(c);
    a.append(b);
    cout<<"帧结束标志: "<<b<<" 发送帧: "<<a;
    int num=0;
    string l;
    for(int i=0;i<a.length();){
        if(a[i]=='7'&&a[i+1]=='E'){
            num++;
            if(num>=2){
                break;
            }
            i+=2;
        } else if(a[i]=='7'&&a[i+1]=='D'&&num>0){
            i+=2;
            if(i>a.length()){
                cout<<"error"<<endl;
                break;
            }
            if(a[i]=='5'&&a[i+1]=='D'){
                l.append("7D");
            }else if(a[i]=='5'&&a[i+1]=='E'){
                l.append("7E");
            }else{
                if(a[i]>'3'){
                    cout<<"error"<<endl;
                    break;
                }else{
                    l+=a[i]-2;
                    l+=a[i+1];
                }
            }
            i=i+2;
        } else if(num>0){
            l+=a[i];
            l+=a[i+1];
            i+=2;
        }
    }
    if(num<2){
        cout<<"error"<<endl;
    }
    cout<<endl<<"删除后的接收帧: "<<l<<endl;
    return 0;
}
