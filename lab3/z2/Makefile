CC = gcc
CFLAGS = -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -pthread
LOCK_FILE ?= nosync_lock.c

.PHONY = all clean

all: linked_list test

linked_list: main.c $(LOCK_FILE) ../common/aff.c
	$(CC) $(CFLAGS) $^ -o $@

test: test.c $(LOCK_FILE) ../common/aff.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) linked_list test
