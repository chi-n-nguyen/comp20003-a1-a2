# Makefile for the wildlife dictionary project (COMP20003 Assignment 1).
#
# Type `make dict1` (or just `make`) to build the Stage 1 program.

CC = gcc
CFLAGS = -Wall -std=c99 -pedantic -g

DICT1_OBJS = dict1.o record.o dictionary.o search.o csv.o compare.o bit.o util.o

all: dict1

dict1: $(DICT1_OBJS)
	$(CC) $(CFLAGS) -o dict1 $(DICT1_OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f dict1 *.o

.PHONY: all clean
