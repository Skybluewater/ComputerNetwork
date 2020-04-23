import java.util.Scanner;

public class DV {
	// {{0, 7, 99, 99, 10}, {7, 0, 1, 99, 8}, {99, 1, 0, 2, 99}, {99, 99, 2, 0, 2}, {10, 8, 99, 2, 0}};
	// {{0, 1, 99, 99, 99}, {1, 0, 1, 99, 99}, {99, 1, 0, 1, 99}, {99, 99, 1, 0, 1}, {99, 99, 99, 1, 0}};
	// {{0, 4, 50}, {4, 0, 1}, {50, 1, 0}};
	// {{0, 1, 99}, {1, 0, 1}, {99, 1, 0}};
	private int n = 5;
	public static int adjMat[][] = new int[][] { { 0, 7, 99, 99, 10 }, { 7, 0, 1, 99, 8 }, { 99, 1, 0, 2, 99 },
			{ 99, 99, 2, 0, 2 }, { 10, 8, 99, 2, 0 } };
	public static int oldTableMat[][] = new int[][] { { 0, 7, 99, 99, 10 }, { 7, 0, 1, 99, 8 }, { 99, 1, 0, 2, 99 },
			{ 99, 99, 2, 0, 2 }, { 10, 8, 99, 2, 0 } };
	private char nextRouter[][] = new char[5][5];
	private String s = "ABCDE";

	public void printRoutingTable() {
		for (int i = 0; i < n; i++) {
			System.out.println("Router " + s.charAt(i));
			for (int j = 0; j < n; j++) {
				System.out.println(nextRouter[i][j] + " " + oldTableMat[i][j]);
			}
			System.out.println();
		}
	}

	public void printInitialRoutingTable() {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				if (oldTableMat[i][j] != 99) {
					nextRouter[i][j] = s.charAt(j);
				} else {
					nextRouter[i][j] = ' ';
				}
			}
		}
		System.out.println("Initial Routing Table For Each Router is:");
		printRoutingTable();
	}

	public boolean compare(int newTableMat[][]) {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				if (oldTableMat[i][j] != newTableMat[i][j]) {
					return false;
				}
			}
		}
		return true;
	}

	public void printAfterUpdateRoutingTable() {
		int time = 0;
		while (true) {
			time++;
			int newTableMat[][] = new int[n][n];
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < n; j++) {
					newTableMat[i][j] = oldTableMat[i][j];
				}
			}
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < n; j++) {
					if (i == j) {
						nextRouter[i][j] = s.charAt(i);
						continue;
					}
					int min = 99;
					char c = ' ';
					for (int k = 0; k < n; k++) {
						if (adjMat[i][k] != 0 && adjMat[i][k] != 99) {
							if (oldTableMat[k][j] != 99) {
								int temp = adjMat[i][k] + oldTableMat[k][j];
								if (temp < min) {
									min = temp;
									c = s.charAt(k);
								}
							}
						}
					}
					newTableMat[i][j] = min;
					nextRouter[i][j] = c;
				}
			}
			if (compare(newTableMat) == false) {
				for (int i = 0; i < n; i++) {
					for (int j = 0; j < n; j++) {
						oldTableMat[i][j] = newTableMat[i][j];
					}
				}
				// printRoutingTable();
			} else {
				System.out.printf("Convergence costs %d times of exchanging information.\n", time);
				System.out.println("After-update Routing Table For Each Router is:");
				printRoutingTable();
				break;
			}

		}
	}

	public static void main(String[] args) {
		DV operation = new DV();
		operation.printInitialRoutingTable();
		operation.printAfterUpdateRoutingTable();
		while (true) {
			Scanner scanner = new Scanner(System.in);
			System.out.println("Enter 0 to exit or enter 1 to upgrade topology network:");
			int a = scanner.nextInt();
			if (a == 0) {
				break;
			} else if (a == 1) {
				System.out.println(
						"Please input a string as format 'num1-num2 distance' such as '1-2 8'(no '') to upgrade a pair of nodes:");
				String more = scanner.nextLine();
				String s = scanner.nextLine();
				int num1 = s.charAt(0) - '0';
				int num2 = s.charAt(2) - '0';
				int distance = Integer.parseInt(s.substring(4));
				adjMat[num1][num2] = distance;
				adjMat[num2][num1] = distance;
				oldTableMat[num1][num2] = distance;
				oldTableMat[num2][num1] = distance;
				operation.printAfterUpdateRoutingTable();
			}

		}
	}
}
