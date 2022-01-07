#! /usr/bin/python

from socket import *

HOST = 'localhost'
PORT = 8888
BUFSIZE = 1024
ADDR = (HOST, PORT)

tcpCliSocket = socket(AF_INET, SOCK_STREAM)
tcpCliSocket.connect(ADDR)

while True:
		data = raw_input('<')
		if not data:
				break
		tcpCliSocket.send(data)
		data = tcpCliSocket.recv(BUFSIZE)
		if not data:
				break
		print data
		
tcpCliSocket.close()

