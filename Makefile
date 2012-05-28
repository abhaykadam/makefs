CC = gcc
CFLAGS = -Wall -g

all: createnformat

createnformat: createnformat.o
	$(CC) $(CFLAGS) -o createnformat createnformat.o

createnformat.o: createnformat.c fs.h
	$(CC) $(CFLAGS) -c createnformat.c

clean: 
	rm -rf *o createnformat

