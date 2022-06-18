#!/usr/bin/env python

import socket
import struct

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)

s.bind(('192.168.1.200', 12345))

while True:
    message, sender = s.recvfrom(4096)
    if len(message) == 16:
        color = struct.unpack("4f", message)
        print(color, sender, len(message))
    if len(message) == 4:
        command = struct.unpack("f",message)
        print(command, sender, len(message))
    if message == b'quit':
        break
