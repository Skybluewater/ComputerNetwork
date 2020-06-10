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
    TCPSeg = [ 0xe6, 0xd1, 0x00, 0x50, 0x9c, 0xbf, 0xea, 0xa9, 0x00, 0x00, 0x00, 0x00, 0x80, 0x02, 0xfa, 0xf0, 0xa1, 0xc1, 0x00, 0x00, 0x02, 0x04, 0x05, 0xb4, 0x01, 0x03, 0x03, 0x08, 0x01, 0x01, 0x04, 0x02]
    TCPpse = []
    for i in range(0,4):
        TCPpse.append(SourceIP[i])
    for i in range(0, 4):
        TCPpse.append(DestinationIP[i])
    TCPpse.append(0x00) #zeros
    TCPpse.append(0x06) #protocol: TCP
    TCPpse.append(0x00)
    TCPpse.append(32) # TCPlength
    #输出伪首部各字段值：
    print("--------TCP伪首部各字段值--------\n");
    print("源IP地址：%d.%d.%d.%d\n" %(TCPpse[0], TCPpse[1], TCPpse[2], TCPpse[3]));
    print("目的IP地址：%d.%d.%d.%d\n"%( TCPpse[4], TCPpse[5], TCPpse[6], TCPpse[7]));
    print("空字段：%02x\n"% (TCPpse[8]));
    print("协议字段：%02x\n"% (TCPpse[9]));
    print("长度字段:%02x %02x\n"% (TCPpse[10], TCPpse[11]));

    print("--------TCP首部各字段值--------\n");
    sp = TCPSeg[0] << 8;
    sp += TCPSeg[1];
    print("源端口：%d\n"% sp);
    sp = TCPSeg[2] << 8;
    sp += TCPSeg[3];
    print("目的端口：%d\n"% sp);
    num = TCPSeg[4];
    for i in range(5,8):
        num = num<<8
        num+=TCPSeg[i]
    print("序列号：%d\n" % num);
    num = TCPSeg[8];
    for i in range(9,12):
        num = num << 8;
        num += TCPSeg[i]
    print("确认号：%d\n"% num);
    daof = TCPSeg[12] >>4
    print("Data offset：%02x\n" %daof);
    print("Reserved：%02x\n"% (TCPSeg[12] % 16));
    print("Flags：%02x\n"%TCPSeg[13]);
    print("Windows：%02x %02x\n"%( TCPSeg[14], TCPSeg[15]));
    print("校验和：0x%x%x\n"%( TCPSeg[16], TCPSeg[17]));
    print("Urgent pointer：%02x %02x\n"% (TCPSeg[18], TCPSeg[19]));
    TCPSeg[16] = 0
    TCPSeg[17] = 0
    res = chksum(TCPSeg+TCPpse)
    print('0x%x' % res)
main()