CC=gcc -Wall

clean:
	rm -f *.o *.gch core.* ipcqs
ipcqs: server.o handler.o
	$(CC) -o ipcqs server.o handler.o $(CLIB)
server.o:
	$(CC) -c server.c
handler.o:
	$(CC) -c handler.c