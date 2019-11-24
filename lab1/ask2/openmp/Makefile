CC = gcc
CFLAGS = -fopenmp -O3 -Wall

.PHONY: all clean

all: fw fw_sr fw_tiled

fw: fw.c util.c
	$(CC) $(CFLAGS) -o fw fw.c util.c

fw_sr: fw_sr.c util.c
	$(CC) $(CFLAGS) -o fw_sr fw_sr.c util.c

fw_tiled: fw_tiled.c util.c
	$(CC) $(CFLAGS) -o fw_tiled fw_tiled.c util.c

clean:
	$(RM) fw fw_sr fw_tiled
