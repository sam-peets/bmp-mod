CC=gcc
CFLAGS=-std=c11 -Wall -ggdb

all:
	$(CC) -o bmp_mod bmp_mod.c emalloc.c filters.c -lm
