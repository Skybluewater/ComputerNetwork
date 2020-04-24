//
//  main.cpp
//  LS_Algorimth
//
//  Created by 崔程远 on 2020/4/24.
//  Copyright © 2020 qc. All rights reserved.
//

#include <iostream>
#include <stack>

using namespace std;

int routers;
int dist[10][10];//边
int rout_table[10][10][2];//路由表
int a[10][2];
int flag[10];

int main(int argc, const char * argv[]) {
    cin>>routers;
    memset(dist,-1,sizeof(dist));
    memset(rout_table,-1,sizeof(rout_table));
    for(int i=0;i<routers;i++){
        for(int k=i;k<routers;k++){
            int temp;
            cin>>temp;
            if(temp==99){
                dist[i][k]=-1;
                dist[k][i]=-1;
            }else{
                dist[i][k]=temp;
                dist[k][i]=temp;
            }
        }
    }
    for(int i=0;i<routers;i++){
        memset(a,0,sizeof(a));
        memset(flag,0,sizeof(flag));
        a[i][0]=0;
        a[i][1]=i;
        flag[i]=1;
        int x=1;
        int current=i;
        while(x){
            for(int k=0;k<routers;k++){
                if(dist[current][k]>0&&flag[k]==0){
                    if(a[current][0]+dist[current][k]<a[k][0]||a[k][0]==0){
                        a[k][0]=a[current][0]+dist[current][k];
                        a[k][1]=current;
                    }
                }
            }
            int min=1000,min2=0;
            for(int k=0;k<routers;k++){
                if(a[k][0]<min&&flag[k]==0&&a[k][0]>0){
                    min=a[k][0];
                    min2=k;
                }
            }
            if(min==1000){
                x=0;
            }else{
                flag[min2]=1;
                current=min2;
            }
        }
        stack<int> s;
        cout<<"route "<<i<<endl;
        for(int k=0;k<routers;k++){
            s.push(k);
            rout_table[i][k][1]=k;
            int pr=a[k][1];
            while(pr!=i){
                rout_table[i][k][1]=pr;
                s.push(pr);
                pr=a[pr][1];
            }
            s.push(i);
            rout_table[i][k][0]=a[k][0];
            cout<<i<<" to "<<k<<" :";
            while(!s.empty()){
                cout<<s.top()<<" ";
                s.pop();
            }
            cout<<"length: "<<a[k][0]<<endl;
        }
    }
    cout<<"routing table"<<endl;
    for(int i=0;i<routers;i++){
        cout<<"table "<<i<<endl;
        for(int k=0;k<routers;k++){
            cout<<"from "<<i<<" to "<<k<<": "<<rout_table[i][k][1]<<" dist: "<<rout_table[i][k][0]<<endl;
        }
    }
    return 0;
}
