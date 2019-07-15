import zmq
import hashlib
import os
import sys
import json
import time


def main():# 1arg=nodoID, 2ipnodo, 3puerto nodo, 4arg=idsucesor 5arg=puerto sucesor
	context = zmq.Context()
	sock = context.socket(zmq.DEALER)
	sock.identity = b'a1'
	sock.connect("tcp://localhost:4443")
	poller = zmq.Poller()
	poller.register(sys.stdin, zmq.POLLIN)
	poller.register(sock, zmq.POLLIN)


	mensaje = {'op':'reg'}
	msg = json.dumps(mensaje)
	sock.send_multipart([b'a1', msg.encode("utf8")])
	while(True) :
		socks = dict(poller.poll())		#router--------------------------------
		if sock in socks: #envia peticiones
			#----------enganchar server------------------------------------
			print('hay un socket')
			sender, msg = sock.recv_multipart()
			#mensaje_json = json.loads(msg)
			print(msg)
		elif sys.stdin.fileno() in socks:
			print("?")
			command = input()


if __name__ == '__main__':
    main()