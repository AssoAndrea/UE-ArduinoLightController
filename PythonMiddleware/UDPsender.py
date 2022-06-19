import serial
import KeyConfig as kc
import struct
import socket

IP = '192.168.1.200'
PORT = 12345

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
s.setblocking(False)

color = [1,1,1]

packet = struct.pack("4f", *color, 1)
s.sendto(packet, (IP, PORT))
print("send: %s" % packet)
