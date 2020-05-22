#include <stdio.h>
#include <stdlib.h>
int main()
{
	int MSS = 1024;
	int Threshold = 32768;
	int TriAckRound = 16;
	int TimeoutRound = 22;
	int EndRound = 26;

	int cwnd = 1;
	int ssthresh = Threshold / MSS;
	printf("MSS: %d, Initial Threshold: %d\n", MSS, Threshold);
	printf("For conveience, change unit from byte to segment, so initial threshold is %d segments\n", ssthresh);
	for (int i = 1; i <= EndRound; i++)
	{
		printf("Round: %d, cwnd: %d, send data\n", i, cwnd);
		if (i == TriAckRound)
		{
			ssthresh = cwnd / 2;
			cwnd = ssthresh;
		}
		else if (i == TimeoutRound)
		{
			ssthresh = cwnd / 2;
			cwnd = 1;
		}
		else
		{
			if (cwnd < ssthresh)
			{
				cwnd *= 2;
			}
			else
			{
				cwnd++;
			}
		}
	}
	system("pause");
}