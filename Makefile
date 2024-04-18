CC := gcc
CFLAGS := -Wall -O2 -pthread
DEBUG := -g -fsanitize=address
SRC := src
QUICKSORT := $(SRC)/quicksort.c
STACK := $(SRC)/stack.c
STACK_TEST := $(SRC)/stack_test.c
SCHED_LIFO := $(SRC)/sched_lifo.c
UTILS := $(SRC)/utils.c
TEST := $(SRC)/main.c

all: scheduler_lifo
test: scheduler_lifo_test

scheduler_lifo_test: $(TEST) $(STACK_TEST) $(UTILS)
	$(CC) $(CFLAGS) -o $@ $^
scheduler_lifo_debug : $(QUICKSORT) $(STACK) $(SCHED_LIFO)
	$(CC) $(CFLAGS) $(DEBUG) -o scheduler_lifo $^
scheduler_lifo: $(QUICKSORT) $(STACK) $(SCHED_LIFO) $(UTILS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f scheduler_lifo scheduler_lifo_test src/*.o
