//
//  main.cpp
//  DV_Algorimth
//
//  Created by 崔程远 on 2020/4/22.
//  Copyright © 2020 qc. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <queue>

using namespace std;

int routers;
int vec[10][10];//两点距离(路由表的部分)
int dist[10][10];//边
int rout_table[10][10];//路由表
int rout_table2[10][10];
typedef struct {
    int rout_table[10][10];//路由表
}router;

queue<int> q;
queue<router> q2;

int relax(int d1,int d2){
    return d1+d2;
}

void prnt();

void compute(){
    while(!q.empty()){
        int cur=q.front();
        int flag=0;
        q.pop();
        /*router=q2.front();
        for(int i=0;i<10;i++){
            for(int j=0;j<10;j++){
                vec[i][j]=router.rout_table[i][j];
            }
        }
        q2.pop();*/
        cout<<"current updating router: "<<cur<<endl;
        int temp[10];
        memset(temp,-1,sizeof(temp));
        temp[cur]=0;
        for(int l=0;l<routers;l++){
            if(dist[cur][l]>0){
                for(int k=0;k<routers;k++){
                    if(vec[l][k]>=0){
                        int disl = relax(dist[cur][l],vec[l][k]);
                        if(temp[k]==-1||temp[k]>disl){
                            temp[k]=disl;
                            rout_table[cur][k]=l;
                        }
                    }
                }
            }
        }
        for(int i=0;i<routers;i++){
            if(temp[i]!=vec[cur][i]){
                flag=1;
            }
            vec[cur][i]=temp[i];
            rout_table2[cur][i]=temp[i];
            /*for(int j=0;j<10;j++){
                router.rout_table[i][j]=vec[i][j];
            }*/
        }
        if(flag==1){
            for(int l=0;l<routers;l++){
                if(dist[cur][l]>0){
                    q.push(l);
                    //q2.push(router);
                }
            }
        }
    }
    prnt();
}

void prnt(){
    cout<<"routing table"<<endl;
    for(int i=0;i<routers;i++){
        cout<<"table "<<i<<endl;
        for(int k=0;k<routers;k++){
            cout<<"from "<<i<<" to "<<k<<": "<<rout_table[i][k]<<" dist:"<<rout_table2[i][k]<<endl;
        }
    }
}

int main(int argc, const char * argv[]) {
    cin>>routers;
    for(int i=0;i<10;i++){
        for(int k=0;k<10;k++){
            dist[i][k]=dist[k][i]=-1;
        }
    }
    memset(rout_table,-1,sizeof(rout_table));
    memset(rout_table2,-1,sizeof(rout_table2));
    memset(vec,-1,sizeof(vec));
    for(int i=0;i<routers;i++){
        for(int k=i;k<routers;k++){
            int temp;
            cin>>temp;
            if(temp==99){
                vec[i][k]=vec[k][i]=-1;
                dist[i][k]=-1;
                dist[k][i]=-1;
            }else{
                vec[i][k]=vec[k][i]=temp;
                dist[i][k]=temp;
                dist[k][i]=temp;
                rout_table[i][k]=k;
                rout_table[k][i]=i;
            }
        }
    }
    router router;
    for(int i=0;i<10;i++){
        for(int j=0;j<10;j++){
            router.rout_table[i][j]=vec[i][j];
        }
    }
    for(int i=0;i<routers;i++){
        q.push(i);
    }
    compute();
    int i,j,weight;
    cout<<"please enter the weight you wants to change (i,j,weight): ";
    cin>>i>>j>>weight;
    dist[i][j]=dist[j][i]=weight;
    vec[i][j]=weight;
    q.push(i);
    q.push(j);
    compute();
    return 0;
}
