import java.util.Arrays;

public class SplitAndCombine {
	private int headLen = 20;
	private int bigIPTotalLen = 4000;
	private int ID = 666;
	private int MTU = 1500;

	private int totalLenPos = 0;
	private int IDPos = 1;
	private int MFPos = 2;
	private int DFPos = 3;
	private int offsetPos = 4;

	private int isEnd = 0;
	private int isNotEnd = 1;
	private int couldFragment = 0;
	private int MAX = 9999;

	private int original[] = new int[5];
	private int buffer[][] = new int[10][5];
	private int assemble[] = new int[5];

	public void Initialize() {
		original[totalLenPos] = bigIPTotalLen;
		original[IDPos] = ID;
		original[MFPos] = isEnd;
		original[DFPos] = couldFragment;
		original[offsetPos] = 0;
		System.out.printf("For original datagram, TotalLen | ID | MF | DF | Offset: %d | %d | %d | %d | %d\n\n",
				original[totalLenPos], original[IDPos], original[MFPos], original[DFPos], original[offsetPos]);
		for (int i = 0; i < buffer.length; i++) {
			buffer[i][offsetPos] = MAX;
		}
	}

	public void Split() {
		int remainder = original[totalLenPos] - headLen;
		System.out.println("The biggest MTU: " + MTU);
		int number = 0;
		while (remainder > 0) {
			if (remainder > (MTU - headLen)) {
				buffer[number][totalLenPos] = MTU;
				buffer[number][IDPos] = original[IDPos];
				buffer[number][MFPos] = isNotEnd;
				buffer[number][DFPos] = couldFragment;
				buffer[number][offsetPos] = ((original[totalLenPos] - headLen) - remainder) / 8;
				remainder -= (MTU - headLen);
			} else {
				buffer[number][totalLenPos] = remainder + headLen;
				buffer[number][IDPos] = original[IDPos];
				buffer[number][MFPos] = original[MFPos];
				buffer[number][DFPos] = couldFragment;
				buffer[number][offsetPos] = ((original[totalLenPos] - headLen) - remainder) / 8;
				remainder = 0;
			}
			number++;
		}
		System.out.println("The number of sub-datagram: " + number);
		System.out.println("The infomation of each sub-datagram: ");
		System.out.println("TotalLen\tID\tMF\tDF\tOffset");
		for (int i = 0; i < number; i++) {
			System.out.printf("%d\t\t%d\t%d\t%d\t%d\n", buffer[i][totalLenPos], buffer[i][IDPos], buffer[i][MFPos],
					buffer[i][DFPos], buffer[i][offsetPos]);
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
		System.out.println("TotalLen\tID\tMF\tDF\tOffset");
		for (int i = 0; i < number; i++) {
			System.out.printf("%d\t\t%d\t%d\t%d\t%d\n", buffer[i][totalLenPos], buffer[i][IDPos], buffer[i][MFPos], buffer[i][DFPos],
					buffer[i][offsetPos]);
		}
		assemble[totalLenPos] = (buffer[number - 1][offsetPos] - buffer[0][offsetPos]) * 8
				+ buffer[number - 1][totalLenPos];
		assemble[IDPos] = buffer[0][IDPos];
		assemble[MFPos] = buffer[number - 1][MFPos];
		assemble[DFPos] = buffer[0][DFPos];
		assemble[offsetPos] = buffer[0][offsetPos];
		System.out.println();
		System.out.printf("After assembling, TotalLen | ID | MF | DF | Offset: %d | %d | %d | %d | %d\n", assemble[totalLenPos],
				assemble[IDPos], assemble[MFPos], assemble[DFPos], assemble[offsetPos]);
	}

	public static void main(String[] args) {
		SplitAndCombine operation = new SplitAndCombine();
		operation.Initialize();
		operation.Split();
		operation.Combine();
	}

}
