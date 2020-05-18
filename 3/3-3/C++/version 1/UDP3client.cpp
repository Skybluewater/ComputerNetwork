#include <Ws2tcpip.h>
#include <stdio.h>
#include <winsock2.h>
#include <iostream>
#include <string>

using namespace std;

#pragma comment(lib, "ws2_32.lib") 

SOCKET sclient;
sockaddr_in serv;

char recvData[250];

int frameExpected = 0;
int CRC_CCITT[16] = { 0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,1 };
int r[16];

int inc(int nd) {
	int temp = (nd + 1) % 2;
	return temp;
}

void CRC(int p[], int len) {
	int c = 1;
	do {
		if (r[0] == 1) {
			for (int i = 0;i < 15;i++) {
				r[i] = r[i + 1];
			}
			r[15] = p[15 + c];
			for (int i = 0;i < 16;i++) {
				r[i] = r[i] ^ CRC_CCITT[i];
			}
		}
		else {
			for (int i = 0;i < 15;i++) {
				r[i] = r[i + 1];
			}
			r[15] = p[15 + c];
		}
		c++;
	} while (c < len + 1);
}

void Rec() {
	int nRet = 0,len = 32;
	int nAddrLen = sizeof(serv);
	int p2[48];
	int flag;
	int sum;
	while (true) {
		flag = 0;
		memset(recvData, 0, sizeof(recvData));
		nRet = recvfrom(sclient, recvData, 250, 0, (sockaddr*)&serv, &nAddrLen);
		if (nRet == SOCKET_ERROR) {
			cout << "recvfrom Error " << WSAGetLastError() << endl;
			break;
		}
		else if (nRet == 0) {
			break;
		}
		else {
			//cout << recvData << endl;
			memset(p2, 0, sizeof(p2));
			for (int i = 0;i < 32;i++) {
				p2[i] = recvData[i] - '0';
			}
			for (int i = 0;i < 16;i++) {
				r[i] = p2[i];
			}
			CRC(p2, len);
			for (int i = 0;i < 16;i++) {
				if (r[i] != 0) {
					flag = 1;
					break;
				}
			}
			if (flag == 1||recvData[0]-'0'!=frameExpected) {
				char t = 1 - frameExpected + '0';
				string k;
				k = k+t;
				cout << recvData <<" error"<<endl;
				if (flag == 0) {
					cout << "deduplicate" << endl;
				}
				else { 
					cout << "transmission error" << endl;
				}
				cout << "ack: " << k << endl;
				Sleep(1000);
				sendto(sclient, k.c_str(), k.length(), 0,(sockaddr*)&serv, nAddrLen);
			}
			else if (recvData[0] - '0' == frameExpected) {
				cout << recvData <<" correct"<< endl;
				sum = 0;
				for (int i = 1;i <16;i++) {
					sum = sum * 2 + (recvData[i] - '0');
				}
				cout << "frame accepted: " << sum << endl;
				frameExpected = inc(frameExpected);
				char t = 1 - frameExpected + '0';
				string k;
				k += t;
				cout << "ack: " << k << endl;
				Sleep(1000);
				sendto(sclient, k.c_str(), k.length(), 0, (sockaddr*)&serv, nAddrLen);
			}
		}
	}
}

int main(int argc, char* argv[])
{
	WORD socketVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(socketVersion, &wsaData) != 0)
	{
		return 0;
	}
	sclient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	serv.sin_family = AF_INET;
	serv.sin_port = htons(8888);
	inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr.S_un.S_addr);
	//sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int len = sizeof(serv);
	sendto(sclient, "client to server", strlen("client to server"), 0, (sockaddr*)&serv, len);
	
	memset(recvData, 0, sizeof(recvData));
	int ret = recvfrom(sclient, recvData, 255, 0, (sockaddr*)&serv, &len);
	cout << recvData << endl;
	Rec();

	system("pause");
	closesocket(sclient);
	WSACleanup();
	return 0;
}