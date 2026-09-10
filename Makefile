# Makefile for the wildlife dictionary project (COMP20003 Assignment 2).
#
# Type `make dict2` (or just `make`) to build the Stage 2 (Patricia tree
# spellcheck) program. Assignment 1's dict1 (linked list) lives on the
# `dict1` branch.

CC = gcc
CFLAGS = -Wall -std=c99 -pedantic -g

DICT2_OBJS = dict2.o record.o patricia.o patriciaSearch.o editdistance.o \
             search.o csv.o compare.o bit.o util.o

all: dict2

dict2: $(DICT2_OBJS)
	$(CC) $(CFLAGS) -o dict2 $(DICT2_OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f dict2 *.o

.PHONY: all clean
