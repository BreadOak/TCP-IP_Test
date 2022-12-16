import sys
import os
import time
import socket
import struct

host = '192.168.0.2'
port = 8000

UDP_server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
UDP_server_socket.bind((host,port))

data_size = 8*3 # byte
data =  UDP_server_socket.recv(data_size)
data = struct.unpack('3d', data)

print("data1:",data[0])
print("data2:",data[1])
print("data3:",data[2])

UDP_server_socket.close()