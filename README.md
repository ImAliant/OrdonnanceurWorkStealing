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

### Options
L'option `-d` permet l'affichage des messages de debug.  
L'option `-g` permet de générer le fichier de benchmark.   
L'option `-s` permet de lancer la version serial du programme.  
L'option `-n <n>` permet de choisir le nombre `n` d'elements qui sera dans le tableau généré.  
L'option `-t <n>` permet de choisir le nombre `n` de threads executé en parallèle. 

### Graphes

Deux graphes de benchmark de comparaison entre l'ordonnanceur lifo et work stealing ont été réalisé dans le répertoire [benchmark/graph](https://gaufre.informatique.univ-paris-diderot.fr/dedeoglu/scheduler-lifo-work-stealing/tree/dev/benchmark/graph)
grâce aux programmes python présent dans le répertoire [benchmark](https://gaufre.informatique.univ-paris-diderot.fr/dedeoglu/scheduler-lifo-work-stealing/tree/dev/benchmark).
