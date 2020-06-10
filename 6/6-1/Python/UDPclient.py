# UDP通信
import socket
import sys

#接收参数 目标IP、目标端口、待处理字符串
if __name__ == '__main__':

    if len(sys.argv) == 4:

        server_addr = (sys.argv[1],int(sys.argv[2]))
        msg = sys.argv[3]

        client = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

        print('Sending message')
        client.sendto(msg.encode('utf-8'),server_addr)
        print('Message sent')

        print('Waiting for Message')
        data,address = client.recvfrom(1024)
        print('Message recived')
        print('toUpper:'+data.decode('utf-8'))
