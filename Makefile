# Makefile for the wildlife dictionary project (COMP20003 Assignment 2).
#
# Type `make dict1` to build the Stage 1 (linked list) program, or
# `make dict2` (or just `make`) to build the Stage 2 (Patricia tree
# spellcheck) program.

CC = gcc
CFLAGS = -Wall -std=c99 -pedantic -g

DICT1_OBJS = dict1.o record.o dictionary.o search.o csv.o compare.o bit.o \
             util.o
DICT2_OBJS = dict2.o record.o patricia.o patriciaSearch.o editdistance.o \
             search.o csv.o compare.o bit.o util.o

all: dict1 dict2

dict1: $(DICT1_OBJS)
	$(CC) $(CFLAGS) -o dict1 $(DICT1_OBJS)

dict2: $(DICT2_OBJS)
	$(CC) $(CFLAGS) -o dict2 $(DICT2_OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f dict1 dict2 *.o

.PHONY: all clean
