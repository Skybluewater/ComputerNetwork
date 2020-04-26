#include <iostream>
#include "WzSerialPort.h"
#include <string.h>

using namespace std;

char parity[10];

char* pari(int Parity) {
	memset(parity, 0, sizeof(parity));
	while (true) {
		switch (Parity) {
		case 0:
			strcpy_s(parity, "无校验");
			return parity;
			break;
		case 1:
			strcpy_s(parity, "奇校验");
			return parity;
			break;
		case 2:
			strcpy_s(parity, "偶校验");
			return parity;
			break;
		default:
			cout << "请重新输入" << endl;
			break;
		}
	}
}

void send(WzSerialPort z) {
	char message[1024];
	while (true) {
		cout << "*****请输入您要输入的字符串*****" << endl;
		memset(message, 0, sizeof(message));
		cin >> message;
		if (strcmp("stop", message) == 0) {
			z.send(message, 1024);
			return;
		}
		z.send(message, 1024);
	}
}

void receive(WzSerialPort z) {
	char message[1024];
	while (true) {
		memset(message, 0, sizeof(message));
		z.receive(message, 1024);
		cout << message << endl;
		if (strcmp(message, "stop") == 0) {
			cout << "*****传送结束*****" << endl;
			return;
		}
	}
}

void sendDemo()
{
	WzSerialPort w;
	char COM1[10];
	int Parity;
	cout << "*****请输入串口*****" << endl;
	cin >> COM1;
	cout << "*****请输入校验位*****" << endl;
	cin >> Parity;
	strcpy_s(parity, pari(Parity));
	if (w.open(COM1, 9600, Parity, 8, 1))
	{
		cout << "*****" << "发送端串口名称: " << COM1 << " 校验方式: " << parity << " 波特率: " << 9600 << " bps 数据位: " << 8 << " 停止位: " << 1 << "*****" << endl;
		send(w);
	}
	else
	{
		cout << "*****串口打开失败*****" << endl;
	}
}

void receiveDemo()
{
	WzSerialPort w;
	char COM2[10];
	int Parity;
	cout << "*****请输入串口*****" << endl;
	cin >> COM2;
	cout << "*****请输入校验位*****" << endl;
	cin >> Parity;
	strcpy_s(parity, pari(Parity));
	if (w.open(COM2, 9600, Parity, 8, 1))
	{
		cout << "*****" << "接收端串口名称: " << COM2 << " 校验方式: " << parity << " 波特率: " << 9600 << " bps 数据位: " << 8 << " 停止位: " << 1 << "*****" << endl;
		receive(w);
	}
	else {
		cout << "*****串口打开失败*****" << endl;
	}
}

int main(int argumentCount, const char* argumentValues[])
{
	// 假设COM1已经和另外一个串口连接好了
	// 发送 demo
	sendDemo();

	// 接收 demo
	// receiveDemo();

	getchar();
	return 0;
}