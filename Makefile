CC := gcc
CFLAGS := -Wall -O2 -pthread
DEBUG := -g -fsanitize=address
SRC := src
QUICKSORT := $(SRC)/quicksort.c
TEST := $(SRC)/main.c
DEQUE := $(SRC)/deque.c
SCHED := $(SRC)/sched_work_stealing.c


all: scheduler
test: scheduler_test

scheduler_debug : $(QUICKSORT) $(SCHED)
	$(CC) $(CFLAGS) $(DEBUG) -o scheduler $^
scheduler: $(QUICKSORT) $(DEQUE) $(SCHED)
	$(CC) $(CFLAGS) -o $@ $^
scheduler_test_debug: $(TEST) $(DEQUE) $(SCHED)
	$(CC) $(CFLAGS) $(DEBUG) -o scheduler_test $^
scheduler_test: $(TEST) $(DEQUE) $(SCHED)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f scheduler scheduler_test src/*.o
