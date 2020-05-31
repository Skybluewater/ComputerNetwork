#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<Winsock2.h>
#include<ws2tcpip.h>
#include<stdlib.h>
#include<malloc.h>
#include<string.h>
#include<windows.h>

#pragma comment(lib , "Ws2_32.lib")
#define ICMP_ECHO_REQUEST 8 //定义回显请求类型
#define DEF_ICMP_DATA_SIZE 20 //定义发送数据长度
#define DEF_ICMP_PACK_SIZE 32 //定义数据包长度
#define MAX_ICMP_PACKET_SIZE 1024 //定义最大数据包长度
#define DEF_ICMP_TIMEOUT 3000  //定义超时为3秒
#define ICMP_TIMEOUT 11 //ICMP超时报文
#define ICMP_ECHO_REPLY 0 //定义回显应答类型
/*
 *IP报头结构
 */
typedef struct
{
	byte h_len_ver; //IP版本号
	byte tos; // 服务类型
	unsigned short total_len; //IP包总长度
	unsigned short ident; // 标识
	unsigned short frag_and_flags; //标志位
	byte ttl; //生存时间
	byte proto; //协议
	unsigned short cksum; //IP首部校验和
	unsigned long sourceIP; //源IP地址
	unsigned long destIP; //目的IP地址
} IP_HEADER;
/*
 *定义ICMP数据类型
 */
typedef struct _ICMP_HEADER
{
	byte type; //类型-----8
	byte code; //代码-----8
	unsigned short cksum; //校验和------16
	unsigned short id; //标识符-------16
	unsigned short seq; //序列号------16
	unsigned int choose; //选项-------32
} ICMP_HEADER;
typedef struct
{
	int usSeqNo; //记录序列号
	DWORD dwRoundTripTime; //记录当前时间
	byte ttl; //生存时间
	in_addr dwIPaddr; //源IP地址
} DECODE_RESULT;
/*
 *产生网际校验和
 */
unsigned short GenerateChecksum(unsigned short *pBuf, int iSize)
{
	unsigned long cksum = 0; //开始时将网际校验和初始化为0
	while (iSize > 1)
	{
		cksum += *pBuf++; //将待校验的数据每16位逐位相加保存在cksum中
		iSize -= sizeof(unsigned short); //每16位加完则将带校验数据量减去16
	}
	//如果待校验的数据为奇数，则循环完之后需将最后一个字节的内容与之前结果相加
	if (iSize)
	{
		cksum += *(unsigned char*)pBuf;
	}
	//之前的结果产生了进位，需要把进位也加入最后的结果中
	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >> 16);
	return (unsigned short)(~cksum);
}
/*
 *对ping应答信息进行解析
 */
