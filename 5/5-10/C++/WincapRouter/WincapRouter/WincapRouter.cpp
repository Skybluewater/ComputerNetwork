#pragma warning(disable:4996)
#define WIN32
#ifndef _XKEYCHECK_H
#define _XKEYCHECK_H
#endif
#pragma comment(lib, "Packet")
#pragma comment(lib, "wpcap")
#pragma comment(lib, "WS2_32")
long long counter = 0;
#define HAVE_REMOTE
#include <pcap.h>
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
#include <fstream>
using namespace std;
#define threshold 1024*1024
/* IP帧格式 */
typedef struct ip_header {
	u_char ver_ihl;				//Version (4 bits) + Internet header length (4 bits)
	u_char tos;					//Type of service
	u_short tlen;				//Total length
	u_short identification;		//Identification
	u_short flags_fo;			//Flags (3 bits) + Fragment offset (13 bits)
	u_char ttl;					//Time to live
	u_char proto;				//Protocol
	u_short crc;				//Header checksum
	u_char saddr[4];			//Source address
	u_char daddr[4];			//Destination address
	u_int op_pad;				//Option + Padding
} ip_header;

char buf1[20], buf2[20];

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

typedef struct mac_header {
	MAC_Address dest;
	MAC_Address src;
	u_char type[2];
} mac_header;
#define MAX_SIZE 500
struct sendtable {
	MAC_Address addr;
	char* interface_description;
	pcap_t* send_interface;
}SendTable[MAX_SIZE];

int table_size = 0;
struct routertable {
	unsigned int dest;
	unsigned int mask;
	unsigned int next;
	bool direct;
} RouterTable[MAX_SIZE];


int router_table_size = 0;
#undef MAX_SIZE

map<unsigned int, mac_header> arp_map;

#include <string>
#include <sstream>
using namespace std;

MAC_Address local1_mac, local2_mac, next_mac, pc_mac;


unsigned int BLEndianUint32(unsigned int value)
{
	return ((value & 0x000000FF) << 24) | ((value & 0x0000FF00) << 8) | ((value & 0x00FF0000) >> 8) | ((value & 0xFF000000) >> 24);
}
void loadARPTable(const char* fileName) {
	ifstream infile;
	infile.open(fileName);
	string line;
	char _;

	{
		getline(infile, line);
		istringstream iss(line);
		MAC_Address& a = pc_mac;
		int t = 6, x;
		for (int i = 0; i < 6; i++)
		{
			iss >> hex >> x >> _;
			a.address[i] = x;
		}
	}
	{
		getline(infile, line);
		istringstream iss(line);
		MAC_Address& a = local1_mac;
		int t = 6, x;
		for (int i = 0; i < 6; i++)
		{
			iss >> hex >> x >> _;
			a.address[i] = x;
		}
	}
	{
		getline(infile, line);
		istringstream iss(line);
		MAC_Address& a = local2_mac;
		int t = 6, x;
		for (int i = 0; i < 6; i++)
		{
			iss >> hex >> x >> _;
			a.address[i] = x;
		}
	}
	{
		getline(infile, line);
		istringstream iss(line);
		MAC_Address& a = next_mac;
		int t = 6, x;
		for (int i = 0; i < 6; i++)
		{
			iss >> hex >> x >> _;
			a.address[i] = x;
		}
	}
}


void printRoutEntry(int i) {
	unsigned int dest = BLEndianUint32(RouterTable[i].dest);
	unsigned int mask = BLEndianUint32(RouterTable[i].mask);

	unsigned int next = BLEndianUint32(RouterTable[i].next);
	bool direct = RouterTable[i].direct;
	if (direct) {

		cout << "dest: " << inet_ntoa(*(in_addr*)&dest) << " mask: ";
		cout << inet_ntoa(*(in_addr*)&mask) << " direct" << endl;
	}
	else {
		cout << "dest: " << inet_ntoa(*(in_addr*)&dest);
		cout << " mask: " << inet_ntoa(*(in_addr*)&mask);
		cout << " next: " << inet_ntoa(*(in_addr*)&next) << " static" << endl;
	}
}

