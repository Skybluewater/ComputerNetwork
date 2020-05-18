#include <stdio.h>
#include <Winsock2.h>
#include <string>
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <time.h>
using namespace std;
#pragma warning(disable:4996)

class UDPSender
{
private:
	SOCKET Sock;
	SOCKADDR_IN receiverAddress;
	int receiverPort = 8888; //接收端端口号

	int dataLen = 20; //数据帧长度
	int sendFrameLen = 40; //发送帧长度
	int receiveFrameLen = 1; //接收帧长度
	int serialPos = 0; //序列号位置
	int dataStartPos = 1; //数据帧起始位置
	int validDataLenPos = 21; //有效数据长度的位置
	int crcStartPos = 22; //16位CRC校验码的起始位置
	int crcLen = 16; //CRC检验码的长度
	int isEndPos = 38; //文件是否发送完毕的标识位置

	int nextFrameToSend = 0;
	long seq = 0; //数据帧的编号
	long filterSeq = 0; //发送帧的编号
	int filterError = 10; //每10帧1帧出错
	int filterLost = 10; //每10帧1帧丢失
	int firstError = 3; //第一个出错发送帧的编号
	int firstLost = 8;  //第一个丢失发送帧的编号

	//三种处理模式的代号
	const int right = 0; 
	const int error = 1;
	const int lost = 2;

public:
	//获得单个字符的八位二进制码
	string getSingleBinaryString(int a)
	{
		char s1[10];
		_itoa_s(a, s1, 2);
		string s2(s1);
		while (s2.length() < 8)
		{
			s2 = "0" + s2;
		}
		return s2;
	}

	//获得字符串中每个字符的八位二进制码组合而成的二进制字符串
	string getBinaryString(string source)
	{
		string s = "";
		for (int i = 0; i < source.length(); i++)
		{
			s += getSingleBinaryString(int(source[i]));
		}
		return s;
	}

	//计算余数
	string getRemainderStr(string dividendStr, string divisorStr)
	{
		int dividendLen = dividendStr.length();
		int divisorLen = divisorStr.length();
		for (int i = 0; i < divisorLen - 1; i++)
		{
			dividendStr += "0";
		}
		for (int i = 0; i < dividendLen; i++)
		{
			if (dividendStr[i] == '1')
			{
				dividendStr[i] = '0';
				for (int j = 1; j < divisorLen; j++)
				{
					if (dividendStr[i + j] == divisorStr[j])
					{
						dividendStr[i + j] = '0';
					}
					else
					{
						dividendStr[i + j] = '1';
					}
				}
			}
		}
		string remainderStr = dividendStr.substr(dividendLen, divisorLen);
		return remainderStr;
	}

	string getCRCString(string s)
	{
		string gxStr = "10001000000100001";
		return getRemainderStr(s, gxStr);
	}

	void printTime()
	{
		SYSTEMTIME time;
		GetLocalTime(&time);
		printf("Current time: %4d-%02d-%02d %02d:%02d:%02d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	}

	void Print(int method)
	{
		printTime();
		cout << "next_frame_to_send: " << nextFrameToSend << endl;
		cout << "seq: " << seq << endl;
		if (method == right)
		{
			cout << "Simulate sending right." << endl;
		}
		else if (method == error)
		{
			cout << "Simulate sending wrong." << endl;
		}
		else if (method == lost)
		{
			cout << "Simulate sending lost." << endl;
		}
		cout << endl;
	}

	//判断超时间隔内是否收到确认帧
	bool waitForEvent()
	{
		char *receiveFrame = new char[receiveFrameLen];
		int len = sizeof(SOCKADDR);
		int a = recvfrom(Sock, receiveFrame, 1024, 0, (SOCKADDR*)&receiverAddress, &len);
		if (a <= 0)
		{
			printTime();
			cout << "Reveiving ack overtime, be ready to resend." << endl << endl;
			return false;
		}
		printTime();
		cout << "Received ack, ack is: " << receiveFrame[0] << endl << endl;
		return true;
	}

	void Send()
	{
		//加载套接字库
		WSADATA WSAdata;
		WSAStartup(MAKEWORD(2, 2), &WSAdata);

		//绑定端口
		Sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		receiverAddress.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
		receiverAddress.sin_family = AF_INET;
		receiverAddress.sin_port = htons(receiverPort);
		int len = sizeof(SOCKADDR);

		//设置recvfrom的接收超时为3秒
		timeval tv_out;
		tv_out.tv_sec = 3000;
		tv_out.tv_usec = 0;
		setsockopt(Sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv_out, sizeof(timeval));

		ifstream in("SendText.txt");
		char *data = new char[dataLen];

		bool flag = true;
		int pos = dataLen;
		while (true)
		{
			for (int i = 0; i < dataLen; i++)
			{
				data[i] = '\a';
			}

			in.read(data, dataLen); 

			if (in.eof())//如果已读完文件，需要识别出读的字符个数
			{
				if (flag == false)//最后一个包含数据的帧收到确认帧，发送没有数据的帧，退出循环
				{
					char *sendFrame = new char[sendFrameLen];
					sendFrame[isEndPos] = '1';
					sendto(Sock, sendFrame, strlen(sendFrame), 0, (SOCKADDR*)&receiverAddress, len);
					cout << "Send the file finished." << endl;
					break;
				}
				pos = 0;
				while (data[pos] != '\a')
				{
					pos++;
					continue;
				}
				flag = false;
			}
			seq++;

			//计算CRC校验码
			string s = getBinaryString(string(data).substr(0, pos));
			string crcStr = getCRCString(s);

			//为发送帧赋值
			char *sendFrame = new char[sendFrameLen];
			sendFrame[serialPos] = nextFrameToSend + '0';
			for (int i = 0; i < dataLen; i++)
			{
				sendFrame[i + dataStartPos] = data[i];
			}
			sendFrame[validDataLenPos] = pos;
			for (int i = 0; i < crcLen; i++)
			{
				sendFrame[i + crcStartPos] = crcStr[i];
			}
			sendFrame[isEndPos] = '0';

			bool mark = false;
			while (mark == false)
			{
				//模拟传输出错
				if ((filterSeq - firstError) % filterError == 0)
				{
					char pre = sendFrame[crcStartPos];
					sendFrame[crcStartPos] = '1' - pre + '0';
					sendto(Sock, sendFrame, strlen(sendFrame), 0, (SOCKADDR*)&receiverAddress, len);
					Print(error);
					sendFrame[crcStartPos] = pre;
					filterSeq++;
				}
				//模拟传输丢失
				else if ((filterSeq - firstLost) % filterLost == 0)
				{
					Print(lost);
					filterSeq++;
				}
				//模拟传输正确
				else
				{
					sendto(Sock, sendFrame, strlen(sendFrame), 0, (SOCKADDR*)&receiverAddress, len);
					Print(right);
					filterSeq++;
				}
				// 调节传输速度
				Sleep(1000);

				mark = waitForEvent();
				if (mark == true) 
				{
					nextFrameToSend = (nextFrameToSend + 1) % 2;
				}
			}
		}
		in.close();
		closesocket(Sock);
		WSACleanup();
	}

};

int main()
{
	//需要先打开接收端，否则recefrom不阻塞
	string confirm;
	cout << "Please open UDPReceiver.exe and input yes!" << endl;
	cin >> confirm;
	if (confirm.compare("yes"))
	{
		return 0;
	}
	cout << endl;
	cout << "Be ready to send file..." << endl;
	UDPSender operation;
	operation.Send();
	system("pause");
}