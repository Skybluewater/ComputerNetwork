#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main()
{
	double RTTArray[] = { 26, 32, 24, 26, 26, 28, 26, 26, 28, 26, 26, 28, 30 };
	double Alpha = 0.125, Beita = 0.25;

	double RTT, RTTs, RTTd, RTO;
	RTT = RTTArray[0];
	RTTs = RTT;
	RTTd = RTT / 2;
	RTO = RTTs + 4 * RTTd;
	printf("Initial RTT: %lf\n", RTT);

	for (int i = 1; i < sizeof(RTTArray) / sizeof(RTTArray[0]); i++) 
	{
		RTT = RTTArray[i];
		RTTs = (1 - Alpha) * RTTs + Alpha * RTT;
		RTTd = (1 - Beita) * RTTd + Beita * fabs(RTTs - RTT);
		RTO = RTTs + 4 * RTTd;
		printf("Round: %d, RTT: %lf, RTTs: %lf, RTO: %lf\n", i, RTT, RTTs, RTO);
	}
	system("pause");
}