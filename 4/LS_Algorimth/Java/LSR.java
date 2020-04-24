//https://www.jianshu.com/p/8ace17859bd1

public class LSR {
	public static int n = 5;
	private int adjMat[][] = new int[][] { 
			{ 0, 7, 99, 99, 10 }, 
			{ 7, 0, 1, 99, 8 }, 
			{ 99, 1, 0, 2, 99 },
			{ 99, 99, 2, 0, 2 }, 
			{ 10, 8, 99, 2, 0 } };
	private int distance[][] = new int[n][n];
	private char nextRouter[][] = new char[n][n];
	private String s = "ABCDE";
	
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
			distance[source][j] = adjMat[source][j];
			nextRouter[source][j] = s.charAt(j);
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
			System.out.printf("Step %d: %c-%c %d\n", i, s.charAt(source), s.charAt(k), min);
			flag[k] = 1;
			for (int j = 0; j < n; j++) {
				if (distance[source][k] + adjMat[k][j] < distance[source][j] && flag[j] == 0) {
					distance[source][j] = distance[source][k] + adjMat[k][j];
					nextRouter[source][j] = nextRouter[source][k];
				}
			}
		}
		
	}

	void printFinalRoutingTable() {
		System.out.println("Final Routing Table For Each Router is:");
		for (int i = 0; i < n; i++) {
			System.out.println("Router " + s.charAt(i));
			for (int j = 0; j < n; j++) {
				System.out.println(nextRouter[i][j] + " " + distance[i][j]);
			}
			System.out.println();
		}
	}

	public static void main(String[] args) {
		LSR operation = new LSR();
		operation.printInitialLinkState();
		System.out.println("Shortest Way Of Each Step For Each Router is:");
		for (int i = 0; i < n; i++) {
			operation.SPF(i);
		}
		operation.printFinalRoutingTable();
	}

}
