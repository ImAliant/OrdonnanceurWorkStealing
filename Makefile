CC := gcc
CFLAGS := -Wall -O2 -pthread
DEBUG := -g -fsanitize=address
SRC := src
BENCH := benchmark
MAIN := $(SRC)/main
TEST := $(SRC)/test

QUICKSORT := $(MAIN)/quicksort.c
UTILS := $(MAIN)/utils.c
BENCHMARK := $(MAIN)/benchmark.c

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

# FILES
LIFO_WS_COMPARISON := $(BENCH)/graph_runtime_comparaison.py
WS_TASK := $(BENCH)/graph_ws_task.py
WS_OPTIMIZATION := $(BENCH)/graph_ws_optimization.py
SERIAL_PARALLEL_COMPARISON := $(BENCH)/graph_serial_parallel_comparaison.py

all: scheduler_lifo scheduler_work_stealing

test_all: scheduler_test_lifo scheduler_test_ws
test_lifo: scheduler_test_lifo
test_ws: scheduler_test_ws

demo: clean benchmark_comparaison_lifo_ws
	python3 $(LIFO_WS_COMPARISON)
	python3 $(WS_TASK)

demo_optimization: clean benchmark_ws_optimization
	python3 $(WS_OPTIMIZATION)

demo_serial_parallel: clean benchmark_serial_parallel
	python3 $(SERIAL_PARALLEL_COMPARISON)

benchmark_comparaison_lifo_ws: scheduler_lifo scheduler_work_stealing
	@for t in $$(seq 1 8); do \
		./scheduler_lifo -g -t $$t; \
	done
	@for t in $$(seq 1 8); do \
		./scheduler_work_stealing -g -t $$t; \
	done

benchmark_ws_optimization: scheduler_work_stealing
	@for t in $$(seq 1 8); do \
		./scheduler_work_stealing -g -t $$t; \
	done
	@for t in $$(seq 1 8); do \
		./scheduler_work_stealing -g -o -t $$t; \
	done

benchmark_serial_parallel: scheduler_lifo scheduler_work_stealing
	./scheduler_lifo -g -s
	@for t in $$(seq 1 8); do \
		./scheduler_lifo -g -t $$t; \
	done
	@for t in $$(seq 1 8); do \
		./scheduler_work_stealing -g -t $$t; \
	done

scheduler_lifo: $(QUICKSORT) $(STACK) $(SCHED_LIFO) $(UTILS) $(BENCHMARK)
	$(CC) $(CFLAGS) -o $@ $^
scheduler_work_stealing: $(QUICKSORT) $(DEQUE) $(SCHED_WS) $(UTILS) $(BENCHMARK)
	$(CC) $(CFLAGS) -o $@ $^
scheduler_test_lifo: $(TEST_LIFO) $(STACK_TEST) $(UTILS)
	$(CC) $(CFLAGS) -o $@ $^
scheduler_test_ws: $(TEST_WS) $(DEQUE_TEST) $(UTILS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f scheduler_* src/*.o
	rm -f benchmark/*.txt