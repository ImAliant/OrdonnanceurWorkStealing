import matplotlib.pyplot as plt

serial_path = 'benchmark/serial.txt'
parallel_lifo_path = 'benchmark/parallel_lifo.txt'
parallel_ws_path = 'benchmark/parallel_ws.txt'

serial_data = {}
parallel_lifo_data = {}
parallel_ws_data = {}

def read_file(file_path, data):
    with open(file_path, 'r') as file:
        for line in file:
            parts = line.strip().split()
            array_size = int(parts[1])
            duration = float(parts[2])
            
            data[array_size] = duration
    
def create_graph():
    array_sizes = sorted(serial_data.keys())
    durations_serial = [serial_data[array_size] for array_size in array_sizes]
    durations_lifo = [parallel_lifo_data[array_size] for array_size in array_sizes]
    durations_ws = [parallel_ws_data[array_size] for array_size in array_sizes]
    
    plt.figure(figsize=(10, 6))
    plt.plot(array_sizes, durations_serial, label='Serial Execution', linestyle='--')
    plt.plot(array_sizes, durations_lifo, label='LIFO Scheduler', marker='o')
    plt.plot(array_sizes, durations_ws, label='Work Stealing Scheduler', marker='x')
    
    plt.xlabel('Array Size')
    plt.ylabel('Duration')
    plt.title('Performance Comparison of Scheduling Strategies')
    plt.legend()
    plt.grid(True)
    plt.show()        

if __name__ == '__main__':
    read_file(serial_path, serial_data)
    read_file(parallel_lifo_path, parallel_lifo_data)
    read_file(parallel_ws_path, parallel_ws_data)
    
    create_graph()