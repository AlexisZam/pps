CC = gcc
CFLAGS = -Wall -Wextra -pthread

.PHONY: all clean

all: ll_serial ll_fgl ll_opt ll_lazy ll_nb test_ll_fgl test_ll_opt test_ll_lazy test_ll_nb

ll_serial: main.c ll_serial.c ../common/aff.c
	$(CC) $(CFLAGS) $^ -o $@

ll_fgl: main.c ll_fgl.c ../common/aff.c
	$(CC) $(CFLAGS) $^ -o $@

ll_opt: main.c ll_opt.c ../common/aff.c
	$(CC) $(CFLAGS) $^ -o $@

ll_lazy: main.c ll_lazy.c ../common/aff.c
	$(CC) $(CFLAGS) $^ -o $@

ll_nb: main.c ll_nb.c ../common/aff.c
	$(CC) $(CFLAGS) $^ -o $@

test_ll_fgl: test.c ll_fgl.c ../common/aff.c
	$(CC) $(CFLAGS) $^ -o $@

test_ll_opt: test.c ll_opt.c ../common/aff.c
	$(CC) $(CFLAGS) $^ -o $@

test_ll_lazy: test.c ll_lazy.c ../common/aff.c
	$(CC) $(CFLAGS) $^ -o $@

test_ll_nb: test.c ll_nb.c ../common/aff.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) ll_serial ll_fgl ll_opt ll_lazy ll_nb test_ll_fgl test_ll_opt test_ll_lazy test_ll_nb
