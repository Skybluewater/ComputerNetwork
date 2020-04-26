class DVR:
    n = 5
    adjMat = [[0, 7, 99, 99, 10],
              [7, 0, 1, 99, 8],
              [99, 1, 0, 2, 99],
              [99, 99, 2, 0, 2], 
              [10, 8, 99, 2, 0]]
    oldTableMat = [[0] * 5 for i in range(5)]
    nextRouter = [[' '] * 5 for i in range(5)]
    s = "ABCDE"

    def printRoutingTable(self):
        for i in range(self.n):
            print("Router " + self.s[i])
            for j in range(self.n):
                print("{} {}".format(self.nextRouter[i][j], self.oldTableMat[i][j]))
            print()

    def printInitialRoutingTable(self):
        for i in range(self.n):
            for j in range(self.n):
                self.oldTableMat[i][j] = self.adjMat[i][j]
                if self.oldTableMat[i][j] != 99:
                    self.nextRouter[i][j] = self.s[j]
                else:
                    self.nextRouter[i][j] = ' '
        print("Initial Routing Table For Each Router is:")
        self.printRoutingTable()

    def compare(self, newTableMat):
        for i in range(self.n):
            for j in range(self.n):
                if self.oldTableMat[i][j] != newTableMat[i][j]:
                    return False
        return True

    def printAfterUpdateRoutingTable(self):
        time = 0
        while (True):
            time += 1
            newTableMat = [[0] * self.n for i in range(self.n)]
            for i in range(self.n):
                for j in range(self.n):
                    if i == j:
                        self.nextRouter[i][j] = self.s[i]
                        continue
                    min = 99
                    c = ' '
                    for k in range(self.n):
                        if (self.adjMat[i][k] != 0 and self.adjMat[i][k] != 99):
                            if self.oldTableMat[k][j] != 99:
                                temp = self.adjMat[i][k] + self.oldTableMat[k][j]
                                if temp < min:
                                    min = temp
                                    c = self.s[k]
                    newTableMat[i][j] = min
                    self.nextRouter[i][j] = c
            if (self.compare(newTableMat) == False):
                for i in range(self.n):
                    for j in range(self.n):
                        self.oldTableMat[i][j] = newTableMat[i][j]
            else:
                print("Convergence costs %d times of exchanging information." % time)
                print("After-update Routing Table For Each Router is:")
                self.printRoutingTable()
                break

if __name__ == '__main__':
    operation = DVR()
    operation.printInitialRoutingTable()
    operation.printAfterUpdateRoutingTable()
    while(True):
        a = input("Enter 0 to exit or enter 1 to upgrade topology network: ")
        if a == "0":
            break
        elif a == "1":
            s = input("Please input a string as format 'num1-num2 distance' such as '1-2 8'(no '') to upgrade a pair of nodes:\n")
            num1 = int(s[0])
            num2 = int(s[2])
            distance = int(s[4: ])
            operation.adjMat[num1][num2] = distance
            operation.adjMat[num2][num1] = distance
            operation.oldTableMat[num1][num2] = distance
            operation.oldTableMat[num2][num1] = distance
            operation.printAfterUpdateRoutingTable()
