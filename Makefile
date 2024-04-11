CC := gcc
CFLAGS := -Wall -O2 -pthread
SRC := src
QUICKSORT := $(SRC)/quicksort.c
STACK := $(SRC)/stack.c
SCHED := $(SRC)/sched.c


all: scheduler

scheduler: $(QUICKSORT) $(STACK) $(SCHED)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f scheduler src/*.o
