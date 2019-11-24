CC = mpicc
CFLAGS = -O3 -Wall
LIBS = -lm
RES = -DPRINT_RESULTS
CONV = -DTEST_CONV

.PHONY: all clean

all: jacobi seidelsor redblacksor

jacobi: Jacobi_serial.c utils.c
	$(CC) $(CFLAGS) $(RES) $(CONV) -o jacobi Jacobi_serial.c utils.c $(LIBS)

seidelsor: GaussSeidelSOR_serial.c utils.c
	$(CC) $(CFLAGS) $(RES) $(CONV) -o seidelsor GaussSeidelSOR_serial.c utils.c $(LIBS)

redblacksor: RedBlackSOR_serial.c utils.c
	$(CC) $(CFLAGS) $(RES) $(CONV) -o redblacksor RedBlackSOR_serial.c utils.c $(LIBS)

clean:
	$(RM) jacobi seidelsor redblacksor
