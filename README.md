# Projet de Programmation Systeme - Ordonnanceur par work stealing

## Description

Le but de ce projet est de paralleliser un programme en creant deux ordonnanceurs:  
    un ordonnanceur LIFO et un ordonnanceur par work stealing.

La parallisation du programme permet d'ameliorer la puissance de calcul du processeur et donc d'améliorer le temps d'execution de celui-ci.

## Lancement du programme "quicksort"
### Compilation
Pour compiler l'ensemble du programme, exécutez simplement:
```
make
```
Pour compiler l'ordonnanceur LIFO:
```
make scheduler_lifo
```
Pour compiler l'ordonnanceur Work stealing:
```
make scheduler_work_stealing
```

### Execution
Pour executer la partie LIFO:
```
./scheduler_lifo
```
Pour executer la partie Work stealing:
```
./scheduler_work_stealing
```

### Démo
Plusieurs démonstrations sont disponibles:
- `make demo` pour afficher deux graphes:
    - un graphe de comparaison de temps d'execution entre l'ordonnanceur LIFO et Work Stealing
    - un graphe affichant les taches executées par chaque thread, les taches volées ratées et les taches volées réussies.
- `make demo_optimization` pour afficher un graphe de comparaison de temps d'execution entre la version optimisée et non optimisée de l'ordonnanceur Work Stealing.
- `make demo_serial` pour afficher un graphe de comparaison de temps entre la version sérial du quicksort et les deux ordonnanceurs en fonction du nombre de threads.

### Options
L'option `-d` permet l'affichage des messages de debug.  
L'option `-g` permet de générer les fichiers de benchmark.   
L'option `-o` permet de lancer la version optimisée du programme (pour l'ordonnanceur work stealing).
L'option `-s` permet de lancer la version serial du programme.  
L'option `-n <n>` permet de choisir le nombre `n` d'elements qui sera dans le tableau généré.  
L'option `-t <n>` permet de choisir le nombre `n` de threads executé en parallèle. 

### Graphes

Deux graphes de benchmark de comparaison entre l'ordonnanceur lifo et work stealing ont été réalisé dans le répertoire [benchmark/graph](https://gaufre.informatique.univ-paris-diderot.fr/dedeoglu/scheduler-lifo-work-stealing/tree/dev/benchmark/graph)
grâce aux programmes python présent dans le répertoire [benchmark](https://gaufre.informatique.univ-paris-diderot.fr/dedeoglu/scheduler-lifo-work-stealing/tree/dev/benchmark).
