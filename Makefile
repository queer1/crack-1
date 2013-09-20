CC=gcc
CFLAGS=-O3 -lcrypt -lpthread

crack: crack.c
	gcc -o $@ $^ $(CFLAGS)