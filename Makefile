CC=gcc
CFLAGS =-Wall -g

clean:
	rm -f *.o *.gch core.* ipcqc
	rm -f *.o *.gch core.* ipcqs
	ipcrm --all=msg
	ipcrm --all=sem
	
ipcqc: client.o io.o semaphore.o
	$(CC) $(CFLAGS) -pthread -o ipcqc client.o io.o semaphore.o $(CLIB)

ipcqs: server.o handler.o io.o semaphore.o
	$(CC) $(CFLAGS) -o ipcqs server.o handler.o io.o semaphore.o $(CLIB)

client.o: client.c
	$(CC) $(CFLAGS) -c client.c -pthread
handler.o: handler.c
	$(CC) $(CFLAGS) -c handler.c
io.o: io.c
	$(CC) $(CFLAGS) -c io.c 
semaphore.o: semaphore.c
	$(CC) $(CFLAGS) -c semaphore.c
server.o: server.c
	$(CC) $(CFLAGS) -c server.c
