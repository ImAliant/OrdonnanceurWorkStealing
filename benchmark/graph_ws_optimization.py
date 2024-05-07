import matplotlib.pyplot as plt

data_normal = {}
data_optimized = {}

file_normal_path = 'benchmark/runtime.txt'
file_optimized_path = 'benchmark/runtime_optimized.txt'

with open(file_normal_path, 'r') as file:
    for line in file:
        parts = line.strip().split()
        scheduler = parts[0].split('/')[-1]
        thread_count = int(parts[1])
        duration = float(parts[2])
    
        data_normal[thread_count] = duration

with open(file_optimized_path, 'r') as file:
    for line in file:
        parts = line.strip().split()
        scheduler = parts[0].split('/')[-1]
        thread_count = int(parts[1])
        duration = float(parts[2])
    
        data_optimized[thread_count] = duration
        
threads = sorted(data_normal.keys())
durations_normal = [data_normal[thread] for thread in threads]
durations_optimized = [data_optimized[thread] for thread in threads]

plt.figure(figsize=(10, 6))
plt.plot(threads, durations_normal, label='Normal Execution', marker='o')
plt.plot(threads, durations_optimized, label='Optimized Execution', marker='x')

plt.xlabel('Number of Threads')
plt.ylabel('Duration')
plt.title('Performance Comparison of Normal and Optimized Execution')
plt.legend()
plt.grid(True)
plt.show()
    
