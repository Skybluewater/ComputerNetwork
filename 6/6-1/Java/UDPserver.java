import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketAddress;

public class UDPserver {
    public static void main (String[] args) throws Exception {//接收参数 本地开放端口
        byte[] buf = new byte[1024];
        if(args.length != 1)
            return;
        DatagramSocket ds = new DatagramSocket(Integer.parseInt(args[0]));
        DatagramPacket dp = new DatagramPacket(buf,1024);
        System.out.println("Waiting for message");
        ds.receive(dp);
        System.out.println("Message received");
        String msg = new String(dp.getData());
        System.out.println("Message:"+msg);
        msg = msg.toUpperCase();

        dp = new DatagramPacket(msg.getBytes(),msg.length(),
                                dp.getSocketAddress());

        System.out.println("Sending message");
        ds.send(dp);
        System.out.println("Message sent");
        ds.close();
    }
}
