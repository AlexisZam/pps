CC = gcc
CFLAGS = -O0 -Wall -Wextra -pthread

.PHONY = all clean

all: accounts

accounts: accounts.c ../common/aff.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) accounts
