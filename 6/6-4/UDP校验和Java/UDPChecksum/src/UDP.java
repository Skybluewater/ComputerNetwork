public class UDP {




    public static void main(String[] args)
    {
        short SourceIP[] = { 192,168,1,108 };
        short DestinationIP[] = { 61, 162, 100, 28 };
        short UDPSeg[] = { 0xcd, 0xdc, 0x01, 0xbb, 0x00, 0x46, 0x57, 0xcb };
        short UDPData[] = { 0x40, 0x74,0x7f, 0xb5, 0x3b, 0x57, 0xfd, 0xb5, 0x93, 0x0d, 0x68, 0x58, 0x22, 0x3b, 0x28, 0x4d,
                0x1a, 0xac, 0x72, 0xa6, 0x0d, 0xb2, 0x8c, 0x10, 0xcc, 0x49, 0xcc, 0xcf, 0xdf, 0x5e, 0xd6, 0x7b, 0xff, 0xa2, 0xe0, 0x73,
                0x99, 0x93, 0x8b, 0x2d, 0x93, 0xa9, 0x7e, 0x1c, 0xe4, 0xd2, 0xe0, 0xaf, 0x4a, 0xd5, 0xd0, 0x1f, 0x59, 0x2d, 0xa2, 0x63,
                0x49, 0x5f, 0xe5, 0xea, 0x9d, 0x0c };
        short UDPpse[] = new short[12];
        short Total[] = new short[82];
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
        System.out.printf("--------UDP伪首部各字段值--------\n");
        System.out.printf("源IP地址：%d.%d.%d.%d\n", UDPpse[0], UDPpse[1], UDPpse[2], UDPpse[3]);
        System.out.printf("目的IP地址：%d.%d.%d.%d\n", UDPpse[4], UDPpse[5], UDPpse[6], UDPpse[7]);
        System.out.printf("空字段：%02x\n", UDPpse[8]);
        System.out.printf("协议字段：%02x\n", UDPpse[9]);
        System.out.printf("长度字段:%02x %02x\n", UDPpse[10], UDPpse[11]);
        //输出UDP首部各字段值：
        System.out.printf("--------UDP首部各字段值--------\n");
        int sp = UDPSeg[0] << 8;
        sp += UDPSeg[1];
        System.out.printf("源端口：%d\n", sp);
        sp = UDPSeg[2] << 8;
        sp += UDPSeg[3];
        System.out.printf("目的端口：%d\n", sp);
        int leng = UDPSeg[4] << 8;
        leng += UDPSeg[5];
        System.out.printf("UDP长度：%d\n", leng);
        System.out.printf("校验和：0x%x%x\n", UDPSeg[6], UDPSeg[7]);
        //计算校验和
        UDPSeg[6] = 0;
        UDPSeg[7] = 0;
        int totallength = 70 + 12;
        int len = 0;
        for (len = 0; len < 8; len++)
            Total[len] = UDPSeg[len];
        for (int i = 0; i < 12; i++)
            Total[len++] = UDPpse[i];
        for (int i = 0; i < 62; i++)
            Total[len++] = UDPData[i];
        if (totallength % 2 != 0)
            Total[len++] = 0;
        short UDP_cksum = 0;
        int cksum = 0;
        int index = 0;
        while (index < totallength) {
            cksum = cksum + ((Total[index] << 8) + (Total[index + 1]));
            index += 2;
        }
        cksum = cksum + (cksum >> 16);
        cksum = ~cksum & 0xffff;
        UDP_cksum = (short)cksum;
        System.out.printf("UDP header checksum = %#x\n", UDP_cksum);
    }
}
