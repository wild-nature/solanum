CC      = gcc
CFLAGS  = -g
RM      = rm -f

default: all

all: main

main: main.c
	$(CC) $(CFLAGS) -o main main.c

clean:
	$(RM) main
