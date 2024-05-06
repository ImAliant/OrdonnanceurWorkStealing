# Projet de Programmation Systeme - Ordonnanceur par work stealing

## Description

Le but de ce projet est de paralleliser un programme en creant deux ordonnaceurs: 
    un ordonnanceur LIFO et un ordonnanceur par work stealing.

La parallisation du programme permet d'ameliorer la puissance de calcul du processeur et donc d'ameliorer le temps d'execution de celui-ci.
## Lancement du programme "quicksort"
### Compilation
Pour compiler l'ensemble du programme, executez simplement:
```
make
```
Pour compiler l'ordonnanceur LIFO:
```
make scheduler_lifo
```
Pour compiler l'ordonnanceur Work-Stealing:
```
make scheduler_work_stealing
```

### Execution
Pour executer la partie LIFO:
```
./scheduler_lifo
```
Pour executer la partie Work-stealing:
```
./scheduler_work_stealing
```

### Options
L'option '-d' permet l'affichage des messages de debug. 
L'option '-g' permet de generer le fichier de benchmark. 
L'option '-s' permet de lancer la version serial du programme. 
L'option '-n <n>' permet de choisir le nombre d'elements qui sera dans le tableau trie.
L'option '-t' permet de choisir le nombre de threads executé en parallele.


