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
using namespace std;
#define threshold 1024*1024
/* IP帧格式 */
u_short flipEndian(u_short x) {
	return ((x << 8) & 0xff00) | ((x >> 8) & 0x00ff);
}
u_int flipEndian32(u_int x) {
	return ((x << 24) & 0xff000000) | ((x << 8) & 0x00ff0000) | ((x >> 8) & 0x0000ff00) | ((x << 24) & 0x000000ff);
}

__int16 ck(__int16* s, int len) {
	int tempck = 0;
	for (int i = 0; i < len; i++) {
		tempck += flipEndian(s[i]);
	}
	while (tempck >= (1 << 16)) {
		tempck = (((1 << 16) - 1) & tempck) + (tempck >> 16);
	}
	return ~tempck;
}

const u_char TCP = 6;
struct ip_header {
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
	string sip() {
		char s[100];
		sprintf(s, "%d.%d.%d.%d", saddr[0], saddr[1], saddr[2], saddr[3]);
		return string(s);
	}
	string dip() {
		char s[100];
		sprintf(s, "%d.%d.%d.%d", daddr[0], daddr[1], daddr[2], daddr[3]);
		return string(s);
	}
	bool checksum() {

		__int8 hl = ver_ihl & 0x0f;
		u_short c = flipEndian(crc);
		crc = 0;
		u_short x = ck((__int16*)this, int(hl) * 2);
		crc = flipEndian(x);
		return c == x;
	}
};

struct tcp_header {
	__int16 src;
	__int16 dest;
	__int32 seq;
	__int32 ack;
	__int8  dr;
	__int8 flags;
	__int16 wnd;
	__int16 crc;
	__int16 up;
	bool checksum(ip_header* ip) {
		struct temps {
			int src;
			int des;
			__int8  z = 0;
			__int8 t = 6;
			__int16 len;
		}ph;
		memcpy(&ph, ip, 8);
		__int8 iphlen = ip->ver_ihl & 0x0f;
		ph.len = flipEndian(flipEndian(ip->tlen) - (u_short)iphlen * 4);


		int realen = flipEndian(ph.len);
		u_short c = crc;
		crc = 0;

		u_int tempck = 0;

		u_short* s = (u_short*)&ph;
		for (int i = 0; i < 6; i++)
			tempck += flipEndian(s[i]);

		s = (u_short*)this;
		for (int i = 0; i < realen / 2; i++)
			tempck += flipEndian(s[i]);
		if (realen % 2 == 1) {
			u_short t = flipEndian(s[realen - 1]) & 0xf0;
			tempck += t;

		}

		while (tempck >= (1 << 16)) {
			tempck = (((1 << 16) - 1) & tempck) + (tempck >> 16);
		}


		tempck = ~tempck;
		u_short re = tempck;
		re = flipEndian(re);

		crc = re;
		return re == c;
	}
};

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
	string tostr() {
		char str[20];
		sprintf(str, "%02X-%02X-%02X-%02X-%02X-%02X", address[0], address[1], address[2], address[3], address[4], address[5]);
		return string(str);
	}
} MacAddress;

/* 以太网数据链路层帧格式 */
const u_short IPv4 = 0x0008;
typedef struct MacHeader {
	MAC_Address dest;
	MAC_Address src;
	u_short type;
	string dest_str() {
		return dest.tostr();
	}
	string src_str() {
		return src.tostr();
	}
} MacHeader;
#define MAX_SIZE 500
struct sendtable {
	MAC_Address addr;
	char* interface_description;
	pcap_t* send_interface;
}SendTable[MAX_SIZE];
#undef MAX_SIZE
int table_size = 0;
/* Storage data structure used to pass parameters to the threads */
typedef struct _in_out_adapters
{
	char name[100];
	int state;
	pcap_t* input_adapter;
	pcap_t* output_adapter;

}adapter;

/* Prototypes */
DWORD WINAPI CaptureAndForwardThread(LPVOID lpParameter);
void ctrlc_handler(int sig);

/* This prevents the two threads to mess-up when they do printfs */
CRITICAL_SECTION print_cs, table_cs;

/* Thread handlers. Global because we wait on the threads from the CTRL+C handler */
HANDLE threads[1];

/* This global variable tells the forwarder threads they must terminate */
volatile int kill_forwaders = 0;



/*******************************************************************/

#include<fstream>

int InIP[4], OutIP[4], mac[6];

