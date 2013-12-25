all: textServer.c textClient.c
	gcc textServer.c -o textServer
	gcc textClient.c -o textClient
