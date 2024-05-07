import matplotlib.pyplot as plt

runtime_serial = 0
data_lifo = {}
data_ws = {}

file_path = 'benchmark/runtime.txt'
with open(file_path, 'r') as file:
    line = file.readline()
    parts = line.strip().split()
    runtime_serial = float(parts[2])
    
    for line in file:
        parts = line.strip().split()
        scheduler = parts[0].split('/')[-1]
        thread_count = int(parts[1])
        duration = float(parts[2])

        if scheduler == 'scheduler_lifo':
            data_lifo[thread_count] = duration
        elif scheduler == 'scheduler_work_stealing':
            data_ws[thread_count] = duration

threads = sorted(data_lifo.keys())
durations_lifo = [data_lifo[thread] for thread in threads]
duration_ws = [data_ws[thread] for thread in threads]

plt.figure(figsize=(10, 6))
plt.plot(threads, [runtime_serial] * len(threads), label='Serial Execution', linestyle='--')
plt.plot(threads, durations_lifo, label='LIFO Scheduler', marker='o')
plt.plot(threads, duration_ws, label='Work Stealing Scheduler', marker='x')

plt.xlabel('Number of Threads')
plt.ylabel('Duration')
plt.title('Performance Comparison of Scheduling Strategies')
plt.legend()
plt.grid(True)
plt.show()