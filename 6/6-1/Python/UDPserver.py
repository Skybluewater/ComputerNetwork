# UDP通信
import socket
import sys

#接收参数 本地开放端口
if __name__ == '__main__':

    if len(sys.argv) == 2:

        port = int(sys.argv[1])

        server = socket.socket(type=socket.SOCK_DGRAM)
        server.bind(('127.0.0.1',port))

        print('Waiting for Message')
        data,address = server.recvfrom(1024)
        print('Message recived')
        str = data.decode('utf-8')
        print('Message:'+str)

        print('Sending message')
        str = str.upper()
        server.sendto(str.encode('utf-8'),address)
        print('Message sent')

        server.close()
