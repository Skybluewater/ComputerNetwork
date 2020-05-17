#include <stdio.h>
#include <string>
#include <iostream>
using namespace std;

class BitStuffing
{
private:
	string flagString = "01111110";
	string insertString = "11111";

public:
	string Send()
	{
		string dataStr = "01101111111111111111111111111110";

		string sendFrame = flagString + dataStr + flagString;
		int flagLen = flagString.length();
		int sendFrameLen = sendFrame.length();
		cout << "Frame start flag：" << sendFrame.substr(0, flagLen) << endl;
		cout << "Frame data info：" << sendFrame.substr(flagLen, sendFrameLen - flagLen - flagLen) << endl;
		cout << "Frame end flag：" << sendFrame.substr(sendFrameLen - flagLen) << endl;

		int insertLen = insertString.length();
		for (int i = flagLen; i < (sendFrame.length() - flagLen); i++)
		{
			if (!sendFrame.substr(i, insertLen).compare(insertString))
			{
				sendFrame = sendFrame.substr(0, i + insertLen) + "0" + sendFrame.substr(i + insertLen);
				i = i + insertLen;
			}
		}

		cout << "Send frame after BitStuffing：" << sendFrame << endl << endl;
		return sendFrame;
	}

	void Receive(string receiveFrame)
	{
		int flagLen = flagString.length();
		int insertLen = insertString.length();
		for (int i = flagLen; i < (receiveFrame.length() - flagLen); i++)
		{
			if (!receiveFrame.substr(i, insertLen).compare(insertString))
			{
				receiveFrame = receiveFrame.substr(0, i + insertLen) + receiveFrame.substr(i + insertLen + 1);
				i = i + insertLen - 1;
			}
		}
		cout << "Receive frame after deleting bit：" << receiveFrame << endl;
	}
};

int main()
{
	BitStuffing operation;
	string frameStr = operation.Send();
	operation.Receive(frameStr);
	system("pause");
}