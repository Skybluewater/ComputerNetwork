
#pragma warning(disable:4996)

#ifndef _XKEYCHECK_H
#define _XKEYCHECK_H
#endif
#define WIN32
#define WPCAP
#define HAVE_REMOTE
#pragma comment(lib, "Packet")
#pragma comment(lib, "wpcap")
#pragma comment(lib, "WS2_32")

#include "pcap.h"
#include <signal.h>
#include <Packet32.h>
#include <ntddndis.h>
#include <string>
#include <remote-ext.h>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <time.h>
#include <map>
using namespace std;
#define threshold 1024*1024
#define MAX_SIZE 500

/* IPv4 数据包首部 */
typedef struct ip_header {
	u_char  ver_ihl;        // 版本 (4 bits) + 首部长度 (4 bits) 共1字节
	u_char  tos;            // 服务类型(Type of service) 
	u_short tlen;           // 总长(Total length)  16位 单位一个字节 2字节
	u_short identification; // 标识(Identification)
	u_short flags_fo;       // 标志位(Flags) (3 bits) + 段偏移量(Fragment offset) (13 bits)
	u_char  ttl;            // 存活时间(Time to live) 8位
	u_char  proto;          // 协议(Protocol) 8位
	u_short crc;            // 首部校验和(Header checksum) 16位
	u_char  saddr[4];      // 源地址(Source address) 32位 4字节
	u_char  daddr[4];      // 目的地址(Destination address) 32位
	u_int   op_pad;         // 选项与填充(Option + Padding)	 32 位 4字节
}ip_header;


/* 最多1500字节最少为46字节的ip数据包 */
typedef struct ip_data {
	u_int byte1;	//4字节
	u_int byte2;
	u_int byte3;
	u_int byte4;
	u_int byte5;
	u_int byte6;
	u_int byte7;
	u_int byte8;
	u_int byte9;
	u_int byte10;
	u_int byte11;
	u_int byte12;
}ip_data;

/* 6字节的MAC地址 */
typedef struct mac_address {
	u_char byte1; //地址第一个字节 8位
	u_char byte2;
	u_char byte3;
	u_char byte4;
	u_char byte5;
	u_char byte6;
}mac_address;


typedef struct MAC_Address {
	u_char address[6];
	bool operator < (const MAC_Address& b) const {
		for (int i = 0; i < 6; i++)
			if (address[i] < b.address[i])
				return true;
		return false;
	}
	bool operator == (const MAC_Address& b) const {
		for (int i = 0; i < 6; i++)
			if (address[i] != b.address[i])
				return false;
		return true;
	}
} macAddr;

/* 14字节的以太网帧头 */
typedef struct mac_header {
	MAC_Address dest;	//目的mac地址 6字节
	MAC_Address src;	//源mac地址 6字节  
	u_short type[2];	//类型 2字节
	//string
}mac_header;

// 4字节的以太网帧尾 
typedef struct mac_tail {
	u_int fcs;	//4字节
}mac_tail;

struct sendtable {
	MAC_Address addr;
	char *interface_description;
	pcap_t *send_interface;
}SendTable[MAX_SIZE];


int table_size = 0;
typedef struct _adapters_info
{
	char *name;		
	pcap_t *input_adapter;
	pcap_t *output_adapter;
}adapters_info;


DWORD WINAPI CaptureAndForwardThread(LPVOID lpParameter);
void ctrlc_handler(int sig);

/* This prevents the two threads to mess-up when they do printfs */
CRITICAL_SECTION print_cs, table_cs;

/* Thread handlers. Global because we wait on the threads from the CTRL+C handler */
HANDLE threads[2];

/* This global variable tells the forwarder threads they must terminate */
volatile int kill_forwaders = 0;

string mactostr(MAC_Address macaddr) {
	char str[20];
	sprintf(str, "%02X-%02X-%02X-%02X-%02X-%02X", macaddr.address[0], macaddr.address[1], macaddr.address[2], macaddr.address[3], macaddr.address[4], macaddr.address[5]);

	return string(str);
}

/* 回调函数原型 */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);


