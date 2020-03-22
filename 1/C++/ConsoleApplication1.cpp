#include "WzSerialPort.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <WinSock2.h>
#include <windows.h>

using namespace std;

WzSerialPort::WzSerialPort()
{
}

WzSerialPort::~WzSerialPort()
{
}

bool WzSerialPort::open(const char* portname,
	int baudrate,
	char parity,
	char databit,
	char stopbit
)
{

	HANDLE hCom = NULL;

	//同步方式
	hCom = CreateFileA(portname, //串口名
		GENERIC_READ | GENERIC_WRITE, //支持读写
		0, //独占方式，串口不支持共享
		NULL,//安全属性指针，默认值为NULL
		OPEN_EXISTING, //打开现有的串口文件
		0, //0：同步方式，FILE_FLAG_OVERLAPPED：异步方式
		NULL);//用于复制文件句柄，默认值为NULL，对串口而言该参数必须置为NULL

	if (hCom == (HANDLE)-1)
	{
		return false;
	}
	//配置缓冲区大小 
	if (!SetupComm(hCom, 1024, 1024))
	{
		return false;
	}

	// 配置参数 
	DCB p;
	memset(&p, 0, sizeof(p));
	p.DCBlength = sizeof(p);
	p.BaudRate = baudrate; // 波特率
	p.ByteSize = databit; // 数据位

	switch (parity) //校验位
	{
	case 0:
		p.Parity = NOPARITY; //无校验
		break;
	case 1:
		p.Parity = ODDPARITY; //奇校验
		break;
	case 2:
		p.Parity = EVENPARITY; //偶校验
		break;
	}
	p.StopBits = ONESTOPBIT;
	if (!SetCommState(hCom, &p))
	{
		return false;
	}

	//超时处理,单位：毫秒
	//总超时＝时间系数×读或写的字符数＋时间常量
	COMMTIMEOUTS TimeOuts;
	TimeOuts.ReadIntervalTimeout = 1000; //读间隔超时
	TimeOuts.ReadTotalTimeoutMultiplier = 500; //读时间系数
	TimeOuts.ReadTotalTimeoutConstant = 5000; //读时间常量
	TimeOuts.WriteTotalTimeoutMultiplier = 500; // 写时间系数
	TimeOuts.WriteTotalTimeoutConstant = 2000; //写时间常量
	SetCommTimeouts(hCom, &TimeOuts);

	PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);//清空串口缓冲区

	memcpy(pHandle, &hCom, sizeof(hCom));// 保存句柄
	cout << "*****串口初始化完成*****" << endl;
	return true;
}

void WzSerialPort::close()
{
	HANDLE hCom = *(HANDLE*)pHandle;
	CloseHandle(hCom);
}

int WzSerialPort::send(const void* buf, int len)
{
	HANDLE hCom = *(HANDLE*)pHandle;

	// 同步方式
	DWORD dwBytesWrite = len; //成功写入的数据字节数
	BOOL bWriteStat = WriteFile(hCom, //串口句柄
		buf, //数据首地址
		dwBytesWrite, //要发送的数据字节数
		&dwBytesWrite, //DWORD*，用来接收返回成功发送的数据字节数
		NULL); //NULL为同步发送，OVERLAPPED*为异步发送
	if (!bWriteStat)
	{
		return 0;
	}
	return dwBytesWrite;
}

int WzSerialPort::receive(void* buf, int maxlen)
{
	HANDLE hCom = *(HANDLE*)pHandle;
	//同步方式
	DWORD wCount = maxlen; //成功读取的数据字节数
	BOOL bReadStat = ReadFile(hCom, //串口句柄
		buf, //数据首地址
		wCount, //要读取的数据最大字节数
		&wCount, //DWORD*,用来接收返回成功读取的数据字节数
		NULL); //NULL为同步发送，OVERLAPPED*为异步发送
	if (!bReadStat)
	{
		return 0;
	}
	return wCount;
}