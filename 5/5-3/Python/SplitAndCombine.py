from operator import itemgetter


class SplitAndCombine:
    headLen = 20
    bigIPTotalLen = 4000
    ID = 666
    MTU = 1500

    totalLenPos = 0
    IDPos = 1
    MFPos = 2
    offsetPos = 3

    isEnd = 0
    isNotEnd = 1
    MAX = 999

    original = [0 for i in range(4)]
    buffer = [[0] * 4 for i in range(10)]
    assemble = [0 for i in range(4)]

    def Initialize(self):
        self.original[self.totalLenPos] = self.bigIPTotalLen
        self.original[self.IDPos] = self.ID
        self.original[self.MFPos] = self.isNotEnd
        self.original[self.offsetPos] = 0;
        print("For original datagram, TotalLen | ID | MF | Offset: {} | {} | {} | {}".format(
            self.original[self.totalLenPos], self.original[self.IDPos], self.original[self.MFPos],
            self.original[self.offsetPos]))
        for i in range(len(self.buffer)):
            self.buffer[i][self.offsetPos] = self.MAX

    def Split(self):
        remainder = self.original[self.totalLenPos] - self.headLen
        print("Biggest MTU: %d" % self.MTU)
        number = 0
        while (remainder > 0):
            if (remainder > (self.MTU - self.headLen)):
                self.buffer[number][self.totalLenPos] = self.MTU
                self.buffer[number][self.IDPos] = self.original[self.IDPos]
                self.buffer[number][self.MFPos] = self.isNotEnd
                self.buffer[number][self.offsetPos] = int(
                    ((self.original[self.totalLenPos] - self.headLen) - remainder) / 8)
                remainder -= (self.MTU - self.headLen)
            else:
                self.buffer[number][self.totalLenPos] = remainder + self.headLen
                self.buffer[number][self.IDPos] = self.original[self.IDPos]
                self.buffer[number][self.MFPos] = self.isEnd
                self.buffer[number][self.offsetPos] = int(
                    ((self.original[self.totalLenPos] - self.headLen) - remainder) / 8)
                remainder = 0
            number += 1
        print("The number of sub-datagram: %d" % number)
        print("The infomation of each sub-datagram: ")
        print("TotalLen\tID\tMF\tOffset")
        for i in range(number):
            print("{}\t\t{}\t{}\t{}".format(self.buffer[i][self.totalLenPos], self.buffer[i][self.IDPos],
                                            self.buffer[i][self.MFPos], self.buffer[i][self.offsetPos]))
        print()

    def Combine(self):
        sorted(self.buffer, key=itemgetter(self.offsetPos))
        number = 0
        for number in range(len(self.buffer)):
            if self.buffer[number][self.MFPos] == self.isEnd:
                number += 1
                break
        print("Be ready to assemble, the number of sub-datagram: %d" % number)
        print("The infomation of each sub-datagram: ")
        print("TotalLen\tID\tMF\tOffset")
        for i in range(number):
            print("{}\t\t{}\t{}\t{}".format(self.buffer[i][self.totalLenPos], self.buffer[i][self.IDPos],
                                            self.buffer[i][self.MFPos], self.buffer[i][self.offsetPos]))
        self.assemble[self.totalLenPos] = (self.buffer[number - 1][self.offsetPos] - self.buffer[0][
            self.offsetPos]) * 8 + self.buffer[number - 1][self.totalLenPos]
        self.assemble[self.IDPos] = self.buffer[0][self.IDPos]
        self.assemble[self.MFPos] = self.buffer[0][self.MFPos]
        self.assemble[self.offsetPos] = self.buffer[0][self.offsetPos]
        print("After assembling, TotalLen | ID | MF | Offset: {} | {} | {} | {}".format(
            self.assemble[self.totalLenPos], self.assemble[self.IDPos], self.assemble[self.MFPos],
            self.assemble[self.offsetPos]))


if __name__ == '__main__':
    operation = SplitAndCombine()
    operation.Initialize()
    operation.Split()
    operation.Combine()
