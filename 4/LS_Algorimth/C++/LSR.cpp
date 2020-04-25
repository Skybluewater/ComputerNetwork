#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#define MAX 30000
#define n 5
using namespace std;

class LSR {
private:
	int adjMat[100][100] = {
				{ 0, 7, 99, 99, 10 },
				{ 7, 0, 1, 99, 8 },
				{ 99, 1, 0, 2, 99 },
				{ 99, 99, 2, 0, 2 },
				{ 10, 8, 99, 2, 0 } };
	int distance[n][n];
	int nextRouter[n][n];
	string s = "ABCDE";
public:
	void Initialize() {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				distance[i][j] = adjMat[i][j];
				nextRouter[i][j] = j;
			}
		}
	}

	void printInitialLinkState() {
		cout << "Initial Link State For Each Router is:" << endl;
		for (int i = 0; i < n; i++) {
			cout << "Router " << s[i] << endl;
			for (int j = 0; j < n; j++) {
				if (adjMat[i][j] != 0 && adjMat[i][j] != 99) {
					cout << s[j] << " " << adjMat[i][j] << endl;
				}
			}
			cout << endl;
		}
	}

	void SPF(int source) {
		cout << "For Router " << s[source] << endl;
		int flag[n];
		for (int j = 0; j < n; j++) {
			flag[j] = 0;
		}
		flag[source] = 1;
		for (int i = 0; i < n; i++) {
			int k = source;
			int min = 99;
			for (int j = 0; j < n; j++) {
				if (distance[source][j] < min && flag[j] == 0) {
					min = distance[source][j];
					k = j;
				}
			}
			if (k == source) {
				cout << endl;
				return;
			}
			cout << "Step " << i << ": " << s[source];
			int next = source;
			while (nextRouter[next][k] != k) {
				cout << "-" << s[nextRouter[next][k]];
				next = nextRouter[next][k];
			}
			cout << "-" << s[k] << " " << min << endl;
			flag[k] = 1;
			for (int j = 0; j < n; j++) {
				if (distance[source][k] + adjMat[k][j] < distance[source][j] && flag[j] == 0) {
					distance[source][j] = distance[source][k] + adjMat[k][j];
					int next2 = source;
					while (next2 != k) {
						nextRouter[next2][j] = nextRouter[next2][k];
						next2 = nextRouter[next2][k];
					}
				}
			}

		}

	}

	void printFinalRoutingTable() {
		cout << "Final Routing Table For Each Router is:" << endl;
		for (int i = 0; i < n; i++) {
			cout << "Router " << s[i] << endl;
			for (int j = 0; j < n; j++) {
				cout << s[nextRouter[i][j]] << " " << distance[i][j] << endl;
			}
			cout << endl;
		}
	}

};

int main() {
	LSR operation;
	operation.Initialize();
	operation.printInitialLinkState();
	cout << "Shortest Way Of Each Step For Each Router is:" << endl;
	for (int i = 0; i < n; i++) {
		operation.SPF(i);
	}
	operation.printFinalRoutingTable();
}
