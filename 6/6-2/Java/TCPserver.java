import java.net.ServerSocket;
import java.net.Socket;
import java.io.*;

public class TCPserver {
    public static void main (String[] args) throws Exception {//接收参数 本地开放端口
        byte[] buf = new byte[1024];
        if(args.length != 1)
            return;
        ServerSocket ss = new ServerSocket(Integer.parseInt(args[0]));
        System.out.println("Waiting for connection");
        Socket sk = ss.accept();
        InputStream is = sk.getInputStream();
        int len = is.read(buf);
        System.out.println("Message received");
        String msg = new String(buf,0,len);
        System.out.println("Message:"+msg);
        msg = msg.toUpperCase();

        System.out.println("Sending message");
        OutputStream os = sk.getOutputStream();
        os.write(msg.getBytes());
        System.out.println("Message sent");
        is.close();
        os.close();
        sk.close();
    }
}
