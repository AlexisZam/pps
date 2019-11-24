CC = gcc
CFLAGS = -O3 -lm -Wall
RES = -DPRINT_RESULTS
CONV = -DTEST_CONV

.PHONY: all clean

all: jacobi seidelsor redblacksor

jacobi: Jacobi_serial.c utils.c
	$(CC) $(CFLAGS) $(RES) $(CONV) -o jacobi Jacobi_serial.c utils.c

seidelsor: GaussSeidelSOR_serial.c utils.c
	$(CC) $(CFLAGS) $(RES) $(CONV) -o seidelsor GaussSeidelSOR_serial.c utils.c

redblacksor: RedBlackSOR_serial.c utils.c
	$(CC) $(CFLAGS) $(RES) $(CONV) -o redblacksor RedBlackSOR_serial.c utils.c

clean:
	$(RM) jacobi seidelsor redblacksor
