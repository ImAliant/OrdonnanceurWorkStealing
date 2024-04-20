CC := gcc
CFLAGS := -Wall -O2 -pthread
DEBUG := -g -fsanitize=address
SRC := src
MAIN := $(SRC)/main
TEST := $(SRC)/test

QUICKSORT := $(MAIN)/quicksort.c
UTILS := $(MAIN)/utils.c

# LIFO
LIFO := $(MAIN)/lifo
STACK := $(LIFO)/stack.c
STACK_TEST := $(TEST)/stack_test.c
SCHED_LIFO := $(LIFO)/sched_lifo.c
TEST_LIFO := $(TEST)/test_lifo.c

# WORK STEALING
WS := $(MAIN)/work_stealing
DEQUE := $(WS)/deque.c
DEQUE_TEST := $(TEST)/deque_test.c
SCHED_WS := $(WS)/sched_work_stealing.c
TEST_WS := $(TEST)/test_ws.c

all: scheduler_lifo scheduler_work_stealing
test_all: scheduler_test_lifo scheduler_test_ws
test_lifo: scheduler_test_lifo
test_ws: scheduler_test_ws

scheduler_lifo: $(QUICKSORT) $(STACK) $(SCHED_LIFO) $(UTILS)
	$(CC) $(CFLAGS) -o $@ $^
scheduler_work_stealing: $(QUICKSORT) $(DEQUE) $(SCHED_WS) $(UTILS)
	$(CC) $(CFLAGS) -o $@ $^
scheduler_test_lifo: $(TEST_LIFO) $(STACK_TEST) $(UTILS)
	$(CC) $(CFLAGS) -o $@ $^
scheduler_test_ws: $(TEST_WS) $(DEQUE_TEST) $(UTILS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f scheduler_* src/*.o