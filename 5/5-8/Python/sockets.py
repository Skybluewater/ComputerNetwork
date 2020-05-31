import socket
from functools import partial

__all__ = ['icmp_socket', 'udp_socket']

icmp = socket.getprotobyname('icmp')
icmp_socket = partial(socket.socket, socket.AF_INET, socket.SOCK_RAW, icmp)

udp = socket.getprotobyname('udp')
udp_socket = partial(socket.socket, socket.AF_INET, socket.SOCK_DGRAM, udp)