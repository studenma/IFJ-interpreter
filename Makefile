#
# IFJ 2017 project
#	File			Makefile
#	Description		Makefile for gcc
#	Author			Michal Zilvar (xzilva02)

CFLAGS=-std=c99 -Wall -pedantic -g
NAME=ifj17
CC=gcc
OBJ=main.c scanner.c string.c error.c instruction.c symtable.c parser.c 

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

clean:
	rm ifj17
	rm main.o
	rm scanner.o
	rm string.o
	rm error.o
	rm instruction.o
	rm parser.o
	rm symtable.o