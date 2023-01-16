import socket                   # Import socket module
import json
from random import randint
import time
import struct

def send_data_stream(conn,n):
	# print("Sending Stream")
	x = list(range(0,n))
	y = []

	for _ in range(n):
		y.append(randint(0, 30)*5.9878)

	# data = json.dumps([{"X":x,"Y":y}])

	send_data = struct.pack('2d', 1, 2)
	print(send_data)
	s.send(send_data)
	# print("Sent")

s = socket.socket()             # Create a socket object
host = socket.gethostname()     # Get local machine name
port = 8000                   # Reserve a port for your service.

s.connect((host, port))

while True:
	send_data_stream(s,1)
	time.sleep(1)
# s.send("Hello server!".encode())

# data = s.recv(1024)
# print(data)

s.close()
print('Connection Closed')