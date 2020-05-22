#define WIN32
#define HAVE_REMOTE
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib")
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#pragma comment(lib, "wpcap.lib")
#pragma comment(lib, "packet.lib")


//#include "remote-ext.h"
#include "pcap.h"
//#include "stdafx.h"
//#include <string>
//#include <iostream>

#define ETH_ARP         0x0806  //以太网帧类型表示后面数据的类型，对于ARP请求或应答来说，该字段的值为x0806
#define ARP_HARDWARE    1  //硬件类型字段值为表示以太网地址
#define ETH_IP          0x0800  //协议类型字段表示要映射的协议地址类型值为x0800表示IP地址
#define ARP_REQUEST     1   //ARP请求
#define ARP_RESPONSE       2      //ARP应答
#define _WINSOCK_DEPRECATED_NO_WARNINGS

using namespace std;


/* 14字节的以太网帧头 */
typedef struct mac_header {
	u_char dmac[6];	//目的mac地址 6字节
	u_char smac[6];	//源mac地址 6字节  
	u_short type;	//类型 2字节
	//string
}mac_header;

typedef struct ip_address {
	u_char byte1; //地址第一个字节 8位
	u_char byte2;
	u_char byte3;
	u_char byte4;
}ip_address;


/*28字节的 ARP帧结构 */
struct arp_header
{
	unsigned short hdType;   //硬件类型
	unsigned short proType;   //协议类型
	unsigned char hdSize;   //硬件地址长度
	unsigned char proSize;   //协议地址长度
	unsigned short op;   //操作类型，ARP请求（1），ARP应答（2），RARP请求（3），RARP应答（4）。
	u_char smac[6];   //源MAC地址
	ip_address sip;   //源IP地址
	u_char dmac[6];   //目的MAC地址
	ip_address dip;   //目的IP地址
};

/* ARP的缓冲区 */
struct arp_buffer
{
	u_char dmac[6];   //目的MAC地址
	u_char dip1;   //目的IP地址
	u_char dip2;
	u_char dip3;
	u_char dip4;
};

//定义整个arp报文包，总长度42字节
struct ArpPacket {
	mac_header ed;
	arp_header ah;
};

arp_buffer *ab[10] ;



/* 回调函数原型 */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);


