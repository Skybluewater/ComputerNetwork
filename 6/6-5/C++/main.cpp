//
//  main.cpp
//  6-5
//
//  Created by 崔程远 on 2020/6/11.
//  Copyright © 2020 qc. All rights reserved.
//

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <chrono>

using namespace std;

int socket_id;
socklen_t last_src_addrlen;
sockaddr_in last_src;

int stringToInt(string ip) {
    int pr = 0;
    int d = 0;
    long long int len=ip.length();
    for(int k=0;k<len;k++){
        if(ip[k]=='.'){
            pr = pr * 256 + d;
            d = 0;
        }else{
            d=d*10+ip[k]-'0';
        }
    }
    pr = pr * 256 + d;
    return pr;
}

void tcp(int port) {
    socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_id < 0) {
        printf("socket create fail!\n");
    }
    sockaddr_in us{};
    us.sin_family = AF_INET;
    us.sin_addr.s_addr = htonl(INADDR_ANY);
    us.sin_port = htons(port);
    if(::bind(socket_id, (struct sockaddr *) &us, sizeof(us))<0){
        cout<<"sock bind error"<<endl;
    }
}

void connect_to(std::string ip, int port) {
    sockaddr_in dst{};
    dst.sin_family = AF_INET;
    dst.sin_addr.s_addr = htonl(stringToInt(ip));
    dst.sin_port = htons(port);
    cout<<"SYN = 1 ACK = 0"<<endl;
    if (auto cnn = connect(socket_id, (sockaddr *) &dst, sizeof(dst)) < 0) {
        cout << "Failed" <<endl;
    } else {
        shutdown(cnn, 2);
        cout << "SYN = 1 ACK = 1" <<endl;
    }
}

void listen_from() {
    if (listen(socket_id, 5) < 0) {
        cout << "Listen Failed" << std::endl;
    }
    int rqst;
    for (;;) {
        while ((rqst = accept(socket_id,(struct sockaddr *) &last_src, &last_src_addrlen)) < 0) {
            if ((errno != ECHILD) && (errno != EINTR)) {
                perror("accept failed");
                exit(1);
            }
        }
        printf("Connection from: %s port %d\n",inet_ntoa(last_src.sin_addr), ntohs(last_src.sin_port));
        shutdown(rqst, 2);
    }
}

void tcpClose(){
    close(socket_id);
}

int main(int argc, char * const argv[]) {
    string ip = "39.156.66.18";
    int port = 80;
    while (true) {
        tcp(0);
        std::chrono::steady_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        cout << "Ping:" << ip << " Port:" << port << endl;
        connect_to(ip, port);
        std::chrono::steady_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        cout << "RTT: " << (std::chrono::nanoseconds(t2 - t1).count() / 1e6) << "ms" << endl;
        sleep(1.5);
    }
    return 0;
}
