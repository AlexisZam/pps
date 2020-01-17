CC = gcc
CFLAGS = -Wall -Wextra -Wno-unused-parameter -pthread

.PHONY: all clean

all: nosync_lock pthread_lock tas_lock ttas_lock array_lock clh_lock test_pthread_lock test_tas_lock test_ttas_lock test_array_lock test_clh_lock

nosync_lock: main.c nosync_lock.c ../common/aff.c
	$(CC) $(CFLAGS) $^ -o $@

pthread_lock: main.c pthread_lock.c ../common/aff.c
	$(CC) $(CFLAGS) $^ -o $@

tas_lock: main.c tas_lock.c ../common/aff.c
	$(CC) $(CFLAGS) $^ -o $@

ttas_lock: main.c ttas_lock.c ../common/aff.c
	$(CC) $(CFLAGS) $^ -o $@

array_lock: main.c array_lock.c ../common/aff.c
	$(CC) $(CFLAGS) $^ -o $@

clh_lock: main.c clh_lock.c ../common/aff.c
	$(CC) $(CFLAGS) $^ -o $@

test_pthread_lock: test.c pthread_lock.c ../common/aff.c
	$(CC) $(CFLAGS) $^ -o $@

test_tas_lock: test.c tas_lock.c ../common/aff.c
	$(CC) $(CFLAGS) $^ -o $@

test_ttas_lock: test.c ttas_lock.c ../common/aff.c
	$(CC) $(CFLAGS) $^ -o $@

test_array_lock: test.c array_lock.c ../common/aff.c
	$(CC) $(CFLAGS) $^ -o $@

test_clh_lock: test.c clh_lock.c ../common/aff.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) nosync_lock pthread_lock tas_lock ttas_lock array_lock clh_lock test_pthread_lock test_tas_lock test_ttas_lock test_array_lock test_clh_lock
