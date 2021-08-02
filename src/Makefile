CC=gcc
CFLAGS=-Wall -Werror -Wvla -std=gnu11 -fsanitize=address
PFLAGS=-fprofile-arcs -ftest-coverage
DFLAGS=-g
HEADERS=server.h message_protocol.h message_process.h
SRC=server.c message_protocol.c message_process.c

procchat: $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(DFLAGS) $(SRC) -o $@

test:
	$(CC) $(CFLAGS) $(PFLAGS) client1.c clients.c -o client1 -lm
	$(CC) $(CFLAGS) $(PFLAGS) client2.c clients.c -o client2 -lm
	$(CC) $(CFLAGS) $(PFLAGS) client3.c clients.c -o client3 -lm
	bash test.sh

clean:
	rm -f procchat

