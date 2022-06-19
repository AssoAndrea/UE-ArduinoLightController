
import serial
import KeyConfig as kc
import struct
import socket

IP = '192.168.1.200'
PORT = 12345

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
s.setblocking(False)



def read_data():
    d: bytes = ser.readline()

    if len(d) > 0:
        res = d.decode().replace('\r\n', '')
        return res

def send_color_data(color:tuple):
    packet = struct.pack("4f",*color,1)
    s.sendto(packet, (IP,PORT))
    print("send: %s" % packet)

def send_command_data(command:float):
    packet = struct.pack("f",command)
    s.sendto(packet, (IP,PORT))
    print("send: %s" % packet)

if __name__ == '__main__':

    ser = serial.Serial('COM5', 9600)
    while True:
        data: bytes = ser.read_until(expected=b"\r\n")
        data = data.replace(b"\r\n",b'')
        int_data = int.from_bytes(data,byteorder="big")
        print(int_data)

        if len(data) > 0:
            if kc.colors.get(int_data):
                print("ok")
                color_to_send = kc.colors[int_data]
                send_color_data(color_to_send)
            elif kc.commands.get(int_data):
                print("command")
                command_to_send = kc.commands[int_data]
                send_command_data(command_to_send)




