//https://www.jianshu.com/p/8ace17859bd1

public class LS {
	public static int n = 5;
	private int adjMat[][] = new int[][] { 
		{ 0, 7, 99, 99, 10 }, 
		{ 7, 0, 1, 99, 8 }, 
		{ 99, 1, 0, 2, 99 },
		{ 99, 99, 2, 0, 2 }, 
		{ 10, 8, 99, 2, 0 } };
	private int distance[][] = new int[n][n];
	private int nextRouter[][] = new int[n][n];
	private String s = "ABCDEFGHIJKLMN";

	public void Initialize() {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				distance[i][j] = adjMat[i][j];
				nextRouter[i][j] = j;
			}
		}
	}

	public void printInitialLinkState() {
		System.out.println("Initial Link State For Each Router is:");
		for (int i = 0; i < n; i++) {
			System.out.println("Router " + s.charAt(i));
			for (int j = 0; j < n; j++) {
				if (adjMat[i][j] != 0 && adjMat[i][j] != 99) {
					System.out.println(s.charAt(j) + " " + adjMat[i][j]);
				}
			}
			System.out.println();
		}
	}

	void SPF(int source) {
		System.out.println("For Router " + s.charAt(source));
		int flag[] = new int[n];
		for (int j = 0; j < n; j++) {
			flag[j] = 0;
		}
		flag[source] = 1;
		for (int i = 0; i < n; i++) {
			int k = source;
			int min = 99;
			for (int j = 0; j < n; j++) {
				if (distance[source][j] < min && flag[j] == 0) {
					min = distance[source][j];
					k = j;
				}
			}
			if (k == source) {
				System.out.println();
				return;
			}
			System.out.printf("Step %d: %c", i, s.charAt(source));
			int next = source;
			while (nextRouter[next][k] != k) {
				System.out.printf("-%c", s.charAt(nextRouter[next][k]));
				next = nextRouter[next][k];
			}
			System.out.printf("-%c %d\n", s.charAt(k), min);
			flag[k] = 1;
			for (int j = 0; j < n; j++) {
				if (distance[source][k] + adjMat[k][j] < distance[source][j] && flag[j] == 0) {
					distance[source][j] = distance[source][k] + adjMat[k][j];
					int next2 = source;
					while (next2 != k) {
						nextRouter[next2][j] = nextRouter[next2][k];
						next2 = nextRouter[next2][k];
					}
				}
			}

		}

	}

	void printFinalRoutingTable() {
		System.out.println("Final Routing Table For Each Router is:");
		for (int i = 0; i < n; i++) {
			System.out.println("Router " + s.charAt(i));
			for (int j = 0; j < n; j++) {
				System.out.println(s.charAt(nextRouter[i][j]) + " " + distance[i][j]);
			}
			System.out.println();
		}
	}

	public static void main(String[] args) {
		LS operation = new LS();
		operation.Initialize();
		operation.printInitialLinkState();
		System.out.println("Shortest Way Of Each Step For Each Router is:");
		for (int i = 0; i < n; i++) {
			operation.SPF(i);
		}
		operation.printFinalRoutingTable();
	}

}
