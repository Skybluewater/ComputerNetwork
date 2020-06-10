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

uint16_t getUDPcheck(uint8_t* data, uint32_t size)
{
	uint32_t cksum = 0;


	cksum = summing(data, 0, size);//UDP_head

	return cksum;
}

int main()
{
	uint8_t SourceIP[] = { 192,168,1,108 };
	uint8_t DestinationIP[] = { 61, 162, 100, 28 };
	uint8_t UDPSeg[8] = { 0xcd, 0xdc, 0x01, 0xbb, 0x00, 0x46, 0x57, 0xcb };
	uint8_t UDPData[62] = { 0x40, 0x74,0x7f, 0xb5, 0x3b, 0x57, 0xfd, 0xb5, 0x93, 0x0d, 0x68, 0x58, 0x22, 0x3b, 0x28, 0x4d,
		0x1a, 0xac, 0x72, 0xa6, 0x0d, 0xb2, 0x8c, 0x10, 0xcc, 0x49, 0xcc, 0xcf, 0xdf, 0x5e, 0xd6, 0x7b, 0xff, 0xa2, 0xe0, 0x73,
		0x99, 0x93, 0x8b, 0x2d, 0x93, 0xa9, 0x7e, 0x1c, 0xe4, 0xd2, 0xe0, 0xaf, 0x4a, 0xd5, 0xd0, 0x1f, 0x59, 0x2d, 0xa2, 0x63, 
		0x49, 0x5f, 0xe5, 0xea, 0x9d, 0x0c };
	uint8_t UDPpse[12] = {  };
	uint8_t Total[82] = {  };
	//建立UDP伪首部
	int psenum = 0;
	for (int i = 0; i < 4; i++)
	{
		UDPpse[psenum++] = SourceIP[i];
	}
	for (int i = 0; i < 4; i++)
		UDPpse[psenum++] = DestinationIP[i];
	UDPpse[psenum++] = 0x00;//zeros
	UDPpse[psenum++] = 0x11;//protocol:UDP
	UDPpse[psenum++] = 0x00;
	UDPpse[psenum++] = UDPSeg[5];//UDPlength

	//输出伪首部各字段值：
	printf("--------UDP伪首部各字段值--------\n");
	printf("源IP地址：%d.%d.%d.%d\n", UDPpse[0], UDPpse[1], UDPpse[2], UDPpse[3]);
	printf("目的IP地址：%d.%d.%d.%d\n", UDPpse[4], UDPpse[5], UDPpse[6], UDPpse[7]);
	printf("空字段：%02x\n", UDPpse[8]);
	printf("协议字段：%02x\n", UDPpse[9]);
	printf("长度字段:%02x %02x\n", UDPpse[10], UDPpse[11]);
	//输出UDP首部各字段值：
	printf("--------UDP首部各字段值--------\n");
	unsigned int sp = UDPSeg[0] << 8;
	sp += UDPSeg[1];
	printf("源端口：%d\n", sp);
	sp = UDPSeg[2] << 8;
	sp += UDPSeg[3];
	printf("目的端口：%d\n", sp);

	unsigned int leng = UDPSeg[4] << 8;
	leng += UDPSeg[5];
	printf("UDP长度：%d\n", leng);
	printf("校验和：0x%x%x\n", UDPSeg[6], UDPSeg[7]);
	//计算校验和
	UDPSeg[6] = 0, UDPSeg[7] = 0;
	int totallength = 70+12;
	int len = 0;
	for (len = 0; len < 8; len++)
		Total[len] = UDPSeg[len];
	for (int i = 0; i < 12; i++)
		Total[len++] = UDPpse[i];
	for (int i = 0; i < 62; i++)
		Total[len++] = UDPData[i];
	if (totallength % 2 != 0)
		Total[len++] = 0;

	uint16_t UDP_cksum = 0;
	UDP_cksum = getUDPcheck(Total, sizeof(Total));
	printf("UDP header checksum = %#x\n", UDP_cksum);
}
