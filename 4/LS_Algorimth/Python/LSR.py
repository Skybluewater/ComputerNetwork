# https://www.jianshu.com/p/8ace17859bd1
class LSR:
    n = 5
    adjMat = [[0, 7, 99, 99, 10],
              [7, 0, 1, 99, 8],
              [99, 1, 0, 2, 99],
              [99, 99, 2, 0, 2],
              [10, 8, 99, 2, 0]]
    distance = [[0] * 5 for i in range(5)]
    nextRouter = [[' '] * 5 for i in range(5)]
    s = "ABCDE"

    def printInitialLinkState(self):
        print("Initial Link State For Each Router is:")
        for i in range(self.n):
            print("Router " + self.s[i])
            for j in range(self.n):
                if (self.adjMat[i][j] != 0 and self.adjMat[i][j] != 99):
                    print("{} {}".format(self.s[j], self.adjMat[i][j]))
            print()

    def SPF(self, source):
        print("For Router " + self.s[source])
        flag = [0 for i in range(self.n)]
        for j in range(self.n):
            self.distance[source][j] = self.adjMat[source][j]
            self.nextRouter[source][j] = self.s[j]
        flag[source] = 1
        for i in range(self.n):
            k = source
            min = 99
            for j in range(self.n):
                if (self.distance[source][j] < min and flag[j] == 0):
                    min = self.distance[source][j]
                    k = j
            if k == source:
                print()
                return
            print("Step {}: {}-{} {}".format(i, self.s[source], self.s[k], min))
            flag[k] = 1
            for j in range(self.n):
                if (self.distance[source][k] + self.adjMat[k][j] < self.distance[source][j] and flag[j] == 0):
                    self.distance[source][j] = self.distance[source][k] + self.adjMat[k][j]
                    self.nextRouter[source][j] = self.nextRouter[source][k]

    def printFinalRoutingTable(self):
        print("Final Routing Table For Each Router is:")
        for i in range(self.n):
            print("Router " + self.s[i])
            for j in range(self.n):
                print("{} {}".format(self.nextRouter[i][j], self.distance[i][j]))
            print()


if __name__ == '__main__':
    operation = LSR()
    operation.printInitialLinkState()
    print("Shortest Way Of Each Step For Each Router is:")
    for i in range(operation.n):
        operation.SPF(i)
    operation.printFinalRoutingTable()
