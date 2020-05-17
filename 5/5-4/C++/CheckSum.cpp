#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
using namespace std;

class CheckSum
{
private:
	string headString = "4500003CB53040008006E251C0A80168D83AC8EE";
public:
	string getBinaryString(int a)
	{
		char s[100];
		_itoa_s(a, s, 2);
		return string(s);
	}

	void Print()
	{
		cout << "IPHeader: " << headString << endl;
		cout << "version: " << strtol(headString.substr(0, 1).c_str(), NULL, 16) << endl;
		cout << "IHL: " << strtol(headString.substr(1, 1).c_str(), NULL, 16) << endl;
		cout << "Differentiated services: " << strtol(headString.substr(2, 2).c_str(), NULL, 16) << endl;
		cout << "Total length: " << strtol(headString.substr(4, 4).c_str(), NULL, 16) << endl;
		cout << "Identification: " << strtol(headString.substr(8, 4).c_str(), NULL, 16) << endl;

		string binaryString = getBinaryString(strtol(headString.substr(12, 4).c_str(), NULL, 16));
		while (binaryString.length() < 16)
		{
			binaryString = "0" + binaryString;
		}
		cout << "DF: " << binaryString[1] << endl;
		cout << "MF: " << binaryString[2] << endl;
		cout << "Fragment offset: " << strtol(binaryString.substr(3).c_str(), NULL, 2);

		cout << "Time to live: " << strtol(headString.substr(16, 2).c_str(), NULL, 16) << endl;
		cout << "Protocol: " << strtol(headString.substr(18, 2).c_str(), NULL, 16) << endl;
		cout << "Header checksum: " << strtol(headString.substr(20, 4).c_str(), NULL, 16) << endl;

		int sourceFirst = strtol(headString.substr(24, 2).c_str(), NULL, 16);
		int sourceSecond = strtol(headString.substr(26, 2).c_str(), NULL, 16);
		int sourceThird = strtol(headString.substr(28, 2).c_str(), NULL, 16);
		int sourceFourth = strtol(headString.substr(30, 2).c_str(), NULL, 16);
		int destinationFirst = strtol(headString.substr(32, 2).c_str(), NULL, 16);
		int destinationSecond = strtol(headString.substr(34, 2).c_str(), NULL, 16);
		int destinationThird = strtol(headString.substr(36, 2).c_str(), NULL, 16);
		int destinationFourth = strtol(headString.substr(38, 2).c_str(), NULL, 16);
		printf("Source address: %d.%d.%d.%d\n", sourceFirst, sourceSecond, sourceThird, sourceFourth);
		printf("Destination address: %d.%d.%d.%d\n\n", destinationFirst, destinationSecond, destinationThird,
			destinationFourth);
	}

	void calculateCheckSum()
	{
		int a[9][4];
		int pos = 0, num = 0;
		while (pos < headString.length())
		{
			if (pos == 20)
			{
				pos += 4;
				continue;
			}
			int sub = 0;
			if (isalpha(headString[pos])) {
				sub = headString[pos] - 'A' + 10;
			}
			else if (isdigit(headString[pos]))
			{
				sub = headString[pos] - '0';
			}
			a[num / 4][num % 4] = sub;
			num++;
			pos++;
		}

		int b[4];
		string checkSumString = "";
		int more = 0;
		for (int i = 3; i >= 0; i--) 
		{
			int sum = more;
			for (int j = 0; j <= 8; j++) 
			{
				sum += a[j][i];
			}
			b[i] = sum % 16;
			more = sum / 16;
		}
		b[3] += more;
		for (int i = 0; i < 4; i++) 
		{
			b[i] = 15 - b[i];
			if (b[i] <= 9) 
			{
				checkSumString.append(1, (char)(b[i] + '0'));
			}
			else 
			{
				checkSumString.append(1, (char)(b[i] - 10 + 'A'));
			}
		}
		cout << "Calculating checksum: " << checkSumString;
		if (!checkSumString.compare(headString.substr(20, 4)))
		{
			cout << ", is identical with Header checksum." << endl;
		}
		else
		{
			cout << ", is not identical with Header checksum." << endl;
		}
	}
};

int main()
{
	CheckSum operation;
	operation.Print();
	operation.calculateCheckSum();
	system("pause");
}