public class CongestionControl {
	private static int MSS = 1024;
	private static int Threshold = 32768;
	private static int TriAckRound = 16;
	private static int TimeoutRound = 22;
	private static int EndRound = 26;

	public static void main(String[] args) {
		int cwnd = 1;
		int ssthresh = Threshold / MSS;
		System.out.printf("MSS: %d, Initial Threshold: %d\n", MSS, Threshold);
		System.out.printf("For conveience, change unit from byte to segment, so initial threshold is %d segments\n",
				ssthresh);
		for (int i = 1; i <= EndRound; i++) {
			System.out.printf("Round: %d, cwnd: %d, send data\n", i, cwnd);
			if (i == TriAckRound) {
				ssthresh = cwnd / 2;
				cwnd = ssthresh;
			} else if (i == TimeoutRound) {
				ssthresh = cwnd / 2;
				cwnd = 1;
			} else {
				if (cwnd < ssthresh) {
					cwnd *= 2;
				} else {
					cwnd++;
				}
			}
		}
	}
}
