#define WIN32
#define HAVE_REMOTE
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib")

//#include "remote-ext.h"
#include "pcap.h"
//#include <string>
//#include <iostream>
using namespace std;

/* 6字节的MAC地址 */
typedef struct mac_address {
	u_char byte1; //地址第一个字节 8位
	u_char byte2;
	u_char byte3;
	u_char byte4;
	u_char byte5;
	u_char byte6;
}mac_address;

/* 14字节的以太网帧头 */
typedef struct mac_header {
	mac_address dmac;	//目的mac地址 6字节
	mac_address smac;	//源mac地址 6字节  
	u_short type;	//类型 2字节
	//string
}mac_header;

/* 4字节的以太网帧尾 */
typedef struct mac_tail {
	u_int fcs;	//4字节
}mac_tail;

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

/* 4字节的IP地址 */
typedef struct ip_address {
	u_char byte1; //地址第一个字节 8位
	u_char byte2;
	u_char byte3;
	u_char byte4;
}ip_address;

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
	ip_address  saddr;      // 源地址(Source address) 32位 4字节
	ip_address  daddr;      // 目的地址(Destination address) 32位
	u_int   op_pad;         // 选项与填充(Option + Padding)	 32 位 4字节
}ip_header;

/* UDP 首部*/
typedef struct udp_header {
	u_short sport;          // 源端口(Source port) 2字节
	u_short dport;          // 目的端口(Destination port) 2字节
	u_short len;            // UDP数据包长度(Datagram length) 2字节
	u_short crc;            // 校验和(Checksum) 2字节
}udp_header;

/* 10字节的udp数据包 */
typedef struct udp_data {
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
}udp_data;

/* TCP 首部*/
typedef struct tcp_header {
	u_short sport;          // 源端口(Source port) 2字节 16bit
	u_short dport;          // 目的端口(Destination port) 2字节
	u_int shunxu;			//  4字节
	u_int queren;			//  4字节
	u_char len;				// 偏移+保留 10位 算8位 当成一个字节
	u_char control;			// 控制 6位算一个字节
	u_short window;			//窗口 2字节
	u_short crc;			//校验和 2字节
	u_short jinji;			//紧急指针 2字节
	u_int   op_pad;         // 选项与填充(Option + Padding)	4字节 32位
}tcp_header;

/* 10字节的tcp数据包 */
typedef struct tcp_data {
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
}tcp_data;


/* UDP专用的DHCP报文格式*/
typedef struct dhcp {
	u_char op;
	u_char htype;
	u_char hlen;
	u_char hops;
	u_int  xid;
	u_short secs;
	u_short flags;
	u_int ciaddr;
	u_int yiaddr;
	u_int siaddr;
	u_int giaddr;
}dhcp;

/* 回调函数原型 */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);


