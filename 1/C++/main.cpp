#include <iostream>
#include "WzSerialPort.h"
#include <string.h>
using namespace std;

void sendDemo()
{
	WzSerialPort w;
	char COM1[10];
	int Parity;
	cout << "*****请输入串口*****" << endl;
	cin >> COM1;
	cout << "*****请输入校验位*****" << endl;
	cin >> Parity;
	if (w.open(COM1, 9600, Parity, 8, 1))
	{
		for (int i = 0;i < 10;i++)
		{
			w.send("helloworld", 10);
		}
		cout << "send demo finished...";
	}
	else
	{
		cout << "*****串口打开失败*****"<< endl;
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
	if (w.open(COM2, 9600, Parity, 8, 1))
	{
		char buf[1024];
		while (true)
		{
			memset(buf, 0, 1024);
			w.receive(buf, 1024);
			cout << buf;
		}
	}
	else {
		cout << "*****串口打开失败*****" << endl;
	}
}

int main(int argumentCount, const char* argumentValues[])
{
	// 假设COM1已经和另外一个串口连接好了
	// 发送 demo
	//sendDemo();

	// 接收 demo
	receiveDemo();

	getchar();
	return 0;
}