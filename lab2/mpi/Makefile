CC = mpicc
CFLAGS = -O3 -Wall
LIBS = -lm
RES = -DPRINT_RESULTS
CONV = -DTEST_CONV

.PHONY: all clean

all: jacobi gaussseidelsor redblacksor

jacobi: Jacobi.c utils.c
	$(CC) $(CFLAGS) $(RES) $(CONV) -o jacobi Jacobi.c utils.c $(LIBS)

gaussseidelsor: GaussSeidelSOR.c utils.c
	$(CC) $(CFLAGS) $(RES) $(CONV) -o gaussseidelsor GaussSeidelSOR.c utils.c $(LIBS)

redblacksor: RedBlackSOR.c utils.c
	$(CC) $(CFLAGS) $(RES) $(CONV) -o redblacksor RedBlackSOR.c utils.c $(LIBS)

clean:
	$(RM) jacobi gaussseidelsor redblacksor
