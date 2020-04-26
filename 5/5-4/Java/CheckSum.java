/*
http://blog.sina.com.cn/s/blog_4ff642f101000byt.html
 */
public class CheckSum {
	private String headString = "4500003CB53040008006E251C0A80168D83AC8EE";

	public void Print() {
		System.out.println("IPHeader: " + headString);
		System.out.println("version: " + Integer.parseInt(headString.substring(0, 1), 16));
		System.out.println("IHL: " + Integer.parseInt(headString.substring(1, 2), 16));
		System.out.println("Differentiated services: " + Integer.parseInt(headString.substring(2, 4), 16));
		System.out.println("Total length: " + Integer.parseInt(headString.substring(4, 8), 16));
		System.out.println("Identification: " + Integer.parseInt(headString.substring(8, 12), 16));

		String binaryString = Integer.toBinaryString(Integer.parseInt(headString.substring(12, 16), 16));
		while (binaryString.length() < 16) {
			binaryString = "0" + binaryString;
		}
		System.out.println("DF: " + binaryString.charAt(1));
		System.out.println("MF: " + binaryString.charAt(2));
		System.out.println("Fragment offset: " + Integer.parseInt(binaryString.substring(3), 2));

		System.out.println("Time to live: " + Integer.parseInt(headString.substring(16, 18), 16));
		System.out.println("Protocol: " + Integer.parseInt(headString.substring(18, 20), 16));
		System.out.println("Header checksum: " + Integer.parseInt(headString.substring(20, 24), 16));
		int sourceFirst = Integer.parseInt(headString.substring(24, 26), 16);
		int sourceSecond = Integer.parseInt(headString.substring(26, 28), 16);
		int sourceThird = Integer.parseInt(headString.substring(28, 30), 16);
		int sourceFourth = Integer.parseInt(headString.substring(30, 32), 16);
		int destinationFirst = Integer.parseInt(headString.substring(32, 34), 16);
		int destinationSecond = Integer.parseInt(headString.substring(34, 36), 16);
		int destinationThird = Integer.parseInt(headString.substring(36, 38), 16);
		int destinationFourth = Integer.parseInt(headString.substring(38, 40), 16);
		System.out.printf("Source address: %d.%d.%d.%d\n", sourceFirst, sourceSecond, sourceThird, sourceFourth);
		System.out.printf("Destination address: %d.%d.%d.%d\n\n", destinationFirst, destinationSecond, destinationThird,
				destinationFourth);
	}

	public void calculateCheckSum() {
		int a[][] = new int[9][4];
		int pos = 0, num = 0;
		while (pos < headString.length()) {
			if (pos == 20) {
				pos += 4;
				continue;
			}
			int sub = 0;
			if (Character.isLetter(headString.charAt(pos))) {
				sub = headString.charAt(pos) - 'A' + 10;
			} else if (Character.isDigit(headString.charAt(pos))) {
				sub = headString.charAt(pos) - '0';
			}
			a[num / 4][num % 4] = sub;
			num++;
			pos++;
		}

		int b[] = new int[4];
		String checkSumString = "";
		int more = 0;
		for (int i = 3; i >= 0; i--) {
			int sum = more;
			for (int j = 0; j <= 8; j++) {
				sum += a[j][i];
			}
			b[i] = sum % 16;
			more = sum / 16;
		}
		b[3] += more;
		for (int i = 0; i < 4; i++) {
			b[i] = 15 - b[i];
			if (b[i] <= 9) {
				checkSumString += Character.toString((char) (b[i] + '0'));
			} else {
				checkSumString += Character.toString((char) (b[i] - 10 + 'A'));
			}
		}
		System.out.printf("Calculating checksum: " + checkSumString);
		if (checkSumString.equals(headString.substring(20, 24))) {
			System.out.println(", is identical with Header checksum.");
		} else {
			System.out.println(", is not identical with Header checksum.");
		}

	}

	public static void main(String[] args) {
		CheckSum operation = new CheckSum();
		operation.Print();
		operation.calculateCheckSum();
	}

}