/* 回调函数，当收到每一个数据包时会被libpcap所调用 */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data)
{
	struct tm *ltime;
	char timestr[16];
	ip_header *ih;;
	u_int ip_len = 0, ip_tlen, udp_len, tcp_len;
	u_short sport, dport, sport2, dport2;
	time_t local_tv_sec;

	/* 将时间戳转换成可识别的格式 */
	local_tv_sec = header->ts.tv_sec;
	ltime = localtime(&local_tv_sec);
	strftime(timestr, sizeof timestr, "%H:%M:%S", ltime);

	/* 打印数据包的时间戳和长度 */
	printf("%s.%.6d len:%d\n ", timestr, header->ts.tv_usec, header->len);

	/* 获得以太网帧头部的位置 */
	mac_header *mh;
	mh = (mac_header*)pkt_data;

	/* 获得IP数据包头部的位置 */
	ih = (ip_header *)(pkt_data + 14); //以太网头部长度


	/* 获得ip数据包的位置 */
	ip_data *id;
	id = (ip_data *)((u_char*)ih + ip_len);


	/* 打印MAC子层结构 */
	printf("捕获帧内容\n");
	//cout << "帧头" << mh<<" IP数据包"<<ih<<"帧尾"<<mt<<endl;
	printf("数据包的前10个字节:%d%d%hd%hd%hd%d%d",
		ih->ver_ihl,
		ih->tos,
		ih->tlen,
		ih->identification,
		ih->flags_fo,
		ih->ttl,
		ih->proto);
	printf("\n\n");

}

DWORD WINAPI CaptureAndForwardThread(LPVOID lpParameter)
{
	struct pcap_pkthdr *header;
	const u_char *pkt_data;
	mac_header *mh;
	int res = 0;
	adapters_info* ad_couple = (adapters_info*)lpParameter;
	unsigned int n_fwd = 0;

	while ((!kill_forwaders) && (res = pcap_next_ex(ad_couple->input_adapter, &header, &pkt_data)) >= 0)
	{
		if (header->len <= 0) continue;
		mh = (mac_header *)pkt_data;
		string mac_dest = mactostr(mh->dest), mac_src = mactostr(mh->src);

		struct tm *ltime;
		char timestr[16];
		ip_header *ih;;
		u_int ip_len = 0, ip_tlen, udp_len, tcp_len;
		u_short sport, dport, sport2, dport2;
		time_t local_tv_sec;

		/* 将时间戳转换成可识别的格式 */
		local_tv_sec = header->ts.tv_sec;
		ltime = localtime(&local_tv_sec);
		strftime(timestr, sizeof timestr, "%H:%M:%S", ltime);

		/* 打印数据包的时间戳和长度 */
		printf("%s.%.6d len:%d\n ", timestr, header->ts.tv_usec, header->len);

		/* 获得以太网帧头部的位置 */
		mac_header *mh;
		mh = (mac_header*)pkt_data;

		/* 获得IP数据包头部的位置 */
		ih = (ip_header *)(pkt_data + 14); //以太网头部长度

		/* 获得ip数据包的位置 */
		ip_data *id;
		id = (ip_data *)((u_char*)ih + ip_len);

		printf("捕获帧内容\n");
		printf("数据包的前10个字节:%d%d%hd%hd%hd%d%d",
			ih->ver_ihl,
			ih->tos,
			ih->tlen,
			ih->identification,
			ih->flags_fo,
			ih->ttl,
			ih->proto);
		printf("\n\n");


		if (res != 0)
		{
			EnterCriticalSection(&print_cs);
			cout << "源MAC地址为 " << mac_src << endl;
			cout << "目的MAC地址位 " << mac_dest << endl;

			EnterCriticalSection(&table_cs);
			int existed = 0;
			for (int i = 1; i <= table_size; i++)
			{
				if (SendTable[i].addr == mh->src)
				{
					existed = i;
					break;
				}
			}
			if (existed)
			{
				SendTable[existed].interface_description = ad_couple->name;
				SendTable[existed].send_interface = ad_couple->input_adapter;
				cout << "更新学习表" << endl;
			}
			else
			{
				table_size++;
				SendTable[table_size].addr = mh->src;
				SendTable[table_size].interface_description = ad_couple->name;
				SendTable[table_size].send_interface = ad_couple->input_adapter;
				cout << "逆向学习添加新的MAC地址" << endl;
			}
			cout << "学习表的内容为:" << endl;
			for (int i = 1; i <= table_size; i++)
			{
				cout << mactostr(SendTable[i].addr) << ": " << SendTable[i].interface_description << endl;
			}
			LeaveCriticalSection(&table_cs);
			cout << endl;
			LeaveCriticalSection(&print_cs);

			
			pair<char *, pcap_t *> tmp;
			int found = 0;
			EnterCriticalSection(&table_cs);
			
			for (int i = 1; i <= table_size; i++)
			{
				if (SendTable[i].addr == mh->dest)
				{
					found = 1;
					tmp.first = SendTable[i].interface_description;
					tmp.second = SendTable[i].send_interface;
				}
			}
			LeaveCriticalSection(&table_cs);
			if (found)
			{
				if (tmp.second == ad_couple->input_adapter)
				{
					EnterCriticalSection(&print_cs);
					cout << "丢弃包" << endl;
					LeaveCriticalSection(&print_cs);
				}
				else {
					EnterCriticalSection(&print_cs);
					cout << "转发包 " << tmp.first << endl;
					LeaveCriticalSection(&print_cs);
					if (pcap_sendpacket(tmp.second, pkt_data, header->caplen) != 0)
					{
						EnterCriticalSection(&print_cs);

						printf("Error sending a %u bytes packets on interface %s: %s\n",
							header->caplen,
							ad_couple->name,
							pcap_geterr(tmp.second));

						LeaveCriticalSection(&print_cs);
					}
					else
					{
						n_fwd++;
					}
				}
			}
			else
			{
				EnterCriticalSection(&print_cs);
				cout << "扩散包" << endl;
				LeaveCriticalSection(&print_cs);
				if (pcap_sendpacket(ad_couple->output_adapter, pkt_data, header->caplen) != 0)
				{
					EnterCriticalSection(&print_cs);

					printf("Error sending a %u bytes packets on interface %s: %s\n",
						header->caplen,
						ad_couple->name,
						pcap_geterr(ad_couple->output_adapter));

					LeaveCriticalSection(&print_cs);
				}
				else
				{
					n_fwd++;
				}
			}
			EnterCriticalSection(&print_cs);
			LeaveCriticalSection(&print_cs);
		}
	}

	
	if (res < 0)
	{
		EnterCriticalSection(&print_cs);

		printf("Error capturing the packets: %s\n", pcap_geterr(ad_couple->input_adapter));
		fflush(stdout);

		LeaveCriticalSection(&print_cs);
	}
	else
	{
		EnterCriticalSection(&print_cs);

		printf("End of bridging on interface %s. Forwarded packets:%u\n",
			ad_couple->name,
			n_fwd);
		fflush(stdout);

		LeaveCriticalSection(&print_cs);
	}

	return 0;
}