/* 回调函数，当收到每一个数据包时会被libpcap所调用 */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data)
{
	struct tm *ltime;
	char timestr[16];
	u_int ip_len,ip_tlen,udp_len,tcp_len;
	u_short sport, dport, sport2, dport2;
	time_t local_tv_sec;
	pcap_t *adhandle;

	/* 将时间戳转换成可识别的格式 */
	local_tv_sec = header->ts.tv_sec;
	ltime = localtime(&local_tv_sec);
	strftime(timestr, sizeof timestr, "%H:%M:%S", ltime);

	/* 打印数据包的时间戳和长度 */
	printf("%s.%.6d len:%d\n ", timestr, header->ts.tv_usec, header->len);

	/* 获得以太网帧头部的位置 */
	mac_header *mh;
	mh = (mac_header*)pkt_data;

	unsigned char sendbuf[42]; //arp包结构大小，42个字节
	unsigned char mac[6] = { 0x00,0x11,0x22,0x33,0x44,0x55 };
	unsigned char ip[4] = { 0x01,0x02,0x03,0x04 };
	mac_header *eh;
	arp_header *ah;
	
	//arp_buffer *ab[10];
	ah = (struct arp_header*)(pkt_data + 14);
//	ab[0]->dip.byte1 = ah->dip.byte1;
	int flag = 0;
	int i, j;
	*ab = new arp_buffer[10];
	//ab[0]->dip1 = 1;
	/*
	for (j = 0; j < 10; j++)
	{
		if (flag <= 10)
		{
			flag++;
			ab[j]->dip1 = ah->dip.byte1;
			ab[j]->dip2 = ah->dip.byte2;
			ab[j]->dip3 = ah->dip.byte3;
			ab[j]->dip4 = ah->dip.byte4;
		
			for( i = 0; i < 6; i++)
			{
				ab[j]->dmac[i] = ah->dmac[i];
			}
		}
		else
		{
			j = 0;
			flag++;
			ab[j]->dip1 = ah->dip.byte1;
			ab[j]->dip2 = ah->dip.byte2;
			ab[j]->dip3 = ah->dip.byte3;
			ab[j]->dip4 = ah->dip.byte4;
			for (i = 0; i < 6; i++)
			{
				ab[j]->dmac[i] = ah->dmac[i];
			}
		}
	}
	*/



	printf("ARP发送包内容\n ");
	printf("源以太网地址:%02x:%02x:%02x:%02x:%02x:%02x\n", *ah->smac, *(ah->smac + 1), *(ah->smac + 2), *(ah->smac + 3), *(ah->smac + 4), *(ah->smac + 5));
	u_char *mac_string;
	//printf("源IP地址:%s\n", (source_ip_address));
	printf("源IP地址：%d.%d.%d.%d\n",(ah->sip.byte1),(ah->sip.byte2),(ah->sip.byte3),(ah->sip.byte4));
	printf("目的MAC地址：ff-ff-ff-ff-ff-ff\n ");
	printf("目的IP地址：%d.%d.%d.%d\n\n", (ah->dip.byte1), (ah->dip.byte2), (ah->dip.byte3), (ah->dip.byte4));

	printf("ARP响应包内容\n ");
	printf("目的IP地址：%d.%d.%d.%d\n", (ah->dip.byte1), (ah->dip.byte2), (ah->dip.byte3), (ah->dip.byte4));
	mac_string = ah->dmac;
	printf("目的IP地址对应的以太网地址:%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
	//printf("目的IP地址:%s\n", (destination_ip_address));

	printf("ARP当前缓冲区内容\n ");
	printf("目的IP地址：%d.%d.%d.%d\n", (ah->dip.byte1), (ah->dip.byte2), (ah->dip.byte3), (ah->dip.byte4));
	mac_string = ah->dmac;
	printf("目的IP地址对应的以太网地址:%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
	/*
	for (i = 0; i < 10; i++)
	{
		printf("目的IP地址：%d.%d.%d.%d\n", (ab[i]->dip1), (ab[i]->dip2), (ab[i]->dip3), (ab[i]->dip4));
		mac_string = ab[i]->dmac;
		printf("对应的以太网地址:%02x:%02x:%02x:%02x:%02x:%02x\n", *mac_string, *(mac_string + 1), *(mac_string + 2), *(mac_string + 3), *(mac_string + 4), *(mac_string + 5));
		//printf("目的IP地址:%s\n", (destination_ip_address));
	}
	*/

	printf("\n\n");


	//构造一个ARP请求
	memset(sendbuf, 0, sizeof(sendbuf));   //ARP清零
	memcpy(sendbuf, &eh, sizeof(eh));
	memcpy(sendbuf + sizeof(eh), &ah, sizeof(ah));
	//pcap_sendpacket(adhandle, sendbuf, 42);
	/*
	//如果发送成功
	if (pcap_sendpacket(adhandle, sendbuf, 42) == 0) {
		printf("\nPacketSend succeed\n");
	}
	else {
		printf("PacketSendPacket in getmine Error: %d\n", GetLastError());
	}
	*/
	
}

int main()
{
	pcap_if_t *alldevs;
	pcap_if_t *d;
	int inum;
	int i = 0;
	pcap_t *adhandle;
	char errbuf[PCAP_ERRBUF_SIZE];
	u_int netmask;
	char packet_filter[] = "ip and udp or ip and tcp";
	struct bpf_program fcode;

	/* 获得设备列表 */
	if (pcap_findalldevs(&alldevs, errbuf) == -1)
	{
		fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}

	/* 打印列表 */
	for (d = alldevs; d; d = d->next)
	{
		printf("%d. %s", ++i, d->name);
		if (d->description)
			printf(" (%s)\n", d->description);
		else
			printf(" (No description available)\n");
	}

	if (i == 0)
	{
		printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
		return -1;
	}

	printf("Enter the interface number (1-%d):", i);
	scanf("%d", &inum);

	if (inum < 1 || inum > i)
	{
		printf("\nInterface number out of range.\n");
		/* 释放设备列表 */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* 跳转到已选设备 */
	for (d = alldevs, i = 0; i < inum - 1; d = d->next, i++);

	/* 打开适配器 */
	if ((adhandle = pcap_open(d->name,  // 设备名
		65536,     // 要捕捉的数据包的部分 
				   // 65535保证能捕获到不同数据链路层上的每个数据包的全部内容
		PCAP_OPENFLAG_PROMISCUOUS,         // 混杂模式
		1000,      // 读取超时时间
		NULL,      // 远程机器验证
		errbuf     // 错误缓冲池
	)) == NULL)
	{
		fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcap\n");
		/* 释放设备列表 */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* 检查数据链路层，为了简单，我们只考虑以太网 */
	if (pcap_datalink(adhandle) != DLT_EN10MB)
	{
		fprintf(stderr, "\nThis program works only on Ethernet networks.\n");
		/* 释放设备列表 */
		pcap_freealldevs(alldevs);
		return -1;
	}

	if (d->addresses != NULL)
		/* 获得接口第一个地址的掩码 */
		netmask = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	else
		/* 如果接口没有地址，那么我们假设一个C类的掩码 */
		netmask = 0xffffff;


	//编译过滤器
	if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) < 0)
	{
		fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");
		/* 释放设备列表 */
		pcap_freealldevs(alldevs);
		return -1;
	}

	//设置过滤器
	if (pcap_setfilter(adhandle, &fcode) < 0)
	{
		fprintf(stderr, "\nError setting the filter.\n");
		/* 释放设备列表 */
		pcap_freealldevs(alldevs);
		return -1;
	}

	printf("\nlistening on %s...\n", d->description);

	/* 释放设备列表 */
	pcap_freealldevs(alldevs);

	/* 开始捕捉 */
	pcap_loop(adhandle, 0, packet_handler, NULL);

	system("pause");
	return 0;
}



