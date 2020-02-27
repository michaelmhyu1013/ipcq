CC=gcc -Wall -g

clean:
	rm -f *.o *.gch core.* ipcqc
	rm -f *.o *.gch core.* ipcqs
	ipcrm --all=msg
	
ipcqc: client.o io.o
	$(CC) -pthread -o ipcqc client.o io.o $(CLIB)

ipcqs: server.o handler.o io.o
	$(CC) -o ipcqs server.o handler.o io.o $(CLIB)

client.o:
	$(CC) -c client.c -pthread
io.o: 
	$(CC) -c io.c 
server.o:
	$(CC) -c server.c
handler.o:
	$(CC) -c handler.c
