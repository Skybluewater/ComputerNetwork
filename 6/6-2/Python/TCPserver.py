# UDP通信
import socket
import sys

#接收参数 本地开放端口
if __name__ == '__main__':

    if len(sys.argv) == 2:

        port = int(sys.argv[1])

        server = socket.socket(type=socket.SOCK_STREAM)
        server.bind(('127.0.0.1',port))

        print('Waiting for Connection')
        server.listen(10)
        print('Waiting for Message')
        con,addr = server.accept()
        data = con.recv(1024)
        print('Message recived')
        str = data.decode('utf-8')
        print('Message:'+str)

        print('Sending message')
        str = str.upper()
        con.send(str.encode('utf-8'))
        print('Message sent')

        #con.close()
        server.close()
