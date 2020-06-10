import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketAddress;
import java.net.InetSocketAddress;

public class UDPclient {
    public static void main (String[] args) throws Exception {//接收参数 目标IP、本地开放端口、目标端口、待处理字符串
        byte[] buf = new byte[1024];
        if(args.length != 4)
            return;
        DatagramSocket ds = new DatagramSocket(Integer.parseInt(args[1]));
        InetSocketAddress address = new InetSocketAddress(args[0], Integer.parseInt(args[2]));
        DatagramPacket dp = new DatagramPacket(args[3].getBytes(),args[3].length(),
                                               address);

        System.out.println("Sending message");
        ds.send(dp);
        System.out.println("Message sent");

        System.out.println("Waiting for message");
        ds.receive(dp);
        System.out.println("Message received");
        String msg = new String(dp.getData());
        System.out.println("toUpper:"+msg);
        ds.close();
    }
}
