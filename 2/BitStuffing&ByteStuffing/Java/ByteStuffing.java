public class ByteStuffing {
	private String flagString = "7E";
	private String insertString = "1B";
	
	public String Send() {
		String dataStr = "347D7E807E40AA7D";
		String sendFrame = flagString + dataStr + flagString;
		int flagLen = flagString.length();
		int sendFrameLen = sendFrame.length();
		System.out.println("帧起始标志：" + sendFrame.substring(0, flagLen));
		System.out.println("帧数据信息：" + sendFrame.substring(flagLen, sendFrameLen - flagLen));
		System.out.println("帧结束标志：" + sendFrame.substring(sendFrameLen - flagLen));
		
		int insertLen = insertString.length();
		for (int i = flagLen; i < (sendFrame.length() - flagLen); i++) {
			if (sendFrame.substring(i, i + flagLen).equals(flagString)) {
				sendFrame = sendFrame.substring(0, i) + insertString + sendFrame.substring(i);
				i = i + insertLen + flagLen;
			}
		}
		
		System.out.println("字节填充后的发送帧：" + sendFrame);
		System.out.println();
		return sendFrame;
	}
	
	public void Receive(String receiveFrame) {
		int flagLen = flagString.length();
		int insertLen = insertString.length();
		for (int i = flagLen; i < (receiveFrame.length() - flagLen); i++) {
			if (receiveFrame.substring(i, i + flagLen).equals(flagString)) {
				receiveFrame = receiveFrame.substring(0, i - insertLen) + receiveFrame.substring(i);
				i = i + flagLen;
			}
		}	
		System.out.println("字节删除后的接收帧：" + receiveFrame);
	}
	
	public static void main(String[] args) {
		ByteStuffing operation = new ByteStuffing();
		String frameStr = operation.Send();
		operation.Receive(frameStr);
	}
	
}