void printRouterTable() {
	cout << "路由表：\n";
	for (int i = 0; i < router_table_size; i++) {
		unsigned int dest = BLEndianUint32(RouterTable[i].dest);
		unsigned int mask = BLEndianUint32(RouterTable[i].mask);

		unsigned int next = BLEndianUint32(RouterTable[i].next);
		bool direct = RouterTable[i].direct;
		if (direct) {

			cout << "目的地址: " << inet_ntoa(*(in_addr*)&dest) << " 子网掩码: ";
			cout << inet_ntoa(*(in_addr*)&mask) << " direc" << endl;
		}
		else {
			cout << "目的地址: " << inet_ntoa(*(in_addr*)&dest);
			cout << " 子网掩码: " << inet_ntoa(*(in_addr*)&mask);
			cout << " 下一个: " << inet_ntoa(*(in_addr*)&next) << " static" << endl;
		}

	}
}
void loadRouterTable(routertable* t, const char* fileName) {
	ifstream infile;
	unsigned short a, b, c, d;
	u_char _;
	int mask_num;
	infile.open(fileName);
	string line;
	while (getline(infile, line)) {
		cout << line << endl;
		if (line.find("direct") != string::npos) {
			istringstream iss(line);
			// direct
			iss >> a >> _ >> b >> _ >> c >> _ >> d >> _ >> mask_num;
			char temp[4] = {};
			temp[0] = a;
			temp[1] = b;
			temp[2] = c;
			temp[3] = d;
			t[router_table_size].dest = BLEndianUint32(*(int*)temp);
			t[router_table_size].mask = UINT_MAX << (32 - mask_num);
			t[router_table_size].direct = true;
			router_table_size++;
		}
		else {
			istringstream iss(line);
			iss >> a >> _ >> b >> _ >> c >> _ >> d >> _ >> mask_num;
			char temp[4] = {};
			temp[0] = a;
			temp[1] = b;
			temp[2] = c;
			temp[3] = d;
			t[router_table_size].dest = BLEndianUint32(*(int*)temp);
			t[router_table_size].mask = UINT_MAX << (32 - mask_num);
			iss >> a >> _ >> b >> _ >> c >> _ >> d;
			printf("%d %d %d %d\n", a, b, c, d);
			temp[0] = a;
			temp[1] = b;
			temp[2] = c;
			temp[3] = d;
			t[router_table_size].next = BLEndianUint32(*(int*)temp);

			t[router_table_size].direct = false;
			router_table_size++;
			// static
		}
		printRouterTable();
		printf("dest ip %d\n", t[router_table_size - 1].dest);
	}
}

typedef struct _in_out_adapters
{
	char* name;		/* Some simple state information */
	pcap_t* input_adapter;
	pcap_t* output_adapter;
}in_out_adapters;

DWORD WINAPI CaptureAndForwardThread(LPVOID lpParameter);

CRITICAL_SECTION print_cs, table_cs;

HANDLE threads[2];

volatile int kill_forwaders = 0;

string mactostr(MAC_Address macaddr) {
	char str[20];

	sprintf(str, "%02X-%02X-%02X-%02X-%02X-%02X", macaddr.address[0], macaddr.address[1], macaddr.address[2], macaddr.address[3], macaddr.address[4], macaddr.address[5]);
	//cout << str << endl;
	return string(str);
}

char* iptos(u_long in);
char* ip6tos(struct sockaddr* sockaddr, char* address, int addrlen);




