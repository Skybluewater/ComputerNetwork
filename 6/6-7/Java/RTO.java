import java.lang.Math;

public class RTO {
	public static void main(String[] args) {
		double RTTArray[] = new double[] { 26, 32, 24, 26, 26, 28, 26, 26, 28, 26, 26, 28, 30 };
		double Alpha = 0.125, Beita = 0.25;
		
		double RTT, RTTs, RTTd, RTO;
		RTT = RTTArray[0];
		RTTs = RTT;
		RTTd = RTT / 2;
		RTO = RTTs + 4 * RTTd;
		System.out.printf("Initial RTT: %f\n", RTT);
		
		for (int i = 1; i < RTTArray.length; i++) {
			RTT = RTTArray[i];
			RTTs = (1 - Alpha) * RTTs + Alpha * RTT;
			RTTd = (1 - Beita) * RTTd + Beita * Math.abs(RTTs - RTT);
			RTO = RTTs + 4 * RTTd;
			System.out.printf("Round: %d, RTT: %f, RTTs: %f, RTO: %f\n", i, RTT, RTTs, RTO);
		}
	}
}
