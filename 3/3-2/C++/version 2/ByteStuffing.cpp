#include <stdio.h>
#include <string>
#include <iostream>
using namespace std;

class bytestuffing
{
private:
	string flagstring = "7e";
	string insertstring = "1b";

public:
	string send()
	{
		string datastr = "347d7e807e40aa7d";
		string sendframe = flagstring + datastr + flagstring;
		int flaglen = flagstring.length();
		int sendframelen = sendframe.length();
		cout << "frame start flag: " << sendframe.substr(0, flaglen) << endl;
		cout << "frame data info: " << sendframe.substr(flaglen, sendframelen - flaglen - flaglen) << endl;
		cout << "frame end flag: " << sendframe.substr(sendframelen - flaglen) << endl;

		int insertlen = insertstring.length();
		for (int i = flaglen; i < (sendframe.length() - flaglen); i++) 
		{
			if (!sendframe.substr(i, flaglen).compare(flagstring)) 
			{
				sendframe = sendframe.substr(0, i) + insertstring + sendframe.substr(i);
				i = i + insertlen + flaglen;
			}
		}

		cout << "send frame after bytestuffing: " << sendframe << endl << endl;
		return sendframe;
	}

	void receive(string receiveframe)
	{
		int flaglen = flagstring.length();
		int insertlen = insertstring.length();
		for (int i = flaglen; i < (receiveframe.length() - flaglen); i++) 
		{
			if (!receiveframe.substr(i, flaglen).compare(flagstring)) 
			{
				receiveframe = receiveframe.substr(0, i - insertlen) + receiveframe.substr(i);
				i = i + flaglen;
			}
		}
		cout << "receive frame after deleting byte: " << receiveframe << endl;
	}
};

int main()
{
	bytestuffing operation;
	string framestr = operation.send();
	operation.receive(framestr);
	system("pause");
}