void ifprint(pcap_if_t* d)
{
	pcap_addr_t* a;
	char ip6str[128];

	printf("%s\n", d->name);

	if (d->description)
		printf("\t描述: %s\n", d->description);

	printf("\t回传: %s\n", (d->flags & PCAP_IF_LOOPBACK) ? "yes" : "no");

	for (a = d->addresses; a; a = a->next) {
		printf("\t地址族: #%d\n", a->addr->sa_family);

		switch (a->addr->sa_family)
		{
		case AF_INET:
			printf("\t地址族名称: AF_INET\n");
			if (a->addr)
				printf("\t地址: %s\n", iptos(((struct sockaddr_in*)a->addr)->sin_addr.s_addr));
			if (a->netmask)
				printf("\t子网掩码: %s\n", iptos(((struct sockaddr_in*)a->netmask)->sin_addr.s_addr));
			if (a->broadaddr)
				printf("\t广播地址: %s\n", iptos(((struct sockaddr_in*)a->broadaddr)->sin_addr.s_addr));
			if (a->dstaddr)
				printf("\t目的地址: %s\n", iptos(((struct sockaddr_in*)a->dstaddr)->sin_addr.s_addr));
			break;

		case AF_INET6:
			printf("\tAddress Family Name: AF_INET6\n");
			if (a->addr)
				printf("\t地址: %s\n", ip6tos(a->addr, ip6str, sizeof(ip6str)));
			break;

		default:
			printf("\t地址族名称未知\n");
			break;
		}
	}
	printf("\n");
}
#define IPTOSBUFFERS    12
char* iptos(u_long in)
{
	static char output[IPTOSBUFFERS][3 * 4 + 3 + 1];
	static short which;
	u_char* p;

	p = (u_char*)&in;
	which = (which + 1 == IPTOSBUFFERS ? 0 : which + 1);
	_snprintf_s(output[which], sizeof(output[which]), sizeof(output[which]), "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
	return output[which];
}

char* ip6tos(struct sockaddr* sockaddr, char* address, int addrlen)
{
	socklen_t sockaddrlen;

#ifdef WIN32
	sockaddrlen = sizeof(struct sockaddr_in6);
#else
	sockaddrlen = sizeof(struct sockaddr_storage);
#endif


	if (getnameinfo(sockaddr,
		sockaddrlen,
		address,
		addrlen,
		NULL,
		0,
		NI_NUMERICHOST) != 0) address = NULL;

	return address;
}

char config_r[40], config_a[40];

DWORD WINAPI CaptureAndForwardThread(LPVOID lpParameter)
{
	struct pcap_pkthdr* header;
	const u_char* pkt_data;
	mac_header* mh;
	int res = 0;
	in_out_adapters* ad_couple = (in_out_adapters*)lpParameter;
	unsigned int n_fwd = 0;


	while ((!kill_forwaders) && (res = pcap_next_ex(ad_couple->input_adapter, &header, &pkt_data)) >= 0)
	{
		if (header->len <= 0) continue;
		mh = (mac_header*)pkt_data;
		string mac_dest = mactostr(mh->dest), mac_src = mactostr(mh->src);
		if (res != 0)	/* Note: res=0 means "read timeout elapsed"*/
		{

			ip_header* iphdr = (ip_header*)((char*)pkt_data + 14);
			EnterCriticalSection(&print_cs);
			struct in_addr addr1;
			struct in_addr addr2;
			memcpy(&addr1, &iphdr->saddr, 4);
			memcpy(&addr2, &iphdr->daddr, 4);
			cout << counter << " ----------------------------------------------------------" << endl;
			cout << "数据帧来自 " << ad_couple->name << endl;
			cout << "源MAC地址： " << mac_src << endl;
			cout << "目的MAC地址： " << mac_dest << endl;
			cout << "源ip地址： " << inet_ntoa(addr1) << endl;
			cout << "目的IP地址： " << inet_ntoa(addr2) << endl;


			int found_entry = -1;
			unsigned dest_ip = BLEndianUint32(*(unsigned int*)(iphdr->daddr));

			printRouterTable();
			for (int i = 0; i < router_table_size; i++)
			{
				if ((dest_ip & RouterTable[i].mask) == (RouterTable[i].dest & RouterTable[i].mask)) {
					printf("查找匹配路由器表 %d\n", i);
					printRoutEntry(i);
					found_entry = i;


					if (RouterTable[found_entry].direct == false) {

						memcpy(&mh->dest, next_mac.address, 6);
						memcpy(&mh->src, local2_mac.address, 6);
					}
					else {
						memcpy(&mh->dest, pc_mac.address, 6);
						memcpy(&mh->src, local1_mac.address, 6);
					}

					char* header = (char*)iphdr;
					int tempck = 0;
					for (int i = 0; i < 10; i++) {
						if (i == 5)
							continue;
						tempck += *(unsigned short*)(header + i * 2);
					}
					while (tempck >= (1 << 16)) {
						tempck = (((1 << 16) - 1) & tempck) + (tempck >> 16);
					}
					iphdr->ttl -= 1;
					unsigned short checksum = ~tempck;
					cout << "校验和: " << checksum << endl;
					iphdr->crc = checksum;
				}
			}



			counter++;
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
				cout << "更新表" << endl;
			}
			else
			{
				table_size++;
				SendTable[table_size].addr = mh->src;
				SendTable[table_size].interface_description = ad_couple->name;
				SendTable[table_size].send_interface = ad_couple->input_adapter;
				cout << "加入新mac地址" << endl;
			}
			LeaveCriticalSection(&table_cs);
			cout << endl;
			LeaveCriticalSection(&print_cs);
			pair<char*, pcap_t*> temp;
			int found = 0;
			EnterCriticalSection(&table_cs);
			for (int i = 1; i <= table_size; i++)
			{
				if (SendTable[i].addr == mh->dest)
				{
					found = 1;
					temp.first = SendTable[i].interface_description;
					temp.second = SendTable[i].send_interface;
				}
			}
			LeaveCriticalSection(&table_cs);
			if (found)
			{
				if (temp.second == ad_couple->input_adapter)
				{
					EnterCriticalSection(&print_cs);
					cout << "丢包" << endl;
					LeaveCriticalSection(&print_cs);
				}
				else {
					EnterCriticalSection(&print_cs);
					cout << "传输包到 " << temp.first << endl;
					LeaveCriticalSection(&print_cs);
					if (pcap_sendpacket(temp.second, pkt_data, header->caplen) != 0)
					{
						EnterCriticalSection(&print_cs);

						printf("错误传输 %u bytes 包至接口 %s: %s\n",
							header->caplen,
							ad_couple->name,
							pcap_geterr(temp.second));

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
				cout << "泛洪" << endl;
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


	return 0;
}

int main()
{
	int z = 0;
	cout << "请输入：" << endl;
	cin >> z;
	getchar();
	sprintf(config_r, "config%d.txt", z);
	sprintf(config_a, "mac%d.txt", z);
	loadRouterTable(RouterTable, config_r);
	loadARPTable(config_a);
	pcap_if_t* alldevs;
	pcap_if_t* d;
	int interfaceNum1, interfaceNum2;
	int i = 0;
	pcap_t* adhandle1, * adhandle2;
	char errbuf[PCAP_ERRBUF_SIZE];
	u_int netmask1, netmask2;
	char packet_filter[256];
	struct bpf_program fcode;
	in_out_adapters couple0, couple1;


	if (pcap_findalldevs_ex((char*)"rpcap:/", NULL, &alldevs, errbuf) == -1)
	{
		fprintf(stderr, "pcap_findalldevs中出错，代码: %s\n", errbuf);
		exit(1);
	}

	/* Print the list */
	for (d = alldevs; d; d = d->next)
	{
		printf("%d. ", ++i);
		if (d->description)
			ifprint(d);
		//printf("%s %s\n", d->description);
		else
			printf("未知适配器\n");
	}

	if (i == 0)
	{
		printf("\n未安装Winpcap.\n");
		return -1;
	}


	
	cout << "输入接口个数：" << endl;
	fgets(packet_filter, sizeof(packet_filter), stdin);

	printf("\n输入要使用的接口编号 (1-%d):", i);
	scanf_s("%d", &interfaceNum1);

	if (interfaceNum1 < 1 || interfaceNum1 > i)
	{
		printf("\n下标越界\n");
		pcap_freealldevs(alldevs);
		return -1;
	}

	printf("输入要使用的接口编号（(1-%d):", i);
	scanf_s("%d", &interfaceNum2);

	if (interfaceNum2 < 1 || interfaceNum2 > i)
	{
		printf("\n下标越界.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	if (interfaceNum1 == interfaceNum2)
	{
		printf("\n接口相同，无法再同一接口操作\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}




	for (d = alldevs, i = 0; i < interfaceNum1 - 1; d = d->next, i++);

	couple0.name = d->description;
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
		pcap_freealldevs(alldevs);
		return -1;
	}

	if (d->addresses != NULL)
	{
		netmask1 = ((struct sockaddr_in*)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	}
	else
	{
		netmask1 = 0xffffff;
	}

	for (d = alldevs, i = 0; i < interfaceNum2 - 1; d = d->next, i++);
	couple1.name = d->description;
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
		pcap_freealldevs(alldevs);
		return -1;
	}

	if (d->addresses != NULL)
	{
		netmask2 = ((struct sockaddr_in*)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	}
	else
	{
		netmask2 = 0xffffff;
	}


	if (pcap_compile(adhandle1, &fcode, packet_filter, 1, netmask1) < 0)
	{
		

		pcap_close(adhandle1);
		pcap_close(adhandle2);

		pcap_freealldevs(alldevs);
		return -1;
	}

	if (pcap_setfilter(adhandle1, &fcode) < 0)
	{

		pcap_close(adhandle1);
		pcap_close(adhandle2);

		pcap_freealldevs(alldevs);
		return -1;
	}

	if (pcap_compile(adhandle2, &fcode, packet_filter, 1, netmask2) < 0)
	{
		
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

	InitializeCriticalSection(&print_cs);
	InitializeCriticalSection(&table_cs);
	sprintf(buf1, "buf1：%d -> %d", interfaceNum1, interfaceNum2);
	couple0.input_adapter = adhandle1;
	couple0.output_adapter = adhandle2;
	couple0.name = buf1;
	sprintf(buf2, "buf2：%d -> %d", interfaceNum2, interfaceNum1);
	couple1.input_adapter = adhandle2;
	couple1.output_adapter = adhandle1;
	couple1.name = buf2;
	if ((threads[0] = CreateThread(
		NULL,
		0,
		CaptureAndForwardThread,
		&couple0,
		0,
		NULL)) == NULL)
	{

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
		&couple1,
		0,
		NULL)) == NULL)
	{

		TerminateThread(threads[0], 0);

		pcap_close(adhandle1);
		pcap_close(adhandle2);

		pcap_freealldevs(alldevs);
		return -1;
	}


	printf("\n开始桥接\n");
	Sleep(INFINITE);
	return 0;
}

