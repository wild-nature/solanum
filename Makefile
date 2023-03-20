CC      = gcc
CFLAGS  = -g -Wall
RM      = rm -f

default: all

all: main

main: main.c lexer.c parser.c
	$(CC) $(CFLAGS) -o main main.c lexer.c parser.c

clean:
	$(RM) main
