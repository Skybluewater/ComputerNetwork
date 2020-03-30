import socket
import time
import struct


class UDP3Client:
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    clientAddress = ('127.0.0.1', 9999)
    serverAddress = ('127.0.0.1', 8888)
    s.bind(clientAddress)

    dataLen = 20
    nextFrameToSend = 0
    seq = 0
    filterSeq = 0
    filterError = 10
    filterLost = 10
    firstError = 3
    firstLost = 8

    right = 0
    error = 1
    lost = 2

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

    def waitForEvent(self):
        flag = True
        try:
            receiveFrame, address = self.s.recvfrom(1024)
        except Exception:
            print("当前时间为：" + time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()))
            print("接收超时！")
            flag = False

        if (flag == True):
            print("当前时间为：" + time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()))
            print("接收到确认帧，确认帧的确认序号为：%d" % receiveFrame[0])
            print()

        print()
        return flag

    def Print(self, method):
        print("当前时间为：" + time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()))
        print("next_frame_to_send的值为：%d" % self.nextFrameToSend)
        print("正在发送帧的编号为：%d" % self.seq)
        if method == self.right:
            print("模拟正确发送")
        elif method == self.error:
            print("模拟传输出错")
        elif method == self.lost:
            print("模拟帧丢失")
        print()

    def Send(self):
        self.s.settimeout(2)
        try:
            f = open("D:\\desktop\\text.txt", 'rb')
        except IOError:
            print("不能打开该文件")

        while True:
            data = f.read(self.dataLen)

            if str(data) == "b''":
                self.s.sendto('end'.encode('utf-8'), self.serverAddress)
                print("文件全部发送完毕")
                break
            else:
                self.seq += 1

                sendFrame = struct.pack('B', self.nextFrameToSend)
                sendFrame += data

                binaryStr = self.getBinaryString(data.decode('ISO-8859-1'))
                crcStr = self.getCRCString(binaryStr)

                beginCrcInt = int(crcStr[0: 8], 2)
                nextCrcInt = int(crcStr[8: 16], 2)
                wrong = (nextCrcInt + 1) % 128

                sendFrame = sendFrame + struct.pack('B', beginCrcInt) + struct.pack('B', nextCrcInt)

                mark = False
                while (mark == False):
                    if ((self.filterSeq - self.firstError) % self.filterError == 0):
                        newSendFrame = sendFrame[: -1] + struct.pack('B', wrong)
                        self.s.sendto(newSendFrame, self.serverAddress)
                        self.Print(self.error)
                        self.filterSeq += 1
                    elif ((self.filterSeq - self.firstLost) % self.filterLost == 0):
                        self.Print(self.lost)
                        self.filterSeq += 1
                    else:
                        self.s.sendto(sendFrame, self.serverAddress)
                        self.Print(self.right)
                        self.filterSeq += 1

                    # 调节传输速度
                    time.sleep(0.5)

                    mark = self.waitForEvent()
                    if (mark == True):
                        self.nextFrameToSend = (self.nextFrameToSend + 1) % 2

        f.close()
        self.s.close()


if __name__ == '__main__':
    operation = UDP3Client()
    frameStr = operation.Send()