/* 回调函数，当收到每一个数据包时会被libpcap所调用 */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data)
{
	struct tm *ltime;
	char timestr[16];
	ip_header *ih;
	udp_header *uh;
	u_int ip_len,ip_tlen,udp_len,tcp_len;
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

	/* 获得UDP首部的位置 */
	ip_len = (ih->ver_ihl & 0xf) * 4;
	uh = (udp_header *)((u_char*)ih + ip_len);

	/* 获得TCP首部的位置 */
	tcp_header *th;
	th = (tcp_header *)((u_char*)ih + ip_len);

	/* 获得ip数据包的位置 */
	ip_data *id;
	id = (ip_data *)((u_char*)ih + ip_len);

	/* 获得以太网帧尾部的位置 */
	mac_tail *mt;
	ip_tlen = (ih->tlen & 0xf) * 1;
	mt = (mac_tail*)((u_char*)ih + ip_tlen);

	/* 获得dhcp的位置 */
	dhcp *d;
	udp_len = (uh->len & 0xf) * 1;
	d = (dhcp*)((u_char*)uh + udp_len);

	/* 获得udp数据包的位置 */
	udp_data *ud;
	ud = (udp_data *)((u_char*)uh + udp_len);

	/* 获得tcp数据包的位置 */
	tcp_data *td;
	tcp_len = (th->len & 0xf) * 4;
	td = (tcp_data *)((u_char*)uh + tcp_len);

	/* 将网络字节序列转换成主机字节序列 */
	sport = ntohs(uh->sport);
	dport = ntohs(uh->dport);
	sport2 = ntohs(th->sport);
	dport2 = ntohs(th->dport);

	/* 打印MAC子层结构 */
	printf("打印MAC子层结构\n");
	//cout << "帧头" << mh<<" IP数据包"<<ih<<"帧尾"<<mt<<endl;
	printf("帧头  DMAC:%d%d%d%d%d%d SMAC:%d%d%d%d%d%d type:%hd",
		mh->dmac.byte1, mh->dmac.byte2, mh->dmac.byte3, mh->dmac.byte4, mh->dmac.byte5, mh->dmac.byte6,
		mh->smac.byte1, mh->smac.byte2, mh->smac.byte3, mh->smac.byte4, mh->smac.byte5, mh->smac.byte6,
		mh->type);
	printf("\n");
	printf("ip数据包的前10个字节:%d%d%hd%hd%hd%d%d",
		ih->ver_ihl,
		ih->tos,
		ih->tlen,
		ih->identification,
		ih->flags_fo,
		ih->ttl,
		ih->proto);
	printf("\n");
	printf("帧尾 FCS:%d",
		mt->fcs);
	printf("\n\n");

	/* 打印网络层结构 */
	printf("打印网络层结构\n");
	printf("ip首部:%d 版本号和首部长度:%d 服务类型:%d 协议:%d 源ip地址:%d.%d.%d.%d 目标ip地址:%d.%d.%d.%d",
		ih,
		ih->ver_ihl,
		ih->tos,
		ih->proto,
		ih->saddr.byte1,
		ih->saddr.byte2,
		ih->saddr.byte3,
		ih->saddr.byte4,
		ih->daddr.byte1,
		ih->daddr.byte2,
		ih->daddr.byte3,
		ih->daddr.byte4);
	printf("\n");
	printf("前46个字节的数据包:%d%d%d%d%d%d%d%d%d%d%d%d",
		id->byte1,
		id->byte2, 
		id->byte3, 
		id->byte4, 
		id->byte5, 
		id->byte6, 
		id->byte7,
		id->byte8,
		id->byte9,
		id->byte10,
		id->byte11,
		id->byte12);
	printf("\n\n");

	/* 打印传输层结构 */
	printf("打印传输层结构\n");
	printf("udp首部:%d 源端口:%d 目标端口:%d",
		uh,
		sport,
		dport);
	printf("\n");
	printf("udp数据包的前10个字节:%d%d%d%d%d%d%d%d%d%d",
		ud->byte1,
		ud->byte2,
		ud->byte3,
		ud->byte4,
		ud->byte5,
		ud->byte6,
		ud->byte7,
		ud->byte8,
		ud->byte9,
		ud->byte10);
	printf("\n");
	printf("tcp首部:%d 源端口:%d 目标端口:%d 序号:%d 确认号:%d",
		th,
		sport2,
		dport2,
		th->shunxu,
		th->queren);
	printf("\n");
	printf("tcp数据包的前10个字节:%d%d%d%d%d%d%d%d%d%d",
		td->byte1,
		td->byte2,
		td->byte3,
		td->byte4,
		td->byte5,
		td->byte6,
		td->byte7,
		td->byte8,
		td->byte9,
		td->byte10);
	printf("\n\n");


	/* 打印应用层结构 */
	printf("打印应用层结构\n");
	printf("DHCP:%d 操作类型:%d 客户端的MAC地址类型:%d 客户端的IP地址:%d 服务器分配给客户端的IP地址:%d",
		d,
		d->op,
		d->htype,
		d->ciaddr,
		d->yiaddr);
	printf("\n\n");
	/*
	printf("DHCP:%d 操作类型:%d 客户端的MAC地址类型:%d 客户端的IP地址:%d 服务器分配给客户端的IP地址:%d",
		d,
		d->op,
		d->htype,
		d->ciaddr,
		d->yiaddr);
	printf("\n\n");
	//tcp的某个协议 http和ftp报文长度不确定，dns太复杂。。。无法输出各个字段的值
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



