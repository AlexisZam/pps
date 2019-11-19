CC = gcc
CFLAGS = -fopenmp -O3 -Wall

.PHONY: all clean

all: fw fw_sr fw_tiled

fw: fw.c utils.c
	$(CC) $(CFLAGS) -o fw fw.c util.c

fw_sr: fw_sr.c utils.c
	$(CC) $(CFLAGS) -o fw_sr fw_sr.c utils.c

fw_tiled: fw_tiled.c utils.c
	$(CC) $(CFLAGS) -o fw_tiled fw_tiled.c utils.c

clean:
	$(RM) *.o fw fw_sr fw_tiled