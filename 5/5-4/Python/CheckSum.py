import os

class CheckSum:
    headString = "4500003CB53040008006E251C0A80168D83AC8EE"

    def Print(self):
        print("IPHeader: " + self.headString)
        print("version: %d" % int(self.headString[0: 1], 16))
        print("IHL: %d" % int(self.headString[1: 2], 16))
        print("Differentiated services: %d" % int(self.headString[2: 4], 16))
        print("Total length: %d" % int(self.headString[4: 8], 16))
        print("Identification: %d" % int(self.headString[8: 12], 16))

        binaryString = bin(int(self.headString[12: 16], 16)).replace('0b', '')
        while (len(binaryString) < 16):
            binaryString = "0" + binaryString
        print("DF: %c" % binaryString[1])
        print("MF: %c" % binaryString[2])
        print("Fragment offset: %d" % int(binaryString[3:], 2))

        print("Time to live: %d" % int(self.headString[16: 18], 16))
        print("Protocol: %d" % int(self.headString[18: 20], 16))
        print("Header checksum: %d" % int(self.headString[20: 24], 16))
        sourceFirst = int(self.headString[24: 26], 16)
        sourceSecond = int(self.headString[26: 28], 16)
        sourceThird = int(self.headString[28: 30], 16)
        sourceFourth = int(self.headString[30: 32], 16)
        destinationFirst = int(self.headString[32: 34], 16)
        destinationSecond = int(self.headString[34: 36], 16)
        destinationThird = int(self.headString[36: 38], 16)
        destinationFourth = int(self.headString[38: 40], 16)
        print("Source address: {}.{}.{}.{}".format(sourceFirst, sourceSecond, sourceThird, sourceFourth))
        print("Destination address: {}.{}.{}.{}".format(destinationFirst, destinationSecond, destinationThird,
                                                        destinationFourth))
        print()

    def calculateCheckSum(self):
        a = [[0] * 4 for i in range(9)]
        pos = 0
        num = 0
        while (pos < len(self.headString)):
            if pos == 20:
                pos += 4
                continue
            sub = 0
            if (self.headString[pos].isalpha()):
                sub = ord(self.headString[pos]) - ord('A') + 10
            elif (self.headString[pos].isdigit()):
                sub = ord(self.headString[pos]) - ord('0')
            a[int(num / 4)][int(num % 4)] = sub
            num += 1
            pos += 1

        b = [0 for i in range(4)]
        checkSumString = ""
        more = 0
        i = 3
        while (i >= 0):
            sum = more
            for j in range(9):
                sum += a[j][i]
            b[i] = sum % 16
            more = sum // 16
            i -= 1
        b[3] += more
        for i in range(4):
            b[i] = 15 - b[i]
            if b[i] <= 9:
                checkSumString += str(b[i])
            else:
                checkSumString += chr(b[i] - 10 + ord('A'))
        print("Calculating checksum: " + checkSumString, end='')
        if checkSumString == self.headString[20: 24]:
            print(", is identical with Header checksum.")
        else:
            print(", is not identical with Header checksum.")


if __name__ == '__main__':
    operation = CheckSum()
    operation.Print()
    operation.calculateCheckSum()
    os.system("pause")
