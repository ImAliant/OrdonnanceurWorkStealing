import matplotlib.pyplot as plt

# Initialisation des dictionnaires pour stocker les données
data_lifo = {}
data_work_stealing = {}

# Lire le fichier
file_path = 'benchmark/runtime.txt'  # Mettez ici le chemin correct du fichier
with open(file_path, 'r') as file:
    for line in file:
        parts = line.strip().split()
        scheduler = parts[0].split('/')[-1]  # Extraire le nom du planificateur
        thread_count = int(parts[1])
        duration = float(parts[2])

        if scheduler == 'scheduler_lifo':
            data_lifo[thread_count] = duration
        elif scheduler == 'scheduler_work_stealing':
            data_work_stealing[thread_count] = duration

# Préparation des données pour le graphique
threads = sorted(data_lifo.keys())  # Les threads, supposés être les mêmes pour les deux planificateurs
durations_lifo = [data_lifo[thread] for thread in threads]
durations_work_stealing = [data_work_stealing[thread] for thread in threads]

### Étape 2 : Création du graphique

plt.figure(figsize=(10, 6))
plt.plot(threads, durations_lifo, label='LIFO Scheduler', marker='o')
plt.plot(threads, durations_work_stealing, label='Work Stealing Scheduler', marker='x')

plt.xlabel('Number of Threads')
plt.ylabel('Duration')
plt.title('Performance Comparison of Scheduling Strategies')
plt.legend()
plt.grid(True)
plt.show()
