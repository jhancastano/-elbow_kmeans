import zmq
import sys
import json
import os

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.lines as lines


def distancias(p1,p2,v):
	listaDist = []
	x0 = p1[0]
	y0 = p1[1]
	x1 = p2[0]
	y1 = p2[1]
	print(len(v))
	for i in range(0,len(v)):
		x = v[i][0]
		y = v[i][1]
		d =abs(((y1-y0)*x-(x1-x0)*y+x1*y0-y1*x0))
		n = pow((pow((y1-y0),2)+pow((x1-x0),2)),0.5)	
		distancia = d/n
		listaDist.append(distancia)
	return listaDist

def dsinprimervalor(p1,p2,v):
	listaDist = []
	x0 = p1[0]
	y0 = p1[1]
	x1 = p2[0]
	y1 = p2[1]
	print(len(v))
	for i in range(1,len(v)):
		x = v[i][0]
		y = v[i][1]
		d =abs(((y1-y0)*x-(x1-x0)*y+x1*y0-y1*x0))
		n = pow((pow((y1-y0),2)+pow((x1-x0),2)),0.5)	
		distancia = d/n
		listaDist.append(distancia)
	return listaDist



def workers(identidad,lista):
	ident =identidad.decode('utf8')
	if ident in lista:
		print('registrado')
	else:
		lista.append(ident)

def ordenar(listax,listay):
	diccionario = {}
	
	for i in range(len(listax)):
		diccionario.update({listax[i]:[listax[i],listay[i]]})
	return diccionario

def listpoint(diccionario):
	listaordenada = []
	for x in range(len(diccionario)):
		listaordenada.append(diccionario[x+1])
	return listaordenada

def listXorY(listpoint,entero):
	listaux = []
	for i in range(len(listpoint)):
		listaux.append(listpoint[i][entero])
	return listaux

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


	puntosDistancias = []
	puntosPendientes = []
	nInstancias = 1
	RecolectaPuntos = []
	diccionarioPuntos = {}

	while True:
		print("k nvariables")
		#sender, destino , msg = socket.recv_multipart()
		#socket.send_multipart([destino, sender, msg])
		#print(msg)
		socks = dict(poller.poll())
		if socket in socks:

				sender, msg = socket.recv_multipart()
				#print(msg)
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
					fig = plt.figure(figsize=(150, 20))
					yi = []
					if(len(RecolectaPuntos) < nInstancias):
						for k in range (mensaje_json['inicial'],mensaje_json['final']+1): 
							yi.append(k) 
						for point in mensaje_json['distancias']:
							RecolectaPuntos.append(point)
						diccionarioPuntos.update(ordenar(yi,RecolectaPuntos))
					if(mensaje_json['final']==nInstancias):
						l1 = listpoint(diccionarioPuntos)

						plt.scatter(listXorY(l1,0),listXorY(l1,1),color='k')
						#linea = lines.Line2D(l1[1],l1[len(l1)-1],transform=fig.transFigure, figure=fig)
						#fig.lines.extend([linea])
						plt.plot([l1[0][0],l1[len(l1)-1][0]],[l1[0][1],l1[len(l1)-1][1]],color='b')
						#print(l1[1],l1[len(l1)-1])
						#puntosDistancias = dsinprimervalor(l1[1],l1[len(l1)-1],l1)
						puntosDistancias = distancias(l1[0],l1[len(l1)-1],l1)

						k = puntosDistancias.index(max(puntosDistancias))

						plt.scatter(l1[k][0],l1[k][1],color='r')
						#print(puntosDistancias)


						print("el numero k aproximado es :" + str(k))

						plt.title(u'Los kodo')

						plt.show()
	
								
					#for k in range (mensaje_json['inicial'],mensaje_json['final']+1): 
						#yi.append(k) 
					#print(yi)
					

					#print(msg)
				else:
					pass
					#print(msg)
					#socket.send_multipart([sender, msg])
		
		elif sys.stdin.fileno() in socks:
				os.system('clear')
				print("op dataset ndatos nvariables?")
				command = input()
				op, dataset, ndatos, nvariables = command.split(' ', 3)
				if(op=='k'):
					msg = json.dumps(jsonworkers)
					nInstancias = int(ndatos)
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
