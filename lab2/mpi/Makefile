CC = mpicc
CFLAGS = -O3 -Wall -Wextra
RES = -D PRINT_RESULTS
CONV = -D TEST_CONV
LIBS = -lm

.PHONY: all clean

all: jacobi gaussseidelsor redblacksor

jacobi: mpi.c utils.c
	$(CC) $(CFLAGS) $(RES) $(CONV) -D JACOBI -o jacobi mpi.c utils.c $(LIBS)

gaussseidelsor: mpi.c utils.c
	$(CC) $(CFLAGS) $(RES) $(CONV) -D GAUSS_SEIDEL_SOR -o gaussseidelsor mpi.c utils.c $(LIBS)

redblacksor: mpi.c utils.c
	$(CC) $(CFLAGS) $(RES) $(CONV) -D RED_BLACK_SOR -o redblacksor mpi.c utils.c $(LIBS)

clean:
	$(RM) jacobi gaussseidelsor redblacksor
