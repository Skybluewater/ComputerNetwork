#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#define MAX 30000
# define n 5
using namespace std;
class LSR{
private:
    int adjMat[100][100] = {
                { 0, 7, 99, 99, 10 },
                { 7, 0, 1, 99, 8 },
                { 99, 1, 0, 2, 99 },
                { 99, 99, 2, 0, 2 },
                { 10, 8, 99, 2, 0 } };
    int distance[n][n];
    char nextRouter[n][n];
    string s = "ABCDE";
public:
    void printInitialLinkState() {
        cout<<"Initial Link State For Each Router is:"<<endl;
        
        for (int i = 0; i < n; i++) {
            cout<<"Router "<<s[i]<<endl;
            for (int j = 0; j < n; j++) {
                if (adjMat[i][j] != 0 && adjMat[i][j] != 99) {
                    cout<<s[j]<<" "<<adjMat[i][j]<<endl;
                }
            }
            cout<<endl;
        }
    }
    void SPF(int source) {
        cout<<"For Router "<<s[source]<<endl;
        int flag[n];
        for (int j = 0; j < n; j++) {
            distance[source][j] = adjMat[source][j];
            nextRouter[source][j] = s[j];
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
                cout<<endl;
                return;
            }
            cout<<"Step"<<i<<":"<<s[source]<<"-"<<s[k]<<" "<<min<<endl;
            flag[k] = 1;
            for (int j = 0; j < n; j++) {
                if (distance[source][k] + adjMat[k][j] < distance[source][j] && flag[j] == 0) {
                    distance[source][j] = distance[source][k] + adjMat[k][j];
                    nextRouter[source][j] = nextRouter[source][k];
                }
            }
        }
    }

    void printFinalRoutingTable() {
        cout<<"Final Routing Table For Each Router is:"<<endl;
        for (int i = 0; i < n; i++) {
            cout<<"Router "<<s[i];
            for (int j = 0; j < n; j++) {
                cout<<nextRouter[i][j]<<" "<<distance[i][j];
            }
            cout<<endl;
        }
    }
};
int main() {
       LSR operation;
       operation.printInitialLinkState();
    cout<<"Shortest Way Of Each Step For Each Router is:"<<endl;
       for (int i = 0; i < n; i++) {
           operation.SPF(i);
       }
       operation.printFinalRoutingTable();
   }

