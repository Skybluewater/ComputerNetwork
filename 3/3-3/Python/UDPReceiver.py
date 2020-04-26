import socket
import time
import struct


class UDPReceiver:
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    clientAddress = ('127.0.0.1', 9999)
    serverAddress = ('127.0.0.1', 8888)
    s.bind(serverAddress)

    seqPos = 0
    dataStartPos = 1
    crcLen = 2
    frameExpected = 0

    def getBinaryString(self, s):
        binaryStr = ""
        for i in range(len(s)):
            subStr = bin(ord(s[i])).replace('0b', '')
            while len(subStr) < 8:
                subStr = "0" + subStr
            binaryStr += subStr
        return binaryStr

    def getRemainderStr(self, dividendStr, divisorStr):
        dividendLen = len(dividendStr)
        divisorLen = len(divisorStr)
        dividend = int(dividendStr, 2)
        divisor = int(divisorStr, 2)
        dividend = dividend << (divisorLen - 1)
        divisor = divisor << (dividendLen - 1)
        sumLen = dividendLen + divisorLen - 1
        flag = pow(2, sumLen - 1)

        for i in range(dividendLen):
            if ((dividend & flag) != 0):
                dividend = dividend ^ divisor
                divisor = divisor >> 1
            else:
                divisor = divisor >> 1
            flag = flag >> 1

        remainderStr = bin(dividend).replace('0b', '')
        while len(remainderStr) < (divisorLen - 1):
            remainderStr = "0" + remainderStr
        return remainderStr

    def getCRCString(self, s):
        gxStr = "10001000000100001"
        return self.getRemainderStr(s, gxStr)

    def Receive(self):
        f = open("D:\\desktop\\copyText.txt", 'wb')
        while True:
            receiveFrame, address = self.s.recvfrom(1024)

            if str(receiveFrame) != "b'end'":

                binaryStr = self.getBinaryString(receiveFrame[self.dataStartPos:].decode('ISO-8859-1'))
                crcStr = self.getCRCString(binaryStr)

                if (int(crcStr, 2) == 0):
                    print("当前时间为：" + time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()))
                    print("frame_expected的值为：%d" % self.frameExpected)
                    print("接收帧数据正确，接收帧的发送帧序号为：%d" % receiveFrame[self.seqPos])

                    ack = 1 - self.frameExpected
                    sendFrame = struct.pack('B', ack)
                    self.s.sendto(sendFrame, self.clientAddress)
                    print("已发送回确认帧，确认帧的确认序号为：%d" % ack)
                    print()

                    if (receiveFrame[self.seqPos] == self.frameExpected):
                        self.frameExpected = (self.frameExpected + 1) % 2
                        f.write(receiveFrame[self.dataStartPos: 0 - self.crcLen])

                else:
                    print("当前时间为：" + time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()))
                    print("接收帧数据错误，不返回确认帧")
                    print()

            else:
                print("文件全部接收完毕")
                break

        f.close()
        self.s.close()


if __name__ == '__main__':
    operation = UDPReceiver()
    frameStr = operation.Receive()
