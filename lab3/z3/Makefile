CC = gcc
CFLAGS = -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -pthread
LL_FILE = ll_serial.c

.PHONY = all clean

all: linked_list

linked_list: main.c $(LL_FILE) ../common/aff.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) linked_list
