def chksum(source :bytes):
	if len(source)%2:
		source += b'\x00'
	sum=0
	for i in range(0,len(source),2):
		sum = sum+((source[i]<<8)+(source[i+1]))
	sum = sum + (sum>>16)
	return ~sum & 0xffff

def main():
    SourceIP = [192, 168, 1, 108]
    DestinationIP = [61, 162, 100, 28]
    UDPSeg = [ 0xcd, 0xdc, 0x01, 0xbb, 0x00, 0x46, 0x57, 0xcb ]
    UDPData = [0x40, 0x74, 0x7f, 0xb5, 0x3b, 0x57, 0xfd, 0xb5, 0x93, 0x0d, 0x68, 0x58, 0x22, 0x3b, 0x28, 0x4d,
                   0x1a, 0xac, 0x72, 0xa6, 0x0d, 0xb2, 0x8c, 0x10, 0xcc, 0x49, 0xcc, 0xcf, 0xdf, 0x5e, 0xd6, 0x7b, 0xff,
                   0xa2, 0xe0, 0x73,
                   0x99, 0x93, 0x8b, 0x2d, 0x93, 0xa9, 0x7e, 0x1c, 0xe4, 0xd2, 0xe0, 0xaf, 0x4a, 0xd5, 0xd0, 0x1f, 0x59,
                   0x2d, 0xa2, 0x63,
                   0x49, 0x5f, 0xe5, 0xea, 0x9d, 0x0c]
    UDPpse = []
    for i in range(0,4):
        UDPpse.append(SourceIP[i])
    for i in range(0, 4):
        UDPpse.append(DestinationIP[i])
    UDPpse.append(0x00) #zeros
    UDPpse.append(0x11) #protocol: UDP
    UDPpse.append(0x00)
    UDPpse.append(UDPSeg[5]) # UDPlength
    #输出伪首部各字段值：
    print("--------UDP伪首部各字段值--------\n");
    print("源IP地址：%d.%d.%d.%d\n" %(UDPpse[0], UDPpse[1], UDPpse[2], UDPpse[3]));
    print("目的IP地址：%d.%d.%d.%d\n"%( UDPpse[4], UDPpse[5], UDPpse[6], UDPpse[7]));
    print("空字段：%02x\n"% (UDPpse[8]));
    print("协议字段：%02x\n"% (UDPpse[9]));
    print("长度字段:%02x %02x\n"% (UDPpse[10], UDPpse[11]));

    print("--------UDP首部各字段值--------\n");
    sp = UDPSeg[0] << 8;
    sp += UDPSeg[1];
    print("源端口：%d\n"% sp);
    sp = UDPSeg[2] << 8;
    sp += UDPSeg[3];
    print("目的端口：%d\n"% sp);
    leng = UDPSeg[4] << 8;
    leng += UDPSeg[5];
    print("UDP长度：%d\n" %leng);
    print("校验和：0x%x%x\n"%( UDPSeg[6], UDPSeg[7]));
    UDPSeg[6] = 0
    UDPSeg[7] = 0
    res = chksum(UDPSeg+UDPpse+UDPData)
    print('计算得到的校验和：0x%x' % res)
main()