int main()
{

	ifstream config("config.txt");
	char temp[1000];
	while (config.getline(temp, 1000)) {
		switch (temp[0]) {
		case 'I':
			sscanf(strchr(temp, '=') + 1, "%d.%d.%d.%d", InIP, InIP + 1, InIP + 2, InIP + 3);
			break;
		case 'O':
			sscanf(strchr(temp, '=') + 1, "%d.%d.%d.%d", OutIP, OutIP + 1, OutIP + 2, OutIP + 3);
			break;
		case 'D':
			sscanf(strchr(temp, '=') + 1, "%d-%d-%d-%d-%d-%d", mac, mac + 1, mac + 2, mac + 3, mac + 4, mac + 5);
			break;
		}
		
	}
	pcap_if_t* alldevs;
	pcap_if_t* d;
	int inum1, inum2;
	int i = 0;
	pcap_t* adhandle1, * adhandle2;
	char errbuf[PCAP_ERRBUF_SIZE];
	u_int netmask1, netmask2;
	char packet_filter[256];
	struct bpf_program fcode;
	adapter couple0, couple1;
	couple0.state = 0;
	couple1.state = 1;

	/*
	 * Retrieve the device list
	 */

	if (pcap_findalldevs_ex((char*)PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
	{
		fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}

	/* Print the list */
	for (d = alldevs; d; d = d->next)
	{
		printf("%d. ", ++i);
		if (d->description)
			printf("%s\n%s\n", d->description, d->name);
		else
			printf("<unknown adapter>\n");
	}

	if (i == 0)
	{
		printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
		return -1;
	}


	/*
	 * Get input from the user
	 */

	 /* Get the filter*/
	printf("\nSpecify filter (hit return for no filter):");

	fgets(packet_filter, sizeof(packet_filter), stdin);

	/* Get the first interface number*/
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


	/*
	 * Open the specified couple of adapters
	 */

	 /* Jump to the first selected adapter */
	for (d = alldevs, i = 0; i < inum1 - 1; d = d->next, i++);

	/*
	 * Open the first adapter.
	 * *NOTICE* the flags we are using, they are important for the behavior of the prgram:
	 *	- PCAP_OPENFLAG_PROMISCUOUS: tells the adapter to go in promiscuous mode.
	 *    This means that we are capturing all the traffic, not only the one to or from
	 *    this machine.
	 *	- PCAP_OPENFLAG_NOCAPTURE_LOCAL: prevents the adapter from capturing again the packets
	 *	  transmitted by itself. This avoids annoying loops.
	 *	- PCAP_OPENFLAG_MAX_RESPONSIVENESS: configures the adapter to provide minimum latency,
	 *	  at the cost of higher CPU usage.
	 */
	strcpy(couple0.name, d->description);
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
		netmask1 = ((struct sockaddr_in*)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	}
	else
	{
		/* If the interface is without addresses we suppose to be in a C class network */
		netmask1 = 0xffffff;
	}

	/* Jump to the second selected adapter */
	for (d = alldevs, i = 0; i < inum2 - 1; d = d->next, i++);
	strcpy(couple1.name, d->description);
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
		netmask2 = ((struct sockaddr_in*)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	}
	else
	{
		/* If the interface is without addresses we suppose to be in a C class network */
		netmask2 = 0xffffff;
	}


	/*
	 * Compile and set the filters
	 */

	 /* compile the filter for the first adapter */
	if (pcap_compile(adhandle1, &fcode, packet_filter, 1, netmask1) < 0)
	{
		fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");

		/* Close the adapters */
		pcap_close(adhandle1);
		pcap_close(adhandle2);

		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* set the filter for the first adapter*/
	if (pcap_setfilter(adhandle1, &fcode) < 0)
	{
		fprintf(stderr, "\nError setting the filter.\n");

		/* Close the adapters */
		pcap_close(adhandle1);
		pcap_close(adhandle2);

		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* compile the filter for the second adapter */
	if (pcap_compile(adhandle2, &fcode, packet_filter, 1, netmask2) < 0)
	{
		fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");

		/* Close the adapters */
		pcap_close(adhandle1);
		pcap_close(adhandle2);

		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* set the filter for the second adapter*/
	if (pcap_setfilter(adhandle2, &fcode) < 0)
	{
		fprintf(stderr, "\nError setting the filter.\n");

		/* Close the adapters */
		pcap_close(adhandle1);
		pcap_close(adhandle2);

		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* At this point, we don't need the device list any more. Free it */


	/*
	 * Start the threads that will forward the packets
	 */

	 /* Initialize the critical section that will be used by the threads for console output */
	InitializeCriticalSection(&print_cs);
	InitializeCriticalSection(&table_cs);
	/* Init input parameters of the threads */
	couple0.input_adapter = adhandle1;
	couple0.output_adapter = adhandle2;
	couple1.input_adapter = adhandle2;
	couple1.output_adapter = adhandle1;

	cout << couple0.name << endl;
	cout << couple1.name << endl;

	pcap_freealldevs(alldevs);
	/* Start first thread */
	if ((threads[0] = CreateThread(
		NULL,
		0,
		CaptureAndForwardThread,
		&couple0,
		0,
		NULL)) == NULL)
	{
		fprintf(stderr, "error creating the first forward thread");
		TerminateThread(threads[0], 0);

		/* Close the adapters */
		pcap_close(adhandle1);
		pcap_close(adhandle2);

		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}



	/*
	 * Install a CTRL+C handler that will do the cleanups on exit
	 */
	signal(SIGINT, ctrlc_handler);

	/*
	 * Done!
	 * Wait for the Greek calends...
	 */
	printf("\nStart bridging the two adapters...\n");
	Sleep(INFINITE);
	return 0;
}

/*******************************************************************
 * Forwarding thread.
 * Gets the packets from the input adapter and sends them to the output one.
 *******************************************************************/
DWORD WINAPI CaptureAndForwardThread(LPVOID lpParameter)
{
	struct pcap_pkthdr* header;
	const u_char* pkt_data;
	MacHeader* mh;
	int res = 0;
	adapter* ad_couple = (adapter*)lpParameter;
	unsigned int n_fwd = 0;

	/*
	 * Loop receiving packets from the first input adapter
	 */

	while ((!kill_forwaders) && (res = pcap_next_ex(ad_couple->input_adapter, &header, &pkt_data)) >= 0)
	{
		if (header->len <= 0) continue;
		mh = (MacHeader*)pkt_data;


		if (res != 0)	/* Note: res=0 means "read timeout elapsed"*/
		{
			/*
			 * Print something, just to show when we have activity.
			 * BEWARE: acquiring a critical section and printing strings with printf
			 * is something inefficient that you seriously want to avoid in your packet loop!
			 * However, since this is a *sample program*, we privilege visual output to efficiency.
			 */
			bool sent = false;
			if (mh->type == IPv4) {
				ip_header* ih;
				ih = (ip_header*)(mh + 1);
				if (ih->proto == TCP) {
					tcp_header* th = (tcp_header*)(ih + 1);
					EnterCriticalSection(&print_cs);
					cout << "----------------------------------------------------------" << endl;
					cout << "Basic: " << endl;
					cout << "Data frame is from " << string(ad_couple->name) << endl;
					cout << "1.source MAC address: " << mh->src_str() << endl;
					cout << "2.source port : " << flipEndian(th->src) << endl;
					cout << "3.destination MAC address: " << mh->dest_str() << endl;
					cout << "4.destination port: " << flipEndian(th->dest) << endl;
					cout << "After: " << endl;
					cout << "1.source IP :" << ih->sip();
					printf(" to %d.%d.%d.%d\n", OutIP[0], OutIP[1], OutIP[2], OutIP[3]);
					cout << "2.destination IP : " << ih->dip() << endl;
					cout << "3.internet type: " << mh->type << endl;
					cout << "4.internet type(IPv4): " << IPv4 << endl;
					cout << "Detail: " << endl;
					cout << "1.new Mac: ";
					printf("%d-%d-%d-%d-%d-%d\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
					printf("2.NAT Table: \n");
					printf("%d.%d.%d.%d -- %d.%d.%d.%d\n", InIP[0], InIP[1], InIP[2], InIP[3], OutIP[0], OutIP[1], OutIP[2], OutIP[3]);

					cout << "----------------------------------------------------------" << endl;
					LeaveCriticalSection(&print_cs);
					for (int i = 0; i < 4; i++) {
						ih->saddr[i] = OutIP[i];
					}for (int i = 0; i < 6; i++) {
						mh->src.address[i] = mac[i];
					}

				}
			}

			if (!sent) {
				if (pcap_sendpacket(ad_couple->output_adapter, pkt_data, header->caplen) != 0) {
					EnterCriticalSection(&print_cs);

					printf("Error sending a %u bytes packets on interface %u: %s\n",
						header->caplen,
						ad_couple->name,
						pcap_geterr(ad_couple->output_adapter));

					LeaveCriticalSection(&print_cs);
				}
				else {
					n_fwd++;
				}
			}


		}
	}

	/*
	 * We're out of the main loop. Check the reason.
	 */
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

/*******************************************************************
 * CTRL+C hanlder.
 * We order the threads to die and then we patiently wait for their
 * suicide.
 *******************************************************************/
void ctrlc_handler(int sig)
{
	/*
	 * unused variable
	 */
	(VOID)(sig);

	kill_forwaders = 1;

	WaitForMultipleObjects(1,
		threads,
		TRUE,		/* Wait for all the handles */
		5000);		/* Timeout */

	exit(0);
}