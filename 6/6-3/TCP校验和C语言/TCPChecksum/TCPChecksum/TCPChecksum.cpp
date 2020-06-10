#include <stdio.h>
#include <stdint.h>

typedef unsigned int u32;
typedef unsigned long __u64;
typedef unsigned int __u32;
typedef unsigned short __u16;

#ifdef __CHECKER__
#define __bitwise__ __attribute__((bitwise))
#else
#define __bitwise__
#endif
#define __bitwise __bitwise__

typedef __u16 __bitwise __le16;
typedef __u16 __bitwise __be16;
typedef __u32 __bitwise __le32;
typedef __u32 __bitwise __be32;
typedef __u64 __bitwise __le64;
typedef __u64 __bitwise __be64;

typedef __u16 __bitwise __sum16;
typedef __u32 __bitwise __wsum;



uint16_t summing(uint8_t* data, uint16_t prior_cksum, uint32_t size)
{
	uint32_t cksum = prior_cksum;
	uint32_t index = 0;

	if (size % 2 != 0) return 0;

	while (index < size) {

		cksum = cksum + ((*(data + index) << 8) + *(data + index + 1));
		index += 2;
	}
	cksum = cksum + (cksum >> 16);
	return ~cksum & 0xffff;


	printf("cksum = %u, cksum = %x\n", cksum, cksum);

	return cksum;
}

uint16_t gettcpcheck(uint8_t* data, uint32_t size)
{
	uint32_t cksum = 0;


	cksum = summing(data, 0, size);//tcp_head

	return cksum;
}

int main()
{
	uint8_t SourceIP[] = { 192,168,1,108 };
	uint8_t DestinationIP[] = { 61, 162, 100, 28 };
	uint8_t TCPSeg[] = { 0xe6, 0xd1, 0x00, 0x50, 0x9c, 0xbf, 0xea, 0xa9, 0x00, 0x00, 0x00, 0x00, 0x80, 0x02, 0xfa, 0xf0, 0xa1, 0xc1, 0x00, 0x00, 0x02, 0x04, 0x05, 0xb4, 0x01, 0x03, 0x03, 0x08, 0x01, 0x01, 0x04, 0x02 };

	uint8_t TCPpse[12] = {  };
	uint8_t Total[44] = {  };
	//建立TCP伪首部
	int psenum = 0;
	for (int i = 0; i < 4; i++)
	{
		TCPpse[psenum++] = SourceIP[i];
	}
	for (int i = 0; i < 4; i++)
		TCPpse[psenum++] = DestinationIP[i];
	TCPpse[psenum++] = 0x00;//zeros
	TCPpse[psenum++] = 0x06;//protocol:TCP
	TCPpse[psenum++] = 0x00;
	TCPpse[psenum++] = 32;//TCPlength

	//输出伪首部各字段值：
	printf("--------TCP伪首部各字段值--------\n");
	printf("源IP地址：%d.%d.%d.%d\n", TCPpse[0], TCPpse[1], TCPpse[2], TCPpse[3]);
	printf("目的IP地址：%d.%d.%d.%d\n", TCPpse[4], TCPpse[5], TCPpse[6], TCPpse[7]);
	printf("空字段：%02x\n", TCPpse[8]);
	printf("协议字段：%02x\n", TCPpse[9]);
	printf("长度字段:%02x %02x\n", TCPpse[10], TCPpse[11]);
	//输出TCP首部各字段值：
	printf("--------TCP首部各字段值--------\n");
	unsigned int sp = TCPSeg[0] << 8;
	sp += TCPSeg[1];
	printf("源端口：%d\n", sp);
	sp = TCPSeg[2] << 8;
	sp += TCPSeg[3];
	printf("目的端口：%d\n", sp);
	unsigned long long int num = TCPSeg[4];
	for (int i = 5; i <= 7; i++)
	{
		num = num << 8;
		num += TCPSeg[i];
	}
	printf("序列号：%llu\n", num);
	num = TCPSeg[8];
	for (int i = 9; i <= 11; i++)
	{
		num = num << 8;
		num += TCPSeg[i];
	}
	printf("确认号：%llu\n", num);
	unsigned int daof = TCPSeg[12] / 16;
	printf("Data offset：%02x\n", daof);
	unsigned int res = TCPSeg[12] % 16;
	printf("Reserved：%02x\n", res);
	printf("Flags：%02x\n", TCPSeg[13]);
	printf("Windows：%02x %02x\n", TCPSeg[14], TCPSeg[15]);
	printf("校验和：0x%x%x\n", TCPSeg[16], TCPSeg[17]);
	printf("Urgent pointer：%02x %02x\n", TCPSeg[18], TCPSeg[19]);
	//计算校验和
	TCPSeg[16] = 0, TCPSeg[17] = 0;
	int totallength = 32 + 12;
	int len = 0;
	for (len = 0; len < 32; len++)
		Total[len] = TCPSeg[len];
	for (int i = 0; i < 12; i++)
		Total[len++] = TCPpse[i];
	if (totallength % 2 != 0)
		Total[len++] = 0;

	uint16_t tcp_cksum = 0;
	tcp_cksum = gettcpcheck(Total, sizeof(Total));
	printf("TCP header checksum = %#x\n", tcp_cksum);
}
