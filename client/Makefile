CC=gcc -Wall

clean:
	rm -f *.o *.gch core.* ipcqc
	rm -f *.o *.gch core.* ipcqs

ipcqc: client.o
	$(CC) -o ipcqc client.o $(CLIB)
client.o:
	$(CC) -c client.c

ipcqs: server.o handler.o
	$(CC) -o ipcqs server.o handler.o $(CLIB)
server.o:
	$(CC) -c server.c
handler.o:
	$(CC) -c handler.c