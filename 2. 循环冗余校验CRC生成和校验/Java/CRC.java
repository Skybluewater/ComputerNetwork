import java.util.Scanner;
import java.lang.Math;

public class CRC {
	private String gxStr = "1101";

	public String GetRemainderStr(String dividendStr, String divisorStr) {
		int dividendLen = dividendStr.length();
		int divisorLen = divisorStr.length();
		long dividend = Long.parseLong(dividendStr, 2);
		long divisor = Long.parseLong(divisorStr, 2);
		dividend = dividend << (divisorLen - 1);
		divisor = divisor << (dividendLen - 1);
		int sumLen = dividendLen + divisorLen - 1;
		long flag = (long) Math.pow(2, sumLen - 1);

		for (int i = 0; i < dividendLen; i++) {
			// 判断补零后的帧最高位为1还是零
			if ((dividend & flag) != 0) {
				dividend = dividend ^ divisor;
				divisor = divisor >> 1;
			} else {
				divisor = divisor >> 1;
			}
			// flag最高位的1右移
			flag = flag >> 1;
		}

		String remainderStr = Long.toBinaryString(dividend);
		while (remainderStr.length() < (divisorLen - 1)) {
			remainderStr = "0" + remainderStr;
		}
		return remainderStr;
	}

	public String Send() {
		System.out.print("请输入待发送的数据信息二进制比特串: ");
		Scanner sc = new Scanner(System.in);
		String dataStr = sc.nextLine();
		sc.close();
		System.out.println("待发送的数据信息二进制比特串为：" + dataStr);
		System.out.println("CRC-CCITT对应的二进制比特串为：" + gxStr);

		String remainderStr = GetRemainderStr(dataStr, gxStr);

		long remainder = Long.parseLong(remainderStr, 2);
		long data = Long.parseLong(dataStr, 2);
		int dataLen = dataStr.length();
		int gxLen = gxStr.length();
		data = data << (gxLen - 1);
		System.out.println(Long.toBinaryString(data));
		String sendFrame = Long.toBinaryString(data ^ remainder);
		String crcStr = sendFrame.substring(dataLen);
		System.out.println("生成的CRC-Code为: " + crcStr);
		System.out.println("带校验和的发送帧为: " + sendFrame);
		System.out.println();
		return sendFrame;
	}

	public void Receive(String sendFrameStr) {
		System.out.println("接收的数据信息二进制比特串为：" + sendFrameStr);
		int sendFrameLen = sendFrameStr.length();
		int gxLen = gxStr.length();
		String crcStr = sendFrameStr.substring(sendFrameLen - gxLen + 1);
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
		String FrameStr = operation.Send();
		operation.Receive(FrameStr);
	}

}
