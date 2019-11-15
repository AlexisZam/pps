CC = gcc
CFLAGS = -fopenmp -O3 -Wall

.PHONY: all clean

all: Game_Of_Life

Game_Of_Life: Game_Of_Life.c
	$(CC) $(CFLAGS) -o Game_Of_Life Game_Of_Life.c

clean:
	$(RM) Game_Of_Life
