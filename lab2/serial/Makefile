CC = gcc
CFLAGS = -O3 -Wall -Wextra
RES = -D PRINT_RESULTS
CONV = -D TEST_CONV
LIBS = -lm

.PHONY: all clean

all: jacobi gaussseidelsor redblacksor

jacobi: Jacobi_serial.c utils.c
	$(CC) $(CFLAGS) $(RES) $(CONV) -o jacobi Jacobi_serial.c utils.c $(LIBS)

gaussseidelsor: GaussSeidelSOR_serial.c utils.c
	$(CC) $(CFLAGS) $(RES) $(CONV) -o gaussseidelsor GaussSeidelSOR_serial.c utils.c $(LIBS)

redblacksor: RedBlackSOR_serial.c utils.c
	$(CC) $(CFLAGS) $(RES) $(CONV) -o redblacksor RedBlackSOR_serial.c utils.c $(LIBS)

clean:
	$(RM) jacobi gaussseidelsor redblacksor
