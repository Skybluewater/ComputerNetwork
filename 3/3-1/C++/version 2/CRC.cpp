#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
using namespace std;

class CRC
{
private:
	string gxStr = "10001000000100001";
public:
	string GetRemainderStr(string dividendStr, string divisorStr)
	{
		int dividendLen = dividendStr.length();
		int divisorLen = divisorStr.length();
		for (int i = 0; i < divisorLen - 1; i++)
		{
			dividendStr += "0";
		}
		for (int i = 0; i < dividendLen; i++)
		{
			if (dividendStr[i] == '1')
			{
				dividendStr[i] = '0';
				for (int j = 1; j < divisorLen; j++)
				{
					if (dividendStr[i + j] == divisorStr[j])
					{
						dividendStr[i + j] = '0';
					}
					else
					{
						dividendStr[i + j] = '1';
					}
				}
			}
		}
		string remainderStr = dividendStr.substr(dividendLen, divisorLen);
		return remainderStr;
	}

	string Send()
	{
		string dataStr = "01100000000000111110000000000110";
		cout << "待发送的数据信息二进制比特串为：" + dataStr << endl;
		cout << "CRC-CCITT对应的二进制比特串为：" + gxStr << endl;

		string remainderStr = GetRemainderStr(dataStr, gxStr);

		string crcStr = remainderStr;
		string sendFrameStr = dataStr + remainderStr;

		cout << "生成的CRC-Code为: " << crcStr << endl;
		cout << "带校验和的发送帧为: " << sendFrameStr << endl << endl;
		return sendFrameStr;
	}

	void Receive(string sendFrameStr)
	{
		int sendFrameLen = sendFrameStr.length();
		int gxLen = gxStr.length();
		string dataStr = sendFrameStr.substr(0, sendFrameLen - gxLen + 1);
		string crcStr = sendFrameStr.substr(sendFrameLen - gxLen + 1);
		cout << "接收的数据信息二进制比特串为：" << dataStr << endl;
		cout << "生成的CRC-Code为: " + crcStr << endl;

		string remainderStr = GetRemainderStr(sendFrameStr, gxStr);

		int remainder = atoi(remainderStr.c_str());
		cout << "余数为: " << remainder << endl;
		if (remainder == 0)
		{
			cout << "校验成功" << endl;
		}
		else
		{
			cout << "校验错误" << endl;
		}
	}

};

int main()
{
	CRC operation;
	string frameStr = operation.Send();
	operation.Receive(frameStr);
	system("pause");
}
