#include <Ws2tcpip.h>
#include <stdio.h>
#include <winsock2.h>
#include <iostream>
#include <Windows.h>
#include <string>
#include <time.h>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

SOCKET serSocket;
sockaddr_in serAddr,remoteAddr;
const int MAX_SEQ = 1;
const int TIMEOUT = 30000;
const int errorNum = 5;
const int lostNum = 7;
HANDLE mEvent;
int sx = 0;

char Recv[250];
char Recv2[250];

/*
s.ack\s.seq frameNum:15 //16bit
*/

int nextFrameToSend = 0;
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


void Asend() {
	int nAddrLen = sizeof(remoteAddr);
	int nRet = 0;
	int i = 1;
	while (true) {
		string k;
		sx++;
		int temp = i;
		k += (nextFrameToSend + '0');
		char ml[20];
		memset(ml, 0, sizeof(ml));
		string l;
		_itoa_s(i, ml, 2);
		for (int g = 0;g < (15 - strlen(ml));g++) {
			k += '0';
		}
		k += ml;
		
		int p1[32],len=16;
		memset(p1, 0, sizeof(p1));
		memset(r, 0, sizeof(r));
		for (int i = 0;i < 16;i++) {
			p1[i] = k[i] - '0';
		}
		for (int i = 0;i < 16;i++) {
			r[i] = p1[i];
		}
		CRC(p1, len);
		for (int g=0; g < 16; g++){
			k += (r[g] + '0');
		}
		cout <<"to be sent " <<k << endl;
		if (sx % errorNum == 0) {
			srand(time(NULL));
			int temp = rand() % 32;
			if (k[temp] == '0') {
				k[temp] = '1';
			}
			else {
				k[temp] = '0';
			}
		}
		Sleep(2000);
		if (sx % lostNum == 0) {
			ResetEvent(mEvent);
		}
		else {
			nRet = sendto(serSocket, k.c_str(), k.length(), 0, (sockaddr*)&remoteAddr, nAddrLen);
			ResetEvent(mEvent);
		}

		if (nRet == SOCKET_ERROR) {
			cout<< "sendto Error " << WSAGetLastError() << endl;
			break;
		}
		DWORD result = WaitForSingleObject(mEvent, TIMEOUT);
		if (result == WAIT_OBJECT_0) {
			if (Recv2[0] == nextFrameToSend + '0') {
				nextFrameToSend = inc(nextFrameToSend);
				i++;
				cout << "next frame to send " <<nextFrameToSend<< endl;
				continue;
			}
			else {
				cout << " ack error: " << Recv2[0] << " next frame to send error: " << nextFrameToSend << endl;
				continue;
			}
			
		}
		else {
			cout << "received times out" << endl;
		}
	}
}

DWORD WINAPI Arecv(LPVOID l) {
	int nRet = 0;
	int nAddrlen = sizeof(remoteAddr);
	while (true) {
		memset(Recv, 0, sizeof(Recv));
		nRet = recvfrom(serSocket, Recv, 250, 0, (sockaddr*)&remoteAddr, &nAddrlen);
		if (nRet == SOCKET_ERROR) {
			cout<< "recvfrom Error " << WSAGetLastError() << endl;
			break;
		}
		else if(nRet==0){
			break;
		}
		else {
			for (int k = 0;k < 250;k++) {
				Recv2[k] = Recv[k];
			}
			cout << "ack get "<<Recv2[0] << endl;
			SetEvent(mEvent);
		}
	}
	return 0;
}

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;
	}
	serSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (serSocket == INVALID_SOCKET)
	{
		printf("socket error !");
		return 0;
	}
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &serAddr.sin_addr.S_un.S_addr);
	//serAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(serSocket, (sockaddr*)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		printf("bind error !");
		closesocket(serSocket);
		return 0;
	}
	int nAddrLen = sizeof(remoteAddr);
	char recvData[255];
	int ret = recvfrom(serSocket, recvData, 255, 0, (sockaddr*)&remoteAddr, &nAddrLen);
	if (ret > 0)
	{
		recvData[ret] = 0x00;
		char str[100];
		memset(str, 0, sizeof(str));
		inet_ntop(AF_INET, &remoteAddr.sin_addr, str, sizeof(str));
		cout << "connection detected: " << str << " port: " << remoteAddr.sin_port << endl;
	}
	sendto(serSocket, "server to client", strlen("server to client"), 0, (sockaddr*)&remoteAddr, nAddrLen);


	mEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	CreateThread(NULL, 0, Arecv, NULL, 0, NULL);
	Asend();

	closesocket(serSocket);
	WSACleanup();
	return 0;
}
