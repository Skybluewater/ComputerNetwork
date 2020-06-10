public class TCPChecksum {




    public static void main(String[] args)
    {
        short SourceIP[] = { 192,168,1,108 };
        short DestinationIP[] = { 61, 162, 100, 28 };
        short TCPSeg[] = { 0xe6, 0xd1, 0x00, 0x50, 0x9c, 0xbf, 0xea, 0xa9, 0x00, 0x00, 0x00, 0x00, 0x80, 0x02, 0xfa, 0xf0, 0xa1, 0xc1, 0x00, 0x00, 0x02, 0x04, 0x05, 0xb4, 0x01, 0x03, 0x03, 0x08, 0x01, 0x01, 0x04, 0x02 };
        short TCPpse[] = new short[12];
        short Total[] = new short[44];
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
        System.out.printf("--------TCP伪首部各字段值--------\n");
        System.out.printf("源IP地址：%d.%d.%d.%d\n", TCPpse[0], TCPpse[1], TCPpse[2], TCPpse[3]);
        System.out.printf("目的IP地址：%d.%d.%d.%d\n", TCPpse[4], TCPpse[5], TCPpse[6], TCPpse[7]);
        System.out.printf("空字段：%02x\n", TCPpse[8]);
        System.out.printf("协议字段：%02x\n", TCPpse[9]);
        System.out.printf("长度字段:%02x %02x\n", TCPpse[10], TCPpse[11]);
        //输出TCP首部各字段值：
        System.out.printf("--------TCP首部各字段值--------\n");
        int sp = TCPSeg[0] << 8;
        sp += TCPSeg[1];
        System.out.printf("源端口：%d\n", sp);
        sp = TCPSeg[2] << 8;
        sp += TCPSeg[3];
        System.out.printf("目的端口：%d\n", sp);
        int num = TCPSeg[4];
        for (int i = 5; i <= 7; i++)
        {
            num = num << 8;
            num += TCPSeg[i];
        }
        System.out.printf("序列号：%d\n", num);
        num = TCPSeg[8];
        for (int i = 9; i <= 11; i++)
        {
            num = num << 8;
            num += TCPSeg[i];
        }
        System.out.printf("确认号：%d\n", num);
        int daof = TCPSeg[12] / 16;
        System.out.printf("Data offset：%02x\n", daof);
        int res = TCPSeg[12] % 16;
        System.out.printf("Reserved：%02x\n", res);
        System.out.printf("Flags：%02x\n", TCPSeg[13]);
        System.out.printf("Windows：%02x %02x\n", TCPSeg[14], TCPSeg[15]);
        System.out.printf("校验和：0x%x%x\n", TCPSeg[16], TCPSeg[17]);
        System.out.printf("Urgent pointer：%02x %02x\n", TCPSeg[18], TCPSeg[19]);
        //计算校验和
        TCPSeg[16] = 0;
        TCPSeg[17] = 0;
        int totallength = 32 + 12;
        int len = 0;
        for (len = 0; len < 32; len++)
            Total[len] = TCPSeg[len];
        for (int i = 0; i < 12; i++)
            Total[len++] = TCPpse[i];
        if (totallength % 2 != 0)
            Total[len++] = 0;
        short tcp_cksum = 0;
        int cksum =0 ;
        int index =0;
        while (index < totallength) {
            cksum = cksum+((Total[index]<<8)+(Total[index+1]));
            index += 2;
        }
        cksum = cksum + (cksum>>16);
        cksum = ~cksum & 0xffff;
        tcp_cksum = (short)cksum;
        System.out.printf("TCP header checksum = %#x\n", tcp_cksum);
    }
}
