import os

class BitStuffing:
    flagString = "01111110"
    insertString = "11111"

    def Send(self):
        dataStr = "01101111111111111111111111111110"
        sendFrame = self.flagString + dataStr + self.flagString
        flagLen = len(self.flagString)
        sendFrameLen = len(sendFrame)
        print("Frame start flag: " + sendFrame[: flagLen])
        print("Frame data info: " + sendFrame[flagLen: sendFrameLen - flagLen])
        print("Frame end flag: " + sendFrame[sendFrameLen - flagLen:])

        insertLen = len(self.insertString)
        i = flagLen
        while (i < len(sendFrame) - flagLen):
            if (sendFrame[i: i + insertLen] == self.insertString):
                sendFrame = sendFrame[: i + insertLen] + "0" + sendFrame[i + insertLen:]
                i = i + insertLen + 1
            else:
                i += 1

        print("Send frame after BitStuffing: " + sendFrame)
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
        print("Receive frame after deleting bit: " + receiveFrame)


if __name__ == '__main__':
    operation = BitStuffing()
    frameStr = operation.Send()
    operation.Receive(frameStr)
    os.system('pause')