SYSTEMTIME currentTime;
boolean DecodeIcmpResponse_Ping(char *pBuf, int iPacketSize, DECODE_RESULT *stDecodeResult)
{
	IP_HEADER *pIpHrd = (IP_HEADER*)pBuf;
	int iIphedLen = 20;
	if (iPacketSize < (int)(iIphedLen + sizeof(ICMP_HEADER)))
	{
		printf("size error! \n");
		return 0;
	}
	//指针指向ICMP报文的首地址
	ICMP_HEADER *pIcmpHrd = (ICMP_HEADER*)(pBuf + iIphedLen);
	unsigned short usID, usSeqNo;
	//获得的数据包的type字段为ICMP_ECHO_REPLY，即收到一个回显应答ICMP报文
	if (pIcmpHrd->type == ICMP_ECHO_REPLY)
	{
		usID = pIcmpHrd->id;
		//接收到的是网络字节顺序的seq字段信息 ， 需转化为主机字节顺序
		usSeqNo = ntohs(pIcmpHrd->seq);
	}
	if (usID != GetCurrentProcessId() || usSeqNo != stDecodeResult->usSeqNo)
	{
		printf("usID error!\n");
		return 0;
	}
	//记录对方主机的IP地址以及计算往返的时延RTT
	GetSystemTime(&currentTime);
	if (pIcmpHrd->type == ICMP_ECHO_REPLY)
	{
		stDecodeResult->dwIPaddr.s_addr = pIpHrd->sourceIP;
		stDecodeResult->ttl = pIpHrd->ttl;
		stDecodeResult->dwRoundTripTime = currentTime.wMinute * 3600 + currentTime.wSecond * 60 + currentTime.wMilliseconds - stDecodeResult->dwRoundTripTime;
		return 1;
	}
	return 0;
}
void Ping(char *IP)
{
	unsigned long ulDestIP = inet_addr(IP); //将IP地址转化为长整形
	if (ulDestIP == INADDR_NONE)
	{
		//转化不成功时按域名解析
		HOSTENT *pHostent = gethostbyname(IP);
		if (pHostent)
		{
			ulDestIP = (*(IN_ADDR*)pHostent->h_addr).s_addr; //将HOSTENT转化为长整形
		}
		else
		{
			printf("地址解析失败！\n");
			return;
		}
	}
	//填充目的Socket地址
	SOCKADDR_IN destSockAddr; //定义目的地址
	ZeroMemory(&destSockAddr, sizeof(SOCKADDR_IN)); //将目的地址清空
	destSockAddr.sin_family = AF_INET;
	destSockAddr.sin_addr.s_addr = ulDestIP;
	destSockAddr.sin_port = htons(0);
	//初始化WinSock
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		printf("初始化WinSock失败！\n");
		return;
	}
	//使用ICMP协议创建Raw Socket
	SOCKET sockRaw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (sockRaw == INVALID_SOCKET)
	{
		printf("创建Socket失败 !\n");
		return;
	}
	//设置端口属性
	int iTimeout = DEF_ICMP_TIMEOUT;
	if (setsockopt(sockRaw, SOL_SOCKET, SO_RCVTIMEO, (char*)&iTimeout, sizeof(iTimeout)) == SOCKET_ERROR)
	{
		printf("设置参数失败！\n");
		return;
	}
	if (setsockopt(sockRaw, SOL_SOCKET, SO_SNDTIMEO, (char*)&iTimeout, sizeof(iTimeout)) == SOCKET_ERROR)
	{
		printf("设置参数失败！\n");
		return;
	}
	//定义发送的数据段
	char IcmpSendBuf[DEF_ICMP_PACK_SIZE];
	//填充ICMP数据包个各字段
	ICMP_HEADER *pIcmpHeader = (ICMP_HEADER*)IcmpSendBuf;
	pIcmpHeader->type = ICMP_ECHO_REQUEST;
	pIcmpHeader->code = 0;
	pIcmpHeader->id = (unsigned short)GetCurrentProcessId();
	memset(IcmpSendBuf + sizeof(ICMP_HEADER), 'E', DEF_ICMP_DATA_SIZE);
	//循环发送四个请求回显icmp数据包
	int usSeqNo = 0;
	DECODE_RESULT stDecodeResult;
	int max_time = 0;
	int min_time = 10000;
	int total_time = 0;
	while (usSeqNo <= 3)
	{
		pIcmpHeader->seq = htons(usSeqNo);
		pIcmpHeader->cksum = 0;
		pIcmpHeader->cksum = GenerateChecksum((unsigned short*)IcmpSendBuf, DEF_ICMP_PACK_SIZE); //生成校验位
		//记录序列号和当前时间
		stDecodeResult.usSeqNo = usSeqNo;
		GetSystemTime(&currentTime);
		stDecodeResult.dwRoundTripTime = currentTime.wMinute * 3600 + currentTime.wSecond * 60 + currentTime.wMilliseconds;
		//发送ICMP的EchoRequest数据包
		if (sendto(sockRaw, IcmpSendBuf, DEF_ICMP_PACK_SIZE, 0, (SOCKADDR*)&destSockAddr, sizeof(destSockAddr)) == SOCKET_ERROR)
		{
			//如果目的主机不可达则直接退出
			if (WSAGetLastError() == WSAEHOSTUNREACH)
			{
				printf("目的主机不可达！\n");
				exit(0);
			}
		}
		SOCKADDR_IN from;
		int iFromLen = sizeof(from);
		int iReadLen;
		//定义接收的数据包
		char IcmpRecvBuf[MAX_ICMP_PACKET_SIZE];
		while (1)
		{
			iReadLen = recvfrom(sockRaw, IcmpRecvBuf, MAX_ICMP_PACKET_SIZE, 0, (SOCKADDR*)&from, &iFromLen);
			if (iReadLen != SOCKET_ERROR)
			{
				if (DecodeIcmpResponse_Ping(IcmpRecvBuf, sizeof(IcmpRecvBuf), &stDecodeResult))
				{
					if (stDecodeResult.dwRoundTripTime > max_time) max_time = stDecodeResult.dwRoundTripTime;
					if (stDecodeResult.dwRoundTripTime < min_time) min_time = stDecodeResult.dwRoundTripTime;
					total_time += stDecodeResult.dwRoundTripTime;
					printf("来自 %s 的回复: 字节 = %d 时间 = %dms TTL = %d\n", inet_ntoa(stDecodeResult.dwIPaddr),
						iReadLen - 20, stDecodeResult.dwRoundTripTime, stDecodeResult.ttl);
					Sleep(1000);
				}
				break;
			}
			else if (WSAGetLastError() == WSAETIMEDOUT)
			{
				printf("time out !  *****\n");
				break;
			}
			else
			{
				printf("发生未知错误！\n");
				break;
			}
		}
		usSeqNo++;
	}
	//输出屏幕信息
	printf("%s 的 Ping 统计信息：\n", inet_ntoa(stDecodeResult.dwIPaddr));
	printf("    数据包：已发送 = 4，已接收 = 4，丢失 = 0 (0%%丢失)，\n");
	printf("往返行程的估计时间(以毫秒为单位)：\n");
	printf("    最短 = %dms，最长 = %dms，平均 = %dms\n", min_time, max_time, total_time / 4);
	printf("Ping complete...\n");
	closesocket(sockRaw);
	WSACleanup();
}
int main(int argc, char* argv[])
{
	char  com[10], IP[20];
	while (1) {
		printf("command>>");
		scanf("%s %s", com, IP);
		if (strcmp(com, "ping") == 0)
		{
			Ping(IP);
		}
		else
		{
			printf("command error ! \n");
		}
	}
	return 0;
}
