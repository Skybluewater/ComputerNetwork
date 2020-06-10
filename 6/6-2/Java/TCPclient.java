import java.net.ServerSocket;
import java.net.Socket;
import java.io.*;

public class TCPclient {
    public static void main (String[] args) throws Exception {//接收参数 目标IP、目标端口、待处理字符串
        byte[] buf = new byte[1024];
        if(args.length != 3)
            return;
        Socket sk = new Socket(args[0],Integer.parseInt(args[1]));

        String msg = args[2];
        System.out.println("Sending message");
        OutputStream os = sk.getOutputStream();
        os.write(msg.getBytes());
        System.out.println("Message sent");

        System.out.println("Waiting for message");
        InputStream is = sk.getInputStream();
        int len = is.read(buf);
        System.out.println("Message received");
        msg = new String(buf,0,len);
        System.out.println("toUpper:"+msg);
        os.close();
        is.close();
        sk.close();
    }
}
