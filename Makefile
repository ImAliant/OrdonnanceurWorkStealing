CC := gcc
CFLAGS := -Wall -O2 -pthread
DEBUG := -g -fsanitize=address
SRC := src
QUICKSORT := $(SRC)/quicksort.c
STACK := $(SRC)/stack.c
SCHED := $(SRC)/sched.c


all: scheduler

scheduler_debug : $(QUICKSORT) $(STACK) $(SCHED)
	$(CC) $(CFLAGS) $(DEBUG) -o scheduler $^
scheduler: $(QUICKSORT) $(STACK) $(SCHED)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f scheduler src/*.o
