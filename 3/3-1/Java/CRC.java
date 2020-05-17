import java.util.Arrays;

public class CRC {
	private String gxStr = "10001000000100001";

	public String GetRemainderStr(String dividendStr, String divisorStr) {
		int dividendLen = dividendStr.length();
		int divisorLen = divisorStr.length();
		for (int i = 0; i < divisorLen - 1; i++) {
			dividendStr += "0";
		}
		char str1[] = dividendStr.toCharArray();
		char str2[] = divisorStr.toCharArray();
		for (int i = 0; i < dividendLen; i++) {
			if (str1[i] == '1') {
				str1[i] = '0';
				for (int j = 1; j < divisorLen; j++) {
					if (str1[i + j] == str2[j]) {
						str1[i + j] = '0';
					} else {
						str1[i + j] = '1';
					}
				}
			}
		}
		String remainderStr = Arrays.toString(str1).replaceAll(", ", "").substring(dividendLen + 1,
				dividendLen + divisorLen);
		return remainderStr;
	}

	public String Send() {
		String dataStr = "01100000000000111110000000000110";
		System.out.println("待发送的数据信息二进制比特串为：" + dataStr);
		System.out.println("CRC-CCITT对应的二进制比特串为：" + gxStr);

		String remainderStr = GetRemainderStr(dataStr, gxStr);

		String crcStr = remainderStr;
		String sendFrameStr = dataStr + remainderStr;
		
		System.out.println("生成的CRC-Code为: " + crcStr);
		System.out.println("带校验和的发送帧为: " + sendFrameStr);
		System.out.println();
		return sendFrameStr;
	}

	public void Receive(String sendFrameStr) {
		int sendFrameLen = sendFrameStr.length();
		int gxLen = gxStr.length();

		String dataStr = sendFrameStr.substring(0, sendFrameLen - gxLen + 1);
		String crcStr = sendFrameStr.substring(sendFrameLen - gxLen + 1);
		System.out.println("接收的数据信息二进制比特串为：" + dataStr);
		System.out.println("生成的CRC-Code为: " + crcStr);

		String remainderStr = GetRemainderStr(sendFrameStr, gxStr);
		
		long remainder = Long.parseLong(remainderStr, 2);
		System.out.println("余数为: " + remainder);
		if (remainder == 0) {
			System.out.println("校验成功");
		} else {
			System.out.println("校验错误");
		}
	}

	public static void main(String[] args) {
		CRC operation = new CRC();
		String frameStr = operation.Send();
		operation.Receive(frameStr);
	}

}

