import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.util.Arrays;
import java.text.SimpleDateFormat;
import java.util.Date;

public class UDPReceiver {
	private int sendPort = 8888;
	private int recePort = 9999;
	DatagramSocket ds;

	private int dataLen = 20;
	private int sendFrameLen = 1;
	private int receFrameLen = 25;
	private int serialPos = 0;
	private int dataStartPos = 1;
	private int crcStartPos = 21;
	private int isEndPos = 23;

	private int frameExpected = 0;

	public String getBinaryString(byte b[]) {
		String str = "";
		for (int i = 0; i < b.length; i++) {
			str += Integer.toBinaryString((b[i] & 0xFF) + 0x100).substring(1);
		}
		return str;
	}

	public String getRemainderStr(String dividendStr, String divisorStr) {
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

	public String getCRCString(String s) {
		String gxStr = "10001000000100001";
		return getRemainderStr(s, gxStr);
	}

	public void Receive() throws Exception {
		try {
			ds = new DatagramSocket(sendPort);
			InetAddress address = InetAddress.getByName(null);

			OutputStream os = new FileOutputStream(new File("D:\\desktop\\copyText.txt"));

			while (true) {
				byte[] receFrame = new byte[receFrameLen];
				DatagramPacket dp = new DatagramPacket(receFrame, receFrame.length);
				ds.receive(dp);

				if (receFrame[isEndPos] == 1) {
					System.out.println("文件全部接收完毕");
					break;
				}
				
				int serial = receFrame[serialPos];

				int index = dataStartPos;
				for (; index < dataStartPos + dataLen; index++) {
					if (receFrame[index] == 0)
						break;
				}

				byte[] data = new byte[index - dataStartPos + 2];
				System.arraycopy(receFrame, dataStartPos, data, 0, index - dataStartPos);
				System.arraycopy(receFrame, crcStartPos, data, index - dataStartPos, 2);
				String binaryStr = getBinaryString(data);
				String crcStr = getCRCString(binaryStr);

				if (Integer.parseInt(crcStr, 2) == 0) {
					Date t = new Date();
					SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
					System.out.println("当前时间为：" + df.format(t));
					System.out.println("frame_expected的值为：" + frameExpected);
					System.out.println("接收帧数据正确，接收帧的发送帧序号：" + serial);

					byte[] sendFrame = new byte[sendFrameLen];
					sendFrame[0] = (byte) (1 - frameExpected);
					DatagramPacket dp2 = new DatagramPacket(sendFrame, sendFrame.length, address, recePort);
					ds.send(dp2);
					System.out.println("已发送回确认帧，确认帧的确认序号为：" + (1 - frameExpected));
					System.out.println();	
					
					if ((byte) frameExpected == serial) {
						frameExpected = (frameExpected + 1) % 2;
						os.write(data, 0, index - dataStartPos);
					}
				} else {
					Date t = new Date();
					SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
					System.out.println("当前时间为：" + df.format(t));
					System.out.println("接收帧数据错误，不返回确认帧");
					System.out.println();
				}

			}

			os.close();
			ds.close();
		} catch (SocketException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}

	}

	public static void main(String[] args) throws Exception {
		UDPReceiver operation = new UDPReceiver();
		operation.Receive();
	}

}