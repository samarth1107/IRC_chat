all:
	gcc -o client client.c -pthread
	gcc -o server server.c -pthread
	clear
	./server -pthread
