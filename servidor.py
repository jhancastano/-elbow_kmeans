import zmq
import sys
import json
import os

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt




def workers(identidad,lista):
	ident =identidad.decode('utf8')
	if ident in lista:
		print('registrado')
	else:
		lista.append(ident)




def main():
	listaworkers = []
	context = zmq.Context()
	socket = context.socket(zmq.ROUTER)
	socket.bind("tcp://*:4443")
	print("Started server")
	poller = zmq.Poller()
	poller.register(sys.stdin, zmq.POLLIN)
	poller.register(socket, zmq.POLLIN)

	jsonworkers = {'op':'sendworker','dataset':'','inicial':1,'final':149,'nvariables':0,'distancias':[]}


	while True:
		print("k nvariables")
		#sender, destino , msg = socket.recv_multipart()
		#socket.send_multipart([destino, sender, msg])
		#print(msg)
		socks = dict(poller.poll())
		if socket in socks:

				sender, msg = socket.recv_multipart()
				print(msg)
				mensaje_json = json.loads(msg)
				operacion = mensaje_json['op']
				if operacion=='reg':
					print(sender)
					workers(sender,listaworkers)
					print(listaworkers)
					socket.send_multipart([sender,msg])
				elif operacion=='dep':
					print("depurando")
					mensaje = {'op':'dep'}
					msg = json.dumps(mensaje)
					socket.send_multipart([sender,msg.encode("utf8")])
				elif  operacion == 'finishwork':
					fig = plt.figure(figsize=(5, 5))
					yi = []
					for k in range (mensaje_json['inicial'],mensaje_json['final']+1): 
						yi.append(k) 
					print(yi)
					plt.scatter(mensaje_json['distancias'], yi, color='k')
					plt.title(u'Los kodo')
					plt.show()


					#print(msg)
				else:
					print(msg)
					#socket.send_multipart([sender, msg])
		
		elif sys.stdin.fileno() in socks:
				os.system('clear')
				print("op dataset ndatos nvariables?")
				command = input()
				op, dataset, ndatos, nvariables = command.split(' ', 3)
				if(op=='k'):
					msg = json.dumps(jsonworkers)
					print(listaworkers[0])
					for x in range(len(listaworkers)):
						y = len(listaworkers)
						i = 1
						if (x == 0):
							jsonworkers = {'op':'sendworker','dataset':dataset,'inicial':i,'final':int(i*int(ndatos)/y),'nvariables':int(nvariables),'distancias':[]}
						else:
							jsonworkers = {'op':'sendworker','dataset':dataset,'inicial':int(i*int(ndatos)/y),'final':int((x+1)*int(ndatos)/y),'nvariables':int(nvariables),'distancias':[]}
						msg = json.dumps(jsonworkers)
						print(x)
						socket.send_multipart([listaworkers[x].encode("utf8"),msg.encode("utf8")])
		

if __name__ == '__main__':
	main()
