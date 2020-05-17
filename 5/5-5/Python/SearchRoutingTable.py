import os

class SearchRoutingTable:
    n = 4
    IPAddress = [[0] * 4 for i in range(4)]
    subnetMask = [[0] * 4 for i in range(4)]
    nextHop = ["Interface0", "Interface1", "Router1", "Router2"]

    IPSubAddress = [[135, 46, 56, 0], [136, 46, 60, 0], [192, 53, 40, 0], [0, 0, 0, 0]]
    prefix = [22, 22, 23, 0]

    def Initialize(self):
        for i in range(self.n):
            for j in range(4):
                self.IPAddress[i][j] = int(bin(self.IPSubAddress[i][j]).replace('0b', ''))
            string = ""
            for j in range(self.prefix[i]):
                string += "1"
            for j in range(self.prefix[i], 32):
                string += "0"
            for j in range(4):
                self.subnetMask[i][j] = int(string[j * 8: j * 8 + 8])

    def printRoutingTable(self):
        print("Routing Table:")
        print("IPAddress/Prefix\tNextHop")
        for i in range(self.n):
            if i == (self.n - 1):
                print("0/0\t\t\t", end='')
            else:
                print("%d" % self.IPSubAddress[i][0], end='')
                for j in range(1, 4):
                    print(".%d" % self.IPSubAddress[i][j], end='')
                print("/%d\t\t" % self.prefix[i], end='')
            print(self.nextHop[i])
        print()

    def Search(self, string):
        subIPAddress = string.split('.')
        maxPrefixPos = self.n
        maxPrefixLen = -1
        for i in range(self.n):
            flag = True
            for j in range(4):
                value1 = int(str(self.subnetMask[i][j]), 2)
                value2 = int(subIPAddress[j])
                value3 = int(str(self.IPAddress[i][j]), 2)
                if ((value1 & value2) != value3):
                    flag = False
                    break
            if flag == False:
                print("Searching line %d, not matched." % i)
            elif flag == True:
                print("Searching line %d, matched successfully." % i)
                if self.prefix[i] > maxPrefixLen:
                    maxPrefixLen = self.prefix[i]
                    maxPrefixPos = i
        print("Next hop: " + self.nextHop[maxPrefixPos])


if __name__ == '__main__':
    string = input(
        "Please input Destination IPAddress of datagram as format 'num1.num2.num3.num4' such as '192.53.41.128'(no ''):\n")
    print()
    operation = SearchRoutingTable()
    operation.Initialize()
    operation.printRoutingTable()
    operation.Search(string)
    os.system("pause")