void ctrlc_handler(int sig)
{
	(VOID)(sig);

	kill_forwaders = 1;

	WaitForMultipleObjects(2,
		threads,
		TRUE,		/* Wait for all the handles */
		5000);		/* Timeout */

	exit(0);
}



int main()
{
	pcap_if_t *alldevs;
	pcap_if_t *d;
	int inum1, inum2;
	int i = 0;
	pcap_t *adhandle1, *adhandle2;
	char errbuf[PCAP_ERRBUF_SIZE];
	u_int netmask1, netmask2;
	char packet_filter[256];
	struct bpf_program fcode;
	adapters_info table1, table2;

	if (pcap_findalldevs_ex((char *)PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
	{
		fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}

	for (d = alldevs; d; d = d->next)
	{
		printf("%d. ", ++i);
		if (d->description)
			printf("%s\n", d->description);
		else
			printf("<unknown adapter>\n");
	}

	if (i == 0)
	{
		printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
		return -1;
	}

	printf("\nEnter the number of the first interface to use (1-%d):", i);
	scanf_s("%d", &inum1);

	if (inum1 < 1 || inum1 > i)
	{
		printf("\nInterface number out of range.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* Get the second interface number*/
	printf("Enter the number of the first interface to use (1-%d):", i);
	scanf_s("%d", &inum2);

	if (inum2 < 1 || inum2 > i)
	{
		printf("\nInterface number out of range.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	if (inum1 == inum2)
	{
		printf("\nCannot bridge packets on the same interface.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	for (d = alldevs, i = 0; i < inum1 - 1; d = d->next, i++);


	table1.name = d->description;
	if ((adhandle1 = pcap_open(d->name,						// name of the device
		65536,							// portion of the packet to capture. 
									   // 65536 grants that the whole packet will be captured on every link layer.
		PCAP_OPENFLAG_PROMISCUOUS |	// flags. We specify that we don't want to capture loopback packets, and that the driver should deliver us the packets as fast as possible
		PCAP_OPENFLAG_NOCAPTURE_LOCAL |
		PCAP_OPENFLAG_MAX_RESPONSIVENESS,
		500,							// read timeout
		NULL,							// remote authentication
		errbuf							// error buffer
	)) == NULL)
	{
		fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcap\n", d->description);
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	if (d->addresses != NULL)
	{
		/* Retrieve the mask of the first address of the interface */
		netmask1 = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	}
	else
	{
		/* If the interface is without addresses we suppose to be in a C class network */
		netmask1 = 0xffffff;
	}

	/* Jump to the second selected adapter */
	for (d = alldevs, i = 0; i < inum2 - 1; d = d->next, i++);
	table2.name = d->description;
	/* Open the second adapter */
	if ((adhandle2 = pcap_open(d->name,						// name of the device
		65536,							// portion of the packet to capture. 
									   // 65536 grants that the whole packet will be captured on every link layer.
		PCAP_OPENFLAG_PROMISCUOUS |	// flags. We specify that we don't want to capture loopback packets, and that the driver should deliver us the packets as fast as possible
		PCAP_OPENFLAG_NOCAPTURE_LOCAL |
		PCAP_OPENFLAG_MAX_RESPONSIVENESS,
		500,							// read timeout
		NULL,							// remote authentication
		errbuf							// error buffer
	)) == NULL)
	{
		fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcap\n", d->description);
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	if (d->addresses != NULL)
	{
		/* Retrieve the mask of the first address of the interface */
		netmask2 = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	}
	else
	{
		/* If the interface is without addresses we suppose to be in a C class network */
		netmask2 = 0xffffff;
	}


	if (pcap_compile(adhandle1, &fcode, packet_filter, 1, netmask1) < 0)
	{
		fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");

		pcap_close(adhandle1);
		pcap_close(adhandle2);

		pcap_freealldevs(alldevs);
		return -1;
	}

	if (pcap_setfilter(adhandle1, &fcode) < 0)
	{
		fprintf(stderr, "\nError setting the filter.\n");

		pcap_close(adhandle1);
		pcap_close(adhandle2);

		pcap_freealldevs(alldevs);
		return -1;
	}

	if (pcap_compile(adhandle2, &fcode, packet_filter, 1, netmask2) < 0)
	{
		fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");

		pcap_close(adhandle1);
		pcap_close(adhandle2);

		pcap_freealldevs(alldevs);
		return -1;
	}

	if (pcap_setfilter(adhandle2, &fcode) < 0)
	{
		fprintf(stderr, "\nError setting the filter.\n");

		pcap_close(adhandle1);
		pcap_close(adhandle2);

		pcap_freealldevs(alldevs);
		return -1;
	}

	pcap_freealldevs(alldevs);

	//初始化临界资源对象
	InitializeCriticalSection(&print_cs);
	InitializeCriticalSection(&table_cs);
	
	table1.input_adapter = adhandle1;
	table1.output_adapter = adhandle2;
	table2.input_adapter = adhandle2;
	table2.output_adapter = adhandle1;

	//pcap_loop(adhandle1, 0, packet_handler, NULL);

	/* Start first thread */
	if ((threads[0] = CreateThread(
		NULL,
		0,
		CaptureAndForwardThread,
		&table1,
		0,
		NULL)) == NULL)
	{
		fprintf(stderr, "error creating the first forward thread");

		/* Close the adapters */
		pcap_close(adhandle1);
		pcap_close(adhandle2);

		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* Start second thread */
	if ((threads[1] = CreateThread(
		NULL,
		0,
		CaptureAndForwardThread,
		&table2,
		0,
		NULL)) == NULL)
	{
		fprintf(stderr, "error creating the second forward thread");

		/* Kill the first thread. Not very gentle at all...*/
		TerminateThread(threads[0], 0);

		/* Close the adapters */
		pcap_close(adhandle1);
		pcap_close(adhandle2);

		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}


	signal(SIGINT, ctrlc_handler);

	printf("\nStart bridging the two adapters...\n");
	Sleep(INFINITE);
	return 0;
}

