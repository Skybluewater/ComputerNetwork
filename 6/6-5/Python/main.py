import socket
import argparse
import struct
import os, time

default_timer = time.time


def checksum(header: bytes) -> int:
    cs = []
    for i in range(0, len(header), 2):
        cs.append(int.from_bytes(header[i:i + 2], "big"))
    cs = sum([i for i in cs])
    while cs > 0xffff:
        cs = (cs & 0xffff) + (cs >> 16)
    cs = cs ^ 0xffff
    return cs


def tcp_checksum(ip_header: bytes, tcp_header: bytes) -> int:
    qseudo = ip_header[12:16]
    qseudo += ip_header[16:20]
    qseudo += b"\x00"
    qseudo += b"\x06"
    qseudo += b"\x00\x14"
    tcpseg = qseudo + tcp_header
    if len(tcpseg) % 2 != 0:
        tcpseg += b'\x00'
    return checksum(tcpseg)


parser = argparse.ArgumentParser()
parser.add_argument("ip", help="ip address of destination.")
parser.add_argument("port", help="port of dest ip address.")
args = parser.parse_args()

s = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_TCP)
# s.setsockopt(socket.IPPROTO_IP, socket.IP_HDRINCL, 1)

srcip = socket.gethostbyname("192.168.3.9")
print(srcip)
srcip = socket.inet_aton(srcip)
destip = socket.inet_aton(args.ip)

ip_header = b"\x45\x00\x00\x40"
ip_header += b"\x00\x00\x40\x00"
ip_header += b"\x40\x06\x00\x00"  # including checksum
ip_header += srcip
ip_header += destip

ip_header = ip_header[:10] + checksum(ip_header).to_bytes(2, 'big') + ip_header[12:]

print(ip_header)

tcp_header = b"\x30\x39"  # Source Port | Destination Port
tcp_header += int(args.port).to_bytes(2, byteorder='big')
tcp_header += b'\x00\x00\x00\x01'  # Sequence Number
tcp_header += b'\x00\x00\x00\x00'  # Acknowledgement Number
tcp_header += b'\x50\x02\x71\x10'  # Data Offset, Reserved, Flags | Window Size
tcp_header += b'\x00\x00\x00\x00'  # Checksum | Urgent Pointer

tcp_header = tcp_header[:16] + tcp_checksum(ip_header, tcp_header).to_bytes(2, 'big') + tcp_header[18:]

print(ip_header.hex())
print(tcp_header.hex())

ID = os.getpid() & 0xFFFF

packet = ip_header + tcp_header

s.sendto(packet, (args.ip, int(args.port)))

dest_addr = socket.gethostbyname("www.baidu.com")

my_checksum = 0

# Make a dummy heder with a 0 checksum.
header = struct.pack("bbHHh", 8, 0, my_checksum, ID, 1)
bytesInDouble = struct.calcsize("d")
data = (192 - bytesInDouble) * b"Q"
data = struct.pack("d", default_timer()) + data

# Calculate the checksum on the data and the dummy header.
my_checksum = checksum(header + data)

my_socket = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_ICMP)

# Now that we have the right checksum, we put that in. It's just easier
# to make up a new header than to stuff it into the dummy.

header = struct.pack(
    "bbHHh", 8, 0, socket.htons(my_checksum), ID, 1
)
packet = header + data
print("The sending packet header is:")
print(header.hex())
my_socket.sendto(packet, (dest_addr, 1))  # Don't know about the 1

timeReceived = default_timer()

recPacket, addr = my_socket.recvfrom(1024)

icmpHeader = recPacket[20:28]
type, code, checksum, packetID, sequence = struct.unpack(
    "bbHHh", icmpHeader
)

print("The received header is:")
print(recPacket[:28])

if type != 8 and packetID == ID:
    bytesInDouble = struct.calcsize("d")
    timeSent = struct.unpack("d", recPacket[28:28 + bytesInDouble])[0]
    print("get ping in:"+str((timeReceived - timeSent)*1e5)+"ms")
