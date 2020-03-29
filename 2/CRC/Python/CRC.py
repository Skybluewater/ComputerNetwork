class CRC:
    gxStr = "10001000000100001"

    def GetRemainderStr(self, dividendStr, divisorStr):
        dividendLen = len(dividendStr)
        divisorLen = len(divisorStr)
        dividend = int(dividendStr, 2)
        divisor = int(divisorStr, 2)
        dividend = dividend << (divisorLen - 1)
        divisor = divisor << (dividendLen - 1)
        sumLen = dividendLen + divisorLen - 1
        flag = pow(2, sumLen - 1)

        for i in range(dividendLen):
            # 判断补零后的帧最高位为1还是零
            if ((dividend & flag) != 0):
                dividend = dividend ^ divisor
                divisor = divisor >> 1
            else:
                divisor = divisor >> 1
            # flag最高位的1右移
            flag = flag >> 1

        remainderStr = bin(dividend).replace('0b', '')
        while len(remainderStr) < (divisorLen - 1):
            remainderStr = "0" + remainderStr
        return remainderStr

    def Send(self):
        dataStr = input("请输入待发送的数据信息二进制比特串: ")
        print("待发送的数据信息二进制比特串为：" + dataStr)
        print("CRC-CCITT对应的二进制比特串为：" + self.gxStr)

        remainderStr = self.GetRemainderStr(dataStr, self.gxStr)

        crcStr = remainderStr
        sendFrameStr = dataStr + remainderStr

        print("生成的CRC-Code为: " + crcStr)
        print("带校验和的发送帧为: " + sendFrameStr)
        print()
        return sendFrameStr

    def Receive(self, sendFrameStr):
        sendFrameLen = len(sendFrameStr)
        gxLen = len(self.gxStr)
        dataStr = sendFrameStr[: sendFrameLen - gxLen + 1]
        crcStr = sendFrameStr[sendFrameLen - gxLen + 1: ]
        print("接收的数据信息二进制比特串为：" + dataStr)
        print("生成的CRC-Code为: " + crcStr)

        remainderStr = self.GetRemainderStr(sendFrameStr, self.gxStr)
        remainder = int(remainderStr, 2)
        print("余数为: ", remainder)
        if remainder == 0:
            print("校验成功")
        else:
            print("校验错误")


if __name__ == '__main__':
    operation = CRC()
    frameStr = operation.Send()
    operation.Receive(frameStr)
