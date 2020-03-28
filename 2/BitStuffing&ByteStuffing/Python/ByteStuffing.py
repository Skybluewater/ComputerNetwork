class ByteStuffing:
    flagString = "7E"
    insertString = "1B"

    def Send(self):
        dataStr = "347D7E807E40AA7D"
        sendFrame = self.flagString + dataStr + self.flagString
        flagLen = len(self.flagString)
        sendFrameLen = len(sendFrame)
        print("帧起始标志：" + sendFrame[: flagLen])
        print("帧数据信息：" + sendFrame[flagLen: sendFrameLen - flagLen])
        print("帧结束标志：" + sendFrame[sendFrameLen - flagLen:])

        insertLen = len(self.insertString)
        i = flagLen
        while (i < len(sendFrame) - flagLen):
            if (sendFrame[i: i + flagLen] == self.flagString):
                sendFrame = sendFrame[: i] + self.insertString + sendFrame[i:]
                i = i + insertLen + flagLen
            i += 1

        print("字节填充后的发送帧：" + sendFrame)
        print()
        return sendFrame

    def Receive(self, receiveFrame):
        flagLen = len(self.flagString)
        insertLen = len(self.insertString)
        i = flagLen
        while (i < len(receiveFrame) - flagLen):
            if (receiveFrame[i: i + flagLen] == self.flagString):
                receiveFrame = receiveFrame[: i - insertLen] + receiveFrame[i:]
                i += flagLen
            i += 1
        print("字节删除后的接收帧：" + receiveFrame)


if __name__ == '__main__':
    operation = ByteStuffing()
    frameStr = operation.Send()
    operation.Receive(frameStr)
