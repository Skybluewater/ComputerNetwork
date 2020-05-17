#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#define n 5
using namespace std;

class DV
{
private:
	char nextRouter[n][n];
	string s = "ABCDEFGHIJKLMN";
public:
	int adjMat[n][n] = {
		{ 0, 7, 99, 99, 10 },
		{ 7, 0, 1, 99, 8 },
		{ 99, 1, 0, 2, 99 },
		{ 99, 99, 2, 0, 2 },
		{ 10, 8, 99, 2, 0 } };
	int oldTableMat[n][n];

	void printRoutingTable()
	{
		for (int i = 0; i < n; i++)
		{
			cout << "Router " << s[i] << endl;
			for (int j = 0; j < n; j++)
			{
				cout << nextRouter[i][j] << " " << oldTableMat[i][j] << endl;
			}
			cout << endl;
		}
	}

	void printInitialRoutingTable()
	{
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				oldTableMat[i][j] = adjMat[i][j];
				if (oldTableMat[i][j] != 99)
				{
					nextRouter[i][j] = s[j];
				}
				else
				{
					nextRouter[i][j] = ' ';
				}
			}
		}
		cout << "Initial Routing Table For Each Router is: " << endl;
		printRoutingTable();
	}

	bool compare(int newTableMat[n][n]) 
	{
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				if (oldTableMat[i][j] != newTableMat[i][j])
				{
					return false;
				}
			}
		}
		return true;
	}

	void printAfterUpdateRoutingTable()
	{
		int time = 0;
		while (true)
		{
			time++;
			int newTableMat[n][n];
			memset(newTableMat, 0, sizeof(newTableMat));
			for (int i = 0; i < n; i++)
			{
				for (int j = 0; j < n; j++)
				{
					if (i == j)
					{
						nextRouter[i][j] = s[i];
						continue;
					}
					int min = 99;
					char c = ' ';
					for (int k = 0; k < n; k++)
					{
						if (adjMat[i][k] != 0 && adjMat[i][k] != 99)
						{
							if (oldTableMat[k][j] != 99)
							{
								int temp = adjMat[i][k] + oldTableMat[k][j];
								if (temp < min)
								{
									min = temp;
									c = s[k];
								}
							}
						}
					}
					newTableMat[i][j] = min;
					nextRouter[i][j] = c;
				}
			}
			if (compare(newTableMat) == false)
			{
				for (int i = 0; i < n; i++)
				{
					for (int j = 0; j < n; j++)
					{
						oldTableMat[i][j] = newTableMat[i][j];
					}
				}
			}
			else
			{
				printf("Convergence costs %d times of exchanging information.\n", time);
				cout << "After-update Routing Table For Each Router is: " << endl;
				printRoutingTable();
				break;
			}

		}
	}

};

int main()
{
	DV operation;
	operation.printInitialRoutingTable();
	operation.printAfterUpdateRoutingTable();
	while (true)
	{
		cout << "Enter 0 to exit or enter 1 to upgrade topology network:" << endl;
		int a;
		cin >> a;
		if (a == 0)
		{
			break;
		}
		else if (a == 1)
		{
			cout << "Please input as format 'num1 num2 distance' such as '0 1 8'(no '') to upgrade a pair of nodes:" << endl;
			int num1, num2, distance;
			cin >> num1 >> num2 >> distance;
			operation.adjMat[num1][num2] = distance;
			operation.adjMat[num2][num1] = distance;
			operation.oldTableMat[num1][num2] = distance;
			operation.oldTableMat[num2][num1] = distance;
			cout << endl;
			operation.printAfterUpdateRoutingTable();
		}

	}
	system("pause");
}