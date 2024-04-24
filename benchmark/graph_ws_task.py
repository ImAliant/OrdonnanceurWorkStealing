import matplotlib.pyplot as plt
import numpy as np

data_dict = {}

def get_dict(filename):
    with open(filename, 'r') as file:
        for line in file:
            tokens = line.strip().split()
            index_thread = int(tokens[0])
            task_comp = int(tokens[1])
            task_ws_comp = int(tokens[2])
            task_ws_fail = int(tokens[3])
            data_dict[index_thread] = [task_comp, task_ws_comp, task_ws_fail]


get_dict('result.txt')

threads = list(data_dict.keys())  # Indices des threads
tasks_comp = [data[0] for data in data_dict.values()]  # task_comp
tasks_ws_comp = [data[1] for data in data_dict.values()]  # task_ws_comp
tasks_ws_fail = [data[2] for data in data_dict.values()]  # task_ws_fail

# Configuration des positions des barres
x = np.arange(len(threads))  # les indices de threads
width = 0.25  # la largeur des barres

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 12))

# Premier graphique : barres groupées
ax1.bar(x, tasks_comp, width, color='blue', label='Complétées')
ax1.set_xlabel('Index des Threads')
ax1.set_ylabel('Nombre de Tâches')
ax1.set_title('Nombre de tâches complétées par thread')
ax1.set_xticks(x)
ax1.set_xticklabels([f'Thread {i}' for i in threads])
ax1.legend()

# Second graphique : barres pour les tâches complétées avec succès (WS) et échouées (WS)
ax2.bar(x - width/2, tasks_ws_comp, width, color='green',
        label='Complétées avec succès (WS)')
ax2.bar(x + width/2, tasks_ws_fail, width, color='red', label='Échouées (WS)')
ax2.set_xlabel('Index des Threads')
ax2.set_ylabel('Nombre de Tâches')
ax2.set_title('Tâches complétées avec succès et échouées par thread (WS)')
ax2.set_xticks(x)
ax2.set_xticklabels([f'Thread {i}' for i in threads])
ax2.legend()

# Affichage de la fenêtre de graphiques
# Ajuste automatiquement les sous-graphiques pour qu'ils ne se chevauchent pas
plt.tight_layout(pad=3.0, h_pad=5.0)
plt.show()
