CC=gcc
CFLAGS=-O3 -lcrypt -lpthread -lm -g

crack: crack.c
	gcc -o $@ $^ $(CFLAGS)