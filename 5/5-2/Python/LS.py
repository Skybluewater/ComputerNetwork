import os

class LS:
    n = 5
    adjMat = [[0, 7, 99, 99, 10],
              [7, 0, 1, 99, 8],
              [99, 1, 0, 2, 99],
              [99, 99, 2, 0, 2],
              [10, 8, 99, 2, 0]]
    distance = [[0] * 5 for i in range(5)]
    nextRouter = [[0] * 5 for i in range(5)]
    s = "ABCDEFGHIJKLMN"

    def Initialize(self):
        for i in range(self.n):
            for j in range(self.n):
                self.distance[i][j] = self.adjMat[i][j]
                self.nextRouter[i][j] = j

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
            print("Step {}: {}".format(i, self.s[source]), end='')
            next = source
            while (self.nextRouter[next][k] != k):
                print("-{}".format(self.s[self.nextRouter[next][k]]), end='')
                next = self.nextRouter[next][k]
            print("-{} {}".format(self.s[k], min))
            flag[k] = 1
            for j in range(self.n):
                if (self.distance[source][k] + self.adjMat[k][j] < self.distance[source][j] and flag[j] == 0):
                    self.distance[source][j] = self.distance[source][k] + self.adjMat[k][j]
                    next2 = source
                    while (next2 != k):
                        self.nextRouter[next2][j] = self.nextRouter[next2][k]
                        next2 = self.nextRouter[next2][k]

    def printFinalRoutingTable(self):
        print("Final Routing Table For Each Router is:")
        for i in range(self.n):
            print("Router " + self.s[i])
            for j in range(self.n):
                print("{} {}".format(self.s[self.nextRouter[i][j]], self.distance[i][j]))
            print()


if __name__ == '__main__':
    operation = LS()
    operation.Initialize()
    operation.printInitialLinkState()
    print("Shortest Way Of Each Step For Each Router is:")
    for i in range(operation.n):
        operation.SPF(i)
    operation.printFinalRoutingTable()
    os.system('pause')
