#include <Ws2tcpip.h>
#include <stdio.h>
#include <winsock2.h>
#include <iostream>
#include <Windows.h>
#include <string>
#include <time.h>
#include <queue>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

SOCKET serSocket;
sockaddr_in serAddr, remoteAddr;
const int MAX_SEQ = 7;
const int TIMEOUT = 15000;
const int errorNum = 5;
const int lostNum = 7;
HANDLE mEvent, hEvent;
int sx = 0;
int nbuffered = 0;

char Recv[250];
char Recv2[250];

int next_frame_to_send = 0;
int frame_expected = 0;
int ack_expected = 0;
float startTime = 0;
int CRC_CCITT[16] = { 0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,1 };
int r[16];
int timeout = 0;
int allow_to_send = 1;
int i = 0;

struct {
	_SYSTEMTIME a;
	int b;
}condition[MAX_SEQ + 1];

queue<int> q;

int inc(int nd) {
	int temp = (nd + 1) % (MAX_SEQ + 1);
	return temp;
}

bool between(int a, int b, int c) {
	if ((a <= b && b < c) || (c < a && a <= b) || (c < a && b < c)) {
		return TRUE;
	}
	else return FALSE;
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

void sendData() {
	int nAddrLen = sizeof(remoteAddr);
	int nRet = 0;
	string k;
	sx++;
	int pl = next_frame_to_send;//r.seq 0
	int tl = (frame_expected + MAX_SEQ) % (MAX_SEQ + 1);//r.ack 1
	char kl[20];
	char ml[20];
	char hl[20];
	memset(kl, 0, sizeof(kl));
	memset(ml, 0, sizeof(ml));
	memset(hl, 0, sizeof(hl));
	string l;
	cout << "sending" << endl;
	_itoa_s(pl, kl, 2);
	_itoa_s(i, ml, 2);
	_itoa_s(tl, hl, 2);
	cout << "next frame to send " << pl << endl << "ack expected " << tl << endl << "frame info " << i << endl;
	for (int g = 0;g < (3 - strlen(kl));g++) {
		k += '0';
	}
	k += kl;
	for (int g = 0;g < (3 - strlen(hl));g++) {
		k += '0';
	}
	k += hl;
	for (int g = 0;g < (10 - strlen(ml));g++) {
		k += '0';
	}
	k += ml;

	int p1[32], len = 16;
	memset(p1, 0, sizeof(p1));
	memset(r, 0, sizeof(r));
	for (int l = 0;l < 16;l++) {
		p1[l] = k[l] - '0';
	}
	for (int l = 0;l < 16;l++) {
		r[l] = p1[l];
	}
	CRC(p1, len);
	for (int g = 0; g < 16; g++) {
		k += (r[g] + '0');
	}

	//cout << "to be sent: " << k << endl;
	cout << endl;
	GetSystemTime(&condition[next_frame_to_send].a);
	condition[next_frame_to_send].b = 1;
	next_frame_to_send = inc(next_frame_to_send);
	//condition[next_frame_to_send][0] = 1;
	//condition[next_frame_to_send][1] = GetTickCount();
	if (sx % 10 == 0) {
		srand(time(NULL));
		int x = rand() % 32;
		if (k[x] == '0') {
			k[x] = '1';
		}
		else {
			k[x] = '0';
		}
	}
	if (sx % 15 == 0) {
		ResetEvent(mEvent);
	}
	else{
		nRet = sendto(serSocket, k.c_str(), k.length(), 0, (sockaddr*)&remoteAddr, nAddrLen);
		ResetEvent(mEvent);
		if (nRet == SOCKET_ERROR) {
			cout << "sendto Error " << WSAGetLastError() << endl;
		}
	}
	/*nRet = sendto(serSocket, k.c_str(), k.length(), 0, (sockaddr*)&remoteAddr, nAddrLen);
	if (nRet == SOCKET_ERROR) {
		cout << "sendto Error " << WSAGetLastError() << endl;
	}*/
}

void Protocol5() {
	while (true) {
		allow_to_send = 1;
		if (nbuffered == MAX_SEQ) {
			allow_to_send = 0;
		}
		if (timeout == 1) {
			next_frame_to_send = ack_expected;
			timeout = 0;
			i = q.front();
			while (!q.empty()) {
				q.pop();
			}
			nbuffered = 0;
			allow_to_send = 1;
			for (int g = 0;g <= MAX_SEQ;g++) {
				condition[g].b = 0;
			}
		}
		if (allow_to_send) {
			sendData();
			nbuffered += 1;
			q.push(i);
			i++;
			//cout << "next frame to send " << next_frame_to_send << " frame expected " << frame_expected << " ack expected " << ack_expected << endl;
			if (nbuffered == MAX_SEQ) {
				allow_to_send = 0;
				cout <<endl<< "buffer full" <<endl<<endl;
			}
			else {
				allow_to_send = 1;
			}
			Sleep(3000); 
		}
		else {
			//cout << "next frame to send " << next_frame_to_send << " frame expected " << frame_expected << " ack expected " << ack_expected << endl;
			cout<<endl<< "not allowed to send" <<endl<<endl;
			Sleep(3000);
		}
	}
}

DWORD WINAPI Atime(LPVOID L) {
	while (true) {
		_SYSTEMTIME now;
		GetSystemTime(&now);
		for (int k = 0;k <= MAX_SEQ;k++) {
			if (condition[k].b == 1) {
				float waitTime = (now.wMilliseconds - condition[k].a.wMilliseconds) + (now.wSecond - condition[k].a.wSecond) * 1000;
				if (waitTime >= TIMEOUT) {
					timeout = 1;
					cout <<endl<< "timeout" <<endl<< endl;
					memset(condition, 0, sizeof(condition));
					break;
				}
			}
		}
		Sleep(1000);
	}
}

DWORD WINAPI Arecv(LPVOID L) {
	int nRet = 0;
	int nAddrlen = sizeof(remoteAddr);
	while (true) {
		memset(Recv, 0, sizeof(Recv));
		nRet = recvfrom(serSocket, Recv, 250, 0, (sockaddr*)&remoteAddr, &nAddrlen);
		if (nRet == SOCKET_ERROR) {
			cout << "recvfrom Error " << WSAGetLastError() << endl;
			break;
		}
		else if (nRet == 0) {
			break;
		}
		int cksum_error = 0;
		int flag = 0;
		int len = 32;
		int p2[48];
		cout << "receiving" << endl;
		memset(p2, 0, sizeof(p2));
		memset(r, 0, sizeof(r));
		for (int i = 0;i < 32;i++) {
			p2[i] = Recv[i] - '0';
		}
		//cout << "frame received: " << Recv << endl;
		for (int k = 0;k < 16;k++) {
			r[k] = p2[k];
		}
		CRC(p2, len);
		for (int k = 0;k < 16;k++) {
			if (r[k] != 0) {
				cksum_error = 1;
				break;
			}
		}
		if (cksum_error) {
			cout << "transmission error" << endl<<endl;
			continue;
		}
		else {
			for (int k = 0;k < 250;k++) {
				Recv2[k] = Recv[k];
			}
			int sum1 = 0, sum2 = 0,sum=0;
			for (int g = 0;g < 3;g++) {
				sum2 = sum2 * 2 + p2[g];
			}
			for (int g = 3;g < 6;g++) {
				sum1 = sum1 * 2 + p2[g];
			}
			for (int g = 6;g < 16;g++) {
				sum = sum * 2 + p2[g];
			}
			cout << "frame expected get " << sum2 << endl;
			cout << "ack get " << sum1 << endl;
			cout << "frame info get " << sum << endl;
			if (sum2 == frame_expected) {
				frame_expected = inc(frame_expected);
			}
			else {
				cout << "not the expected frame" << endl;
			}
			Sleep(100);
			while (between(ack_expected, sum1, next_frame_to_send)) {
				nbuffered = nbuffered - 1;
				condition[ack_expected].a = { 0 };
				condition[ack_expected].b = 0;
				//condition[ack_expected][1] = 0;
				ack_expected = inc(ack_expected);
				//cout << "buffer poped " << q.front() << endl;
				q.pop();
			}
			cout<<endl;
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
	inet_pton(AF_INET, "127.0.0.1", &serAddr.sin_addr.S_un.S_addr);
	serAddr.sin_port = htons(8888);
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
		cout << recvData << endl;
		memset(str, 0, sizeof(str));
		inet_ntop(AF_INET, &remoteAddr.sin_addr, str, sizeof(str));
		cout << "connection detected: " << str << " port: " << remoteAddr.sin_port <<endl<<endl;
	}
	sendto(serSocket, "server to client", strlen("server to client"), 0, (sockaddr*)&remoteAddr, nAddrLen);
	memset(condition, 0, sizeof(condition));
	mEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	ResetEvent(mEvent);
	CreateThread(NULL, 0, Atime, NULL, 0, NULL);
	CreateThread(NULL, 0, Arecv, NULL,0, NULL);
	Protocol5();

	closesocket(serSocket);
	WSACleanup();
	system("pause");
	return 0;
}
