from socket import *
import socket
import os
import sys
import struct
import time
import select
import binascii


ICMP_ECHO_REQUEST = 8
ICMP_ECHO_REQUEST_CODE = 0
MAX_HOPS = 30
TIMEOUT = 2.0
TRIES = 1
SEQ = 1
PORT = 0
BUFFER = 1024
DST_UNRCBL = 3
ECHO_REPLY = 0
TIME_ECX = 11


def checksum(str_):
    str_ = bytearray(str_)
    csum = 0
    countTo = (len(str_) // 2) * 2

    for count in range(0, countTo, 2):
        thisVal = str_[count+1] * 256 + str_[count]
        csum = csum + thisVal
        csum = csum & 0xffffffff

    if countTo < len(str_):
        csum = csum + str_[-1]
        csum = csum & 0xffffffff

    csum = (csum >> 16) + (csum & 0xffff)
    csum = csum + (csum >> 16)
    answer = ~csum
    answer = answer & 0xffff
    answer = answer >> 8 | (answer << 8 & 0xff00)
    return answer


def build_packet(seq):
    myChecksum = 0

    myID = 1#os.getpid() & 0xFFFF
    myID = htons(myID)

    header = struct.pack("bbHHh", ICMP_ECHO_REQUEST, ICMP_ECHO_REQUEST_CODE, myChecksum, myID, seq)

    #data = struct.pack("d", time.time())
    data = struct.pack("qqqqqqqq", 0, 0, 0, 0, 0, 0, 0, 0)


    myChecksum = checksum(header + data)
    myChecksum = htons(myChecksum)

    header = struct.pack("bbHHh", ICMP_ECHO_REQUEST, ICMP_ECHO_REQUEST_CODE, myChecksum, myID, seq)

    packet = header + data
    return packet

def get_route(hostname, mode):
    print("Route to %s(%s)..."% (hostname,gethostbyname(hostname)))
    timeLeft = TIMEOUT
    seq = 1

    for ttl in range(1,MAX_HOPS):
        print(ttl)
        for tries in range(TRIES):
            destAddr = socket.gethostbyname(hostname)

            mySocket = socket.socket(
            family=socket.AF_INET,
            type=socket.SOCK_RAW,
            proto=socket.IPPROTO_ICMP
            )
            mySocket.settimeout(TIMEOUT)
            timeout = struct.pack("ll", 2, 2)
            #print(timeout)
            #mySocket.setsockopt(socket.SOL_SOCKET, socket.SO_RCVTIMEO, timeout)
            mySocket.setsockopt(socket.IPPROTO_IP, socket.IP_TTL, struct.pack('I', ttl))


            seq += 1
            d = build_packet(seq)
            mySocket.sendto(d, (destAddr, PORT))
            t = time.time()

                #whatReady = select.select([mySocket], [], [], TIMEOUT)
                #if (not whatReady[0]):
                #    print ("  * Request timed out.")
            try:
                recvPacket, addr = mySocket.recvfrom(BUFFER)

                timeReceived = time.time()



            except socket.timeout as e:
                print(e)
                print ("  * Request timed out.")

            else:
                icmpHeader = recvPacket[20:28]
                request_type, code, checksum, packetID, sequence = struct.unpack("bbHHh", icmpHeader)

                if (request_type == TIME_ECX) or (request_type == 3):
                    #bytes = struct.calcsize("d")
                    #timeSent = struct.unpack("d", recvPacket[28:28 + bytes])[0]
                    if(mode != "-d"):
                        try:
                            print ("  %.0f ms %s [%s]" % ((timeReceived - t)*1000, gethostbyaddr(addr[0])[0], addr[0]))
                        except socket.herror:
                            print ("  %.0f ms %s" % ((timeReceived - t)*1000, addr[0]))
                    else:
                        print ("  %.0f ms %s" % ((timeReceived - t)*1000, addr[0]))
                elif request_type == ECHO_REPLY:
                    #bytes = struct.calcsize("d")
                    #timeSent = struct.unpack("d", recvPacket[28:28 + bytes])[0]
                    if(mode != "-d"):
                        try:
                            print ("  %.0f ms %s [%s]" % ((timeReceived -t)*1000, gethostbyaddr(addr[0])[0], addr[0]))
                        except socket.herror:
                            print ("  %.0f ms %s" % ((timeReceived - t)*1000, addr[0]))
                    else:
                        print ("  %.0f ms %s" % ((timeReceived - t)*1000, addr[0]))
                    if(tries == 2):
                        print("跟踪完成")
                        return
                else:
                    print ("error")
                    break



if __name__ == '__main__':
     get_route("www.baidu.com", "-d")
