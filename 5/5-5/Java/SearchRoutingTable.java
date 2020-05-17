
/*
 https://blog.csdn.net/qq_34134278/article/details/83538898
 https://blog.csdn.net/gatieme/article/details/50989257?depth_1-utm_source=distribute.pc_relevant.none-task-blog-BlogCommendFromBaidu-1&utm_source=distribute.pc_relevant.none-task-blog-BlogCommendFromBaidu-1
 https://blog.csdn.net/qq_34902437/article/details/91346938
 Book Page: 340-345
 */
import java.util.Scanner;

public class SearchRoutingTable {
	private static int n = 4;
	private int IPAddress[][] = new int[n][4];
	private int subnetMask[][] = new int[n][4];
	private String nextHop[] = new String[] { "Interface0", "Interface1", "Router1", "Router2" };

	private int IPSubAddress[][] = new int[][] { { 135, 46, 56, 0 }, { 136, 46, 60, 0 }, { 192, 53, 40, 0 },
			{ 0, 0, 0, 0 } };
	private int prefix[] = new int[] { 22, 22, 23, 0 };

	public void Initialize() {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < 4; j++) {
				IPAddress[i][j] = Integer.parseInt(Integer.toBinaryString(IPSubAddress[i][j]));
			}
			String string = "";
			for (int j = 0; j < prefix[i]; j++) {
				string += "1";
			}
			for (int j = prefix[i]; j < 32; j++) {
				string += "0";
			}
			for (int j = 0; j < 4; j++) {
				subnetMask[i][j] = Integer.parseInt(string.substring(j * 8, j * 8 + 8));
			}
		}
	}

	public void printRoutingTable() {
		System.out.println("Routing Table:");
		System.out.println("IPAddress/Prefix\tNextHop");
		for (int i = 0; i < n; i++) {
			if (i == n - 1) {
				System.out.printf("0/0\t\t\t");
			} else {
				System.out.printf("%d", IPSubAddress[i][0]);
				for (int j = 1; j < 4; j++) {
					System.out.printf(".%d", IPSubAddress[i][j]);
				}
				System.out.printf("/%d\t\t", prefix[i]);
			}
			System.out.println(nextHop[i]);
		}
		System.out.println();
	}

	public void Search(String string) {
		String subIPAddress[] = string.split("\\.");
		int maxPrefixPos = n;
		int maxPrefixLen = -1;
		for (int i = 0; i < n; i++) {
			boolean flag = true;
			for (int j = 0; j < 4; j++) {
				int value1 = Integer.parseInt(String.valueOf(subnetMask[i][j]), 2);
				int value2 = Integer.parseInt(subIPAddress[j]);
				int value3 = Integer.parseInt(String.valueOf(IPAddress[i][j]), 2);
				if ((value1 & value2) != value3) {
					flag = false;
					break;
				}
			}
			if (flag == false) {
				System.out.printf("Searching line %d, not matched.\n", i);
			} else if (flag == true) {
				System.out.printf("Searching line %d, matched successfully.\n", i);
				if (prefix[i] > maxPrefixLen) {
					maxPrefixLen = prefix[i];
					maxPrefixPos = i;
				}
			}

		}
		System.out.println("Next hop: " + nextHop[maxPrefixPos]);
	}

	public static void main(String[] args) {
		@SuppressWarnings("resource")
		Scanner scanner = new Scanner(System.in);
		System.out.println(
				"Please input Destination IPAddress of datagram as format 'num1.num2.num3.num4' such as '192.53.41.128'(no ''):");
		String string = scanner.nextLine();
		System.out.println();
		SearchRoutingTable operation = new SearchRoutingTable();
		operation.Initialize();
		operation.printRoutingTable();
		operation.Search(string);
	}

}
