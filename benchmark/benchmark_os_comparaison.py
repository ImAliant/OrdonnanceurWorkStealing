import matplotlib.pyplot as plt

linux_4_core_path = 'benchmark/runtime_linux_4_core.txt'
linux_8_core_path = 'benchmark/runtime_linux_8_core.txt'
lulu_16_core_path = 'benchmark/runtime_lulu_16_core.txt'
mac_4_core_path = 'benchmark/runtime_macOs_4_core.txt'
windows_4_core_path = 'benchmark/runtime_windows_4_core.txt'

data_linux_4_core_lifo = {}
data_linux_4_core_ws = {}
data_linux_8_core_lifo = {}
data_linux_8_core_ws = {}
data_lulu_16_core_lifo = {}
data_lulu_16_core_ws = {}
data_mac_4_core_lifo = {}
data_mac_4_core_ws = {}
data_windows_4_core_lifo = {}
data_windows_4_core_ws = {}

def read_file(file_path, data_lifo, data_ws):
    with open(file_path, 'r') as file:
        for line in file:
            parts = line.strip().split()
            scheduler = parts[0]
            thread_count = int(parts[1])
            duration = float(parts[2])

            if scheduler == './scheduler_lifo':
                data_lifo[thread_count] = duration
            elif scheduler == './scheduler_work_stealing':
                data_ws[thread_count] = duration

def read_linux_4():
    read_file(linux_4_core_path, data_linux_4_core_lifo, data_linux_4_core_ws)

def read_linux_8():
    read_file(linux_8_core_path, data_linux_8_core_lifo, data_linux_8_core_ws)
    
def read_lulu_16():
    read_file(lulu_16_core_path, data_lulu_16_core_lifo, data_lulu_16_core_ws)

def read_mac_4():
    read_file(mac_4_core_path, data_mac_4_core_lifo, data_mac_4_core_ws)
                
def read_windows_4():
    read_file(windows_4_core_path, data_windows_4_core_lifo, data_windows_4_core_ws)
    
def create_lifo_graph():
    threads_16 = sorted(data_lulu_16_core_lifo.keys())
    threads_8 = sorted(data_linux_8_core_lifo.keys())
    durations_linux_4_core_lifo = [data_linux_4_core_lifo[thread] for thread in threads_8]
    durations_linux_8_core_lifo = [data_linux_8_core_lifo[thread] for thread in threads_8]
    durations_lulu_16_core_lifo = [data_lulu_16_core_lifo[thread] for thread in threads_16]
    durations_mac_4_core_lifo = [data_mac_4_core_lifo[thread] for thread in threads_8]
    durations_windows_4_core_lifo = [data_windows_4_core_lifo[thread] for thread in threads_8]
    
    plt.figure(figsize=(10, 6))
    plt.plot(threads_8, durations_linux_4_core_lifo, label='Linux 4 Core', marker='x')
    plt.plot(threads_8, durations_linux_8_core_lifo, label='Linux 8 Core', marker='o')
    plt.plot(threads_16, durations_lulu_16_core_lifo, label='Lulu 16 Core', marker='x')
    plt.plot(threads_8, durations_mac_4_core_lifo, label='Mac 4 Core', marker='s')
    plt.plot(threads_8, durations_windows_4_core_lifo, label='Windows 4 Core', marker='d')
    
    plt.xlabel('Number of Threads')
    plt.ylabel('Duration')
    plt.title('Performance Comparison of LIFO Scheduler')
    plt.legend()
    plt.grid(True)
    plt.show()
    
    
def create_ws_graph():
    threads_16 = sorted(data_lulu_16_core_lifo.keys())
    threads_8 = sorted(data_linux_8_core_lifo.keys())
    durations_linux_4_core_ws = [data_linux_4_core_ws[thread] for thread in threads_8]
    durations_linux_8_core_ws = [data_linux_8_core_ws[thread] for thread in threads_8]
    duration_lulu_16_core_ws = [data_lulu_16_core_ws[thread] for thread in threads_16]
    duration_mac_4_core_ws = [data_mac_4_core_ws[thread] for thread in threads_8]
    duration_windows_4_core_ws = [data_windows_4_core_ws[thread] for thread in threads_8]

    plt.figure(figsize=(10, 6))
    plt.plot(threads_8, durations_linux_4_core_ws, label='Linux 4 Core', marker='x')
    plt.plot(threads_8, durations_linux_8_core_ws, label='Linux 8 Core', marker='o')
    plt.plot(threads_16, duration_lulu_16_core_ws, label='Lulu 16 Core', marker='x')
    plt.plot(threads_8, duration_mac_4_core_ws, label='Mac 4 Core', marker='s')
    plt.plot(threads_8, duration_windows_4_core_ws, label='Windows 4 Core', marker='d')

    plt.xlabel('Number of Threads')
    plt.ylabel('Duration')
    plt.title('Performance Comparison of Work Stealing Scheduler')
    plt.legend()
    plt.grid(True)
    plt.show()

if __name__ == '__main__':
    read_linux_4()
    read_linux_8()
    read_lulu_16()
    read_mac_4()
    read_windows_4()
    
    create_lifo_graph()
    create_ws_graph()
