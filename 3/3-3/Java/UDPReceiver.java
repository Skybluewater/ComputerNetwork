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
	private int senderPort = 9999;
	private int receiverPort = 8888;
	DatagramSocket ds;

	private int dataLen = 20;
	private int sendFrameLen = 1;
	private int receiveFrameLen = 25;
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
			ds = new DatagramSocket(receiverPort);
			InetAddress address = InetAddress.getByName(null);

			OutputStream os = new FileOutputStream(new File("ReceiveText.txt"));

			while (true) {
				byte[] receiveFrame = new byte[receiveFrameLen];
				DatagramPacket dp = new DatagramPacket(receiveFrame, receiveFrame.length);
				ds.receive(dp);

				if (receiveFrame[isEndPos] == 1) {
					System.out.println("Receive the file finished.");
					break;
				}
				
				int serial = receiveFrame[serialPos];

				int index = dataStartPos;
				for (; index < dataStartPos + dataLen; index++) {
					if (receiveFrame[index] == 0)
						break;
				}

				byte[] data = new byte[index - dataStartPos + 2];
				System.arraycopy(receiveFrame, dataStartPos, data, 0, index - dataStartPos);
				System.arraycopy(receiveFrame, crcStartPos, data, index - dataStartPos, 2);
				String binaryStr = getBinaryString(data);
				String crcStr = getCRCString(binaryStr);

				if (Integer.parseInt(crcStr, 2) == 0) {
					Date t = new Date();
					SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
					System.out.println("Current time: " + df.format(t));
					System.out.println("frame_expected: " + frameExpected);
					System.out.println("Data of the frame is right, serial is: " + serial);

					byte[] sendFrame = new byte[sendFrameLen];
					sendFrame[0] = (byte) (1 - frameExpected);
					DatagramPacket dp2 = new DatagramPacket(sendFrame, sendFrame.length, address, senderPort);
					ds.send(dp2);
					System.out.println("Sending ack, ack is: " + (1 - frameExpected));
					System.out.println();	
					
					if ((byte) frameExpected == serial) {
						frameExpected = (frameExpected + 1) % 2;
						os.write(data, 0, index - dataStartPos);
					}
				} else {
					Date t = new Date();
					SimpleDateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
					System.out.println("Curremt time: " + df.format(t));
					System.out.println("Data of the frame is wrong, doesn't send ack.");
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
		System.out.println("Be ready to receive file...");
		UDPReceiver operation = new UDPReceiver();
		operation.Receive();
	}

}