#! /usr/bin/python3

# PYTHON CLIENT

import socket

port = 3490
ip = 'localhost'

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # socket object
s.connect((ip, port))
request = "GET /home/hallison/webserv/Makefile HTTP/1.0\r\n\r\n"

s.send(request.encode())
data = s.recv(1024).decode()
print(data)
# print(s.recv(1024))

s.close()
