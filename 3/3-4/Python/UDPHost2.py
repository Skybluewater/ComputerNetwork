import socket
import time
import struct
import threading

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
host1_address = ('127.0.0.1', 9999)
host2_address = ('127.0.0.1', 8888)
s.bind(host2_address)

MAX_SEQ = 7
buffer = [[] for i in range(MAX_SEQ + 1)]
nbuffered = 0
number = [0 for i in range(MAX_SEQ + 1)]

nextFrameToSend = 0
ackExpected = 0
frameExpected = 0
rank = 0

needToResend = False
waitForResend = False
receiveInfoFinished = False
lastSendAck = False
receiveFirstInfo = False

readFileFlag = False
overtimeFlag = 0

class UDPHost1(threading.Thread):
    f1 = open("D:\\desktop\\SendText2.txt", "rb")
    f2 = open("D:\\desktop\\ReceiveText1.txt", "wb")

    infoLen = 20
    frameLen = 28
    kindPos = 0
    hasData = 1
    notHasData = 0
    seqPos = 1
    ackPos = 2
    notHasAck = 127
    infoFlagPos = 3
    lastInfoFrame = 0
    notLastInfoFrame = 1
    infoStartPos = 4
    crcStartPos = 24
    crcLen = 2

    filterRank = 0
    filterError = 10
    filterLost = 10
    firstError = 3
    firstLost = 8

    right = 0
    error = 1
    lost = 2

    def Inc(self, k):
        global MAX_SEQ
        if k < MAX_SEQ:
            return k + 1
        else:
            return 0

    def Between(self, a, b, c):
        if ((a <= b) and (b < c)) or ((c < a) and (a <= b)) or ((b < c) and (c < a)):
            return True
        else:
            return False

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

    def isInfoRight(self, info):
        binaryStr = self.getBinaryString(info)
        crcStr = self.getCRCString(binaryStr)
        if (int(crcStr, 2) == 0):
            return True
        else:
            return False

    def fetchNewInfo(self):
        global readFileFlag, rank
        info = self.f1.read(self.infoLen)
        if readFileFlag == False:
            if str(info) == "b''":
                readFileFlag = True
                return False
            else:
                binaryStr = self.getBinaryString(info.decode('ISO-8859-1'))
                crcStr = self.getCRCString(binaryStr)
                beginCrcInt = int(crcStr[0: 8], 2)
                nextCrcInt = int(crcStr[8: 16], 2)
                buffer[ackExpected] = info + struct.pack('B', beginCrcInt) + struct.pack('B', nextCrcInt)
                number[ackExpected] = rank
                rank += 1
                return True
        return False

    def run(self):
        global s, rank, MAX_SEQ, readFileFlag, nbuffered
        global ackExpected, nextFrameToSend, receiveInfoFinished, frameExpected, needToResend,  waitForResend, receiveFirstInfo

        for rank in range(MAX_SEQ + 1):
            info = self.f1.read(self.infoLen)
            if str(info) == "b''":
                readFileFlag = True
                break
            else:
                binaryStr = self.getBinaryString(info.decode('ISO-8859-1'))
                crcStr = self.getCRCString(binaryStr)
                beginCrcInt = int(crcStr[0: 8], 2)
                nextCrcInt = int(crcStr[8: 16], 2)
                buffer[rank] = info + struct.pack('B', beginCrcInt) + struct.pack('B', nextCrcInt)
                number[rank] = rank
        nbuffered = rank


        while (True):
            try:
                receiveFrame, address = s.recvfrom(1024)
                if receiveFrame[self.kindPos] == self.notHasData:
                    print("Current time: " + time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()))
                    print("Receive a frame, only has ack, the ack is: %d" % receiveFrame[-1])
                    print()
                    while (self.Between(ackExpected, receiveFrame[-1], nextFrameToSend)):
                        nbuffered -= 1
                        if self.fetchNewInfo() == True:
                            nbuffered += 1
                        ackExpected = self.Inc(ackExpected)
                        if nbuffered == 0:
                            print("SendFile was sended completed.")
                            print()
                    if (nbuffered == 0 and receiveInfoFinished == True):
                        break
                elif receiveFrame[self.kindPos] == self.hasData:
                    if receiveFrame[self.ackPos] != self.notHasAck:
                        while (self.Between(ackExpected, receiveFrame[-1], nextFrameToSend)):
                            nbuffered -= 1
                        if self.fetchNewInfo() == True:
                            nbuffered += 1
                        ackExpected = self.Inc(ackExpected)
                        if nbuffered == 0:
                            print("SendFile was sended completed.")
                            print()
                    verify = self.isInfoRight(receiveFrame[self.infoStartPos:])
                    if (receiveFrame[self.seqPos] == frameExpected and verigy == True):
                        self.f2.write(receiveFrame[self.infoStartPos: 0 - self.crcLen])
                        print("Current time: " + time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()))
                        if receiveFrame[self.ackPos] == self.notHasAck:
                            print(
                                "Receive a frame, both seq and info are right, doesn't has ack, seq | frame_expected are: {} | {}".format(
                                    receiveFrame[self.seqPos], frameExpected))
                        else:
                            print(
                                "Receive a frame, both seq and info are right, seq | ack | frame_expected are: {} | {} | {}".format(
                                    receiveFrame[self.seqPos], receiveFrame[self.ackPos, frameExpected]))
                        print()

                        frameExpected = self.Inc(frameExpected)
                        waitForResend = False
                        receiveFirstInfo = True

                        if (receiveFrame[self.infoFlagPos] == self.lastInfoFrame):
                            print("ReceiveFile was received completed.")
                            print()
                            receiveInfoFinished = True
                            if (nbuffered == 0):
                                break
                    else:
                        if (waitForResend == True):
                            continue
                        print("Current time: " + time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()))
                        if (receiveFrame[self.seqPos] != frameExpected and verify == False):
                            print("Receive a frame, both seq and info are wrong, wait for resend!")
                        elif (receiveFrame[self.seqPos] != frameExpected):
                            print("Receive a frame, seq is wrong, wait for resend!")
                        elif (verify == False):
                            print("Receive a frame, info is wrong, wait for resend!")
                        print()
                        waitForResend = True
            except Exception:
                print("Current time: " + time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()))
                print("Receiving is overtime, need to resend!")
                print()
                needToResend = True


    def fillAndSend(self, method):
        global nextFrameToSend, lastSendAck, receiveFirstInfo, frameExpected, MAX_SEQ, receiveInfoFinished
        global s, host1_address, number, ackExpected, readFileFlag, nbuffered

        sendFrame = struct.pack('B', self.notHasData)
        sendFrame += struct.pack('B', nextFrameToSend)
        if (lastSendAck == False and receiveFirstInfo == True):
            ack = (frameExpected + MAX_SEQ) % (MAX_SEQ + 1)
            sendFrame += struct.pack('B', ack)
            if receiveInfoFinished == True:
                lastSendAck = True
        else:
            sendFrame += struct.pack('B', self.notHasAck)

        if (readFileFlag == True and nbuffered == 1):
            sendFrame += struct.pack('B', self.lastInfoFrame)
        else:
            sendFrame += struct.pack('B', self.notLastInfoFrame)

        sendFrame += buffer[nextFrameToSend]

        if (method == self.error):
            # pre = int.from_bytes(sendFrame[-1], 'little')
            # sendFrame[-1] = ((pre + 1) % 256).to_bytes(2, 'little')
            sendFrame[-1] = (sendFrame[-1] + 1) % 256
            print("Current time: " + time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()))
            print("ack_expected | next_frame_to_send | frame_expected are: {} | {} | {}".format(ackExpected,
                                                                                                nextFrameToSend,
                                                                                                frameExpected))
            if sendFrame[self.ackPos] != self.notHasAck:
                print("Stimulate sending wrongly, the number and ack of the frame are: {} | {}".format(
                    number[nextFrameToSend], sendFrame[self.ackPos]))
            else:
                print("Stimulate sending wrongly, doesn't has ack, the number of the frame is: %d" % number[
                    nextFrameToSend])
            print()
            s.sendto(sendFrame, host1_address)
        elif (method == self.lost):
            print("Current time: " + time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()))
            print("ack_expected | next_frame_to_send | frame_expected are: {} | {} | {}".format(ackExpected,
                                                                                                nextFrameToSend,
                                                                                                frameExpected))
            if sendFrame[self.ackPos] != self.notHasAck:
                print("Stimulate sending lost, the number and ack of the frame are: {} | {}".format(
                    number[nextFrameToSend], sendFrame[self.ackPos]))
            else:
                print("Stimulate sending lost, doesn't has ack, the number of the frame is: %d" % number[
                    nextFrameToSend])
            print()
        elif (method == self.right):
            print("Current time: " + time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()))
            print("ack_expected | next_frame_to_send | frame_expected are: {} | {} | {}".format(ackExpected,
                                                                                                nextFrameToSend,
                                                                                                frameExpected))
            if sendFrame[self.ackPos] != self.notHasAck:
                print("Stimulate sending rightly, the number and ack of the frame are: {} | {}".format(
                    number[nextFrameToSend], sendFrame[self.ackPos]))
            else:
                print("Stimulate sending rightly, doesn't has ack, the number of the frame is: %d" % number[
                    nextFrameToSend])
            print()
            s.sendto(sendFrame, host1_address)

    def Send(self):
        global nbuffered, waitForResend, needToResend, nextFrameToSend, ackExpected, lastSendAck
        global s, host1_address
        while (nbuffered != 0):
            if waitForResend == True:
                continue
            if needToResend == True:
                needToResend = False
                nextFrameToSend = ackExpected
                i = 1
                while (i <= nbuffered):
                    self.fillAndSend(self.right)
                    self.filterRank += 1
                    nextFrameToSend = self.Inc(nextFrameToSend)
                    time.sleep(1)
                    if (needToResend == True):
                        i = 1
                        needToResend = False
                        nextFrameToSend = ackExpected
                continue

            if (self.filterRank - self.firstError) % self.filterError == 0:
                self.fillAndSend(self.error)
            elif (self.filterRank - self.firstLost) % self.filterLost == 0:
                self.fillAndSend(self.lost)
            else:
                self.fillAndSend(self.right)
            self.filterRank += 1
            nextFrameToSend = self.Inc(nextFrameToSend)
            time.sleep(1)

        while (lastSendAck == False):
            sendFrame = struct.pack('B', self.notHasData)
            ack = (frameExpected + MAX_SEQ) % (MAX_SEQ + 1)
            sendFrame += struct.pack('B', ack)
            print("Current time: " + time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()))
            print("Send a frame, only has ack, ack is: %d" % ack)
            print()
            s.sendto(sendFrame, host1_address)
            time.sleep(1)
            if receiveInfoFinished == True:
                lastSendAck = True


if __name__ == '__main__':
    s.settimeout(5)
    send = UDPHost1()
    receive = UDPHost1()

    receive.start()
    time.sleep(3)
    send.Send()