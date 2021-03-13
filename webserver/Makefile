CC = gcc
CFLAGS = -std=c99 -Wall -Wextra

all: main.o parser.o sender.o validator.o
	$(CC) -o webserver $^ $(CFLAGS)

%.o: %.c header.h 
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -rf *.o
	
distclean:
	rm -rf *.o webserver
