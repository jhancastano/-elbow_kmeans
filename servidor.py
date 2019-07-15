import zmq
import sys
import json
import os


def workers(identidad,lista):
	ident =identidad.decode('utf8')
	if ident in lista:
		print('registrado')
	else:
		lista.append(ident)

def main():
	listaworkers = []
	numeroWorkers = 1
	context = zmq.Context()
	socket = context.socket(zmq.ROUTER)
	socket.bind("tcp://*:4443")
	print("Started server")
	poller = zmq.Poller()
	poller.register(sys.stdin, zmq.POLLIN)
	poller.register(socket, zmq.POLLIN)



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
				else:
					print(msg)
					#socket.send_multipart([sender, msg])
		
		elif sys.stdin.fileno() in socks:
				os.system('clear')
				print("?")
				command = input()
				op, msg = command.split(' ', 1)
				
		

if __name__ == '__main__':
	main()
