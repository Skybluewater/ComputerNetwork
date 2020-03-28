class BitStuffing:
    flagString = "01111110"
    insertString = "11111"

    def Send(self):
        dataStr = input("请输入待发送的数据信息二进制比特串: ")
        sendFrame = self.flagString + dataStr + self.flagString
        flagLen = len(self.flagString)
        sendFrameLen = len(sendFrame)
        print("帧起始标志：" + sendFrame[: flagLen])
        print("帧数据信息：" + sendFrame[flagLen: sendFrameLen - flagLen])
        print("帧结束标志：" + sendFrame[sendFrameLen - flagLen:])

        insertLen = len(self.insertString)
        i = flagLen
        while (i < len(sendFrame) - flagLen):
            if (sendFrame[i: i + insertLen] == self.insertString):
                sendFrame = sendFrame[: i + insertLen] + "0" + sendFrame[i + insertLen:]
                i = i + insertLen + 1
            else:
                i += 1

        print("比特填充后的发送帧：" + sendFrame)
        print()
        return sendFrame

    def Receive(self, receiveFrame):
        flagLen = len(self.flagString)
        insertLen = len(self.insertString)
        i = flagLen
        while (i < len(receiveFrame) - flagLen):
            if (receiveFrame[i: i + insertLen] == self.insertString):
                receiveFrame = receiveFrame[: i + insertLen] + receiveFrame[i + insertLen + 1:]
                i = i + insertLen
            else:
                i += 1
        print("比特删除后的接收帧：" + receiveFrame)


if __name__ == '__main__':
    operation = BitStuffing()
    frameStr = operation.Send()
    operation.Receive(frameStr)
