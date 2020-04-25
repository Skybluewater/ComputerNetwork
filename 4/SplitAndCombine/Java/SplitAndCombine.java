import java.util.Arrays;

public class SplitAndCombine {
	private int headLen = 20;
	private int bigIPTotalLen = 4000;
	private int ID = 666;
	private int MTU = 1500;

	private int totalLenPos = 0;
	private int IDPos = 1;
	private int MFPos = 2;
	private int offsetPos = 3;
	
	private int isEnd = 0;
	private int isNotEnd = 1;
	private int MAX = 9999;

	private int original[] = new int[4];
	private int buffer[][] = new int[10][4];
	private int assemble[] = new int[4];

	public void Initialize() {
		original[totalLenPos] = bigIPTotalLen;
		original[IDPos] = ID;
		original[MFPos] = isNotEnd;
		original[offsetPos] = 0;
		System.out.printf("For original datagram, TotalLen | ID | MF | Offset: %d | %d | %d | %d\n",
				original[totalLenPos], original[IDPos], original[MFPos], original[offsetPos]);
		for (int i = 0; i < buffer.length; i++) {
			buffer[i][offsetPos] = MAX;
		}
	}

	public void Split() {
		int remainder = original[totalLenPos] - headLen;
		System.out.println("Biggest MTU: " + MTU);
		int number = 0;
		while (remainder > 0) {
			if (remainder > (MTU - headLen)) {
				buffer[number][totalLenPos] = MTU;
				buffer[number][IDPos] = original[IDPos];
				buffer[number][MFPos] = isNotEnd;
				buffer[number][offsetPos] = ((original[totalLenPos] - headLen) - remainder) / 8;
				remainder -= (MTU - headLen);
			} else {
				buffer[number][totalLenPos] = remainder + headLen;
				buffer[number][IDPos] = original[IDPos];
				buffer[number][MFPos] = isEnd;
				buffer[number][offsetPos] = ((original[totalLenPos] - headLen) - remainder) / 8;
				remainder = 0;
			}
			number++;
		}
		System.out.println("The number of sub-datagram: " + number);
		System.out.println("The infomation of each sub-datagram: ");
		System.out.println("TotalLen\tID\tMF\tOffset");
		for (int i = 0; i < number; i++) {
			System.out.printf("%d\t\t%d\t%d\t%d\n", buffer[i][totalLenPos], buffer[i][IDPos], buffer[i][MFPos],
					buffer[i][offsetPos]);
		}
		System.out.println();
	}

	public void Combine() {
		Arrays.sort(buffer, (a, b) -> a[offsetPos] - b[offsetPos]);
		int number;
		for (number = 0; number < buffer.length; number++) {
			if (buffer[number][MFPos] == isEnd) {
				number++;
				break;
			}
		}
		System.out.println("Be ready to assemble, the number of sub-datagram: " + number);
		System.out.println("The infomation of each sub-datagram: ");
		System.out.println("TotalLen\tID\tMF\tOffset");
		for (int i = 0; i < number; i++) {
			System.out.printf("%d\t\t%d\t%d\t%d\n", buffer[i][totalLenPos], buffer[i][IDPos], buffer[i][MFPos],
					buffer[i][offsetPos]);
		}
		assemble[totalLenPos] = (buffer[number - 1][offsetPos] - buffer[0][offsetPos]) * 8
				+ buffer[number - 1][totalLenPos];
		assemble[IDPos] = buffer[0][IDPos];
		assemble[MFPos] = buffer[0][MFPos];
		assemble[offsetPos] = buffer[0][offsetPos];
		System.out.printf("After assembling, TotalLen | ID | MF | Offset: %d | %d | %d | %d\n", assemble[totalLenPos],
				assemble[IDPos], assemble[MFPos], assemble[offsetPos]);
	}

	public static void main(String[] args) {
		SplitAndCombine operation = new SplitAndCombine();
		operation.Initialize();
		operation.Split();
		operation.Combine();
	}

}
