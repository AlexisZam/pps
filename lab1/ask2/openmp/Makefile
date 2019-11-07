.PHONY: all clean

all: fw fw_sr fw_tiled

CC = gcc
CFLAGS = -fopenmp -O3 -Wall

HDEPS += %.h

OBJS = util.o

fw: $(OBJS) fw.c
	$(CC) $(CFLAGS) $(OBJS) fw.c -o fw
fw_sr: fw_sr.c 
	$(CC) $(CFLAGS) $(OBJS) fw_sr.c -o fw_sr
fw_tiled: fw_tiled.c 
	$(CC) $(CFLAGS) $(OBJS) fw_tiled.c -o fw_tiled

%.o: %.c $(HDEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) *.o fw fw_sr fw_tiled

