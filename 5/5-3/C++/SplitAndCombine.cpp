#include <stdio.h>
#include <string>
#include <algorithm>
#include <iostream>
#define bufferLen 10
using namespace std;

class SplitAndCombine
{
private:
	int headLen = 20;
	int bigIPTotalLen = 4000;
	int ID = 666;
	int MTU = 1500;

	int totalLenPos = 0;
	int IDPos = 1;
	int MFPos = 2;
	int DFPos = 3;
	int offsetPos = 4;

	int isEnd = 0;
	int isNotEnd = 1;
	int couldFragment = 0;
	int MAX = 9999;

	int original[5];
	int buffer[bufferLen][5];
	int assemble[5];

public:
	void Initialize()
	{
		original[totalLenPos] = bigIPTotalLen;
		original[IDPos] = ID;
		original[MFPos] = isEnd;
		original[DFPos] = couldFragment;
		original[offsetPos] = 0;
		printf("For original datagram, TotalLen | ID | MF | DF | Offset: %d | %d | %d | %d | %d\n\n",
			original[totalLenPos], original[IDPos], original[MFPos], original[DFPos], original[offsetPos]);
		for (int i = 0; i < bufferLen; i++)
		{
			buffer[i][offsetPos] = MAX;
		}
	}

	void Split()
	{
		int remainder = original[totalLenPos] - headLen;
		cout << "The biggest MTU: " << MTU << endl;
		int number = 0;
		while (remainder > 0) 
		{
			if (remainder > (MTU - headLen)) 
			{
				buffer[number][totalLenPos] = MTU;
				buffer[number][IDPos] = original[IDPos];
				buffer[number][MFPos] = isNotEnd;
				buffer[number][DFPos] = couldFragment;
				buffer[number][offsetPos] = ((original[totalLenPos] - headLen) - remainder) / 8;
				remainder -= (MTU - headLen);
			}
			else 
			{
				buffer[number][totalLenPos] = remainder + headLen;
				buffer[number][IDPos] = original[IDPos];
				buffer[number][MFPos] = original[MFPos];
				buffer[number][DFPos] = couldFragment;
				buffer[number][offsetPos] = ((original[totalLenPos] - headLen) - remainder) / 8;
				remainder = 0;
			}
			number++;
		}
		cout << "The number of sub-datagram: " << number << endl;
		cout << "The infomation of each sub-datagram: " << endl;
		printf("TotalLen\tID\tMF\tDF\tOffset\n");
		for (int i = 0; i < number; i++)
		{
			printf("%d\t\t%d\t%d\t%d\t%d\n", buffer[i][totalLenPos], buffer[i][IDPos], buffer[i][MFPos],
				buffer[i][DFPos], buffer[i][offsetPos]);
		}
		cout << endl;
	}

	void Combine()
	{
		int number;
		for (number = 0; number < bufferLen; number++)
		{
			if (buffer[number][MFPos] == isEnd)
			{
				number++;
				break;
			}
		}
		cout << "Be ready to assemble, the number of sub-datagram: " << number << endl;
		cout << "The infomation of each sub-datagram: " << endl;
		printf("TotalLen\tID\tMF\tDF\tOffset\n");
		for (int i = 0; i < number; i++)
		{
			printf("%d\t\t%d\t%d\t%d\t%d\n", buffer[i][totalLenPos], buffer[i][IDPos], buffer[i][MFPos], buffer[i][DFPos],
				buffer[i][offsetPos]);
		}
		assemble[totalLenPos] = (buffer[number - 1][offsetPos] - buffer[0][offsetPos]) * 8
			+ buffer[number - 1][totalLenPos];
		assemble[IDPos] = buffer[0][IDPos];
		assemble[MFPos] = buffer[number - 1][MFPos];
		assemble[DFPos] = buffer[0][DFPos];
		assemble[offsetPos] = buffer[0][offsetPos];
		cout << endl;
		printf("After assembling, TotalLen | ID | MF | DF | Offset: %d | %d | %d | %d | %d\n", assemble[totalLenPos],
			assemble[IDPos], assemble[MFPos], assemble[DFPos], assemble[offsetPos]);
	}
};

int main()
{
	SplitAndCombine operation;
	operation.Initialize();
	operation.Split();
	operation.Combine();
	system("pause");
}
