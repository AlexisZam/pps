CC = gcc
CFLAGS = -fopenmp -O3 -Wall
HDEPS += %.h
OBJS = util.o

.PHONY: all clean

all: fw fw_sr fw_tiled

fw: $(OBJS) fw.c
	$(CC) $(CFLAGS) -o fw $(OBJS) fw.c
fw_sr: fw_sr.c 
	$(CC) $(CFLAGS) -o fw_sr $(OBJS) fw_sr.c
fw_tiled: fw_tiled.c 
	$(CC) $(CFLAGS) -o fw_tiled $(OBJS) fw_tiled.c

%.o: %.c $(HDEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) *.o fw fw_sr fw_tiled

