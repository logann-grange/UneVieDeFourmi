# Projet Fourmiliere — Simulation de deplacement de fourmis
 
 
---
 
## Presentation
 
Ce projet simule le deplacement d'un ensemble de fourmis dans une fourmiliere modelisee sous forme de **graphe non oriente**. Chaque fourmi doit rejoindre le sommet d'arrivee (`Sd`) depuis le sommet de depart (`Sv`) en respectant les **contraintes de capacite** de chaque salle (sommet).
 
Trois algorithmes de recherche de chemin ont ete implementes et peuvent etre compares via un benchmark integre.
 
---
 
## Fonctionnalites
 
- Chargement d'une fourmiliere depuis un fichier texte
- Deplacement de N fourmis avec gestion des conflits de capacite par tour
- Affichage pas-a-pas de la simulation (tour par tour)
- Trois algorithmes de resolution au choix
- Benchmark pour comparer les performances des algorithmes
---
 
## Algorithmes implementes
 
| # | Algorithme | Description |
|---|-----------|-------------|
| 1 | **BFS** | Parcours en largeur classique. Trouve le chemin le plus court en nombre de noeuds. |
| 2 | **BFS Bidirectionnel** | Deux BFS simultanes depuis `Sv` et `Sd` qui se rejoignent au milieu. Plus efficace sur les grands graphes. |
| 3 | **A\*** | Algorithme informe utilisant une heuristique (distance BFS inverse depuis `Sd`). Oriente la recherche vers la destination. |
 
---
 
## Structure du projet
 
```
Projet-fourmiliere/
│
├── include/
│   ├── Fourmiliere.hpp       # Structure du graphe (sommets, aretes, fourmis)
│   ├── Ants.hpp              # Classe representant une fourmi
│   ├── ResolutionBFS.hpp
│   ├── BidirectionalBFS.hpp
│   ├── ResolutionAStar.hpp
│   ├── Benchmark.hpp
│   └── Affichage.hpp
│
├── src/
│   ├── Fourmiliere.cpp       # Chargement fichier, gestion capacites
│   ├── Ants.cpp
│   ├── ResolutionBFS.cpp
│   ├── BidirectionalBFS.cpp
│   ├── ResolutionAstar.cpp
│   ├── Benchmark.cpp         # Mesure comparative des 3 algorithmes
│   ├── Affichage.cpp
│   └── main.cpp
│
└── fourmilieres/
    ├── fourmiliere_cinq.txt  # Exemple de fichier d'entree
    └── ...
```
 
---
 
## Format du fichier d'entree
 
```
f=10
Sv
Sd
S1 { 8 }
S2 { 4 }
S3 { 2 }
Sv - S1
S1 - S2
S2 - S3
S3 - Sd
```
 
- `f=N` : nombre de fourmis
- `Sv` : sommet de depart (capacite illimitee)
- `Sd` : sommet d'arrivee (capacite illimitee)
- `SX { C }` : sommet avec capacite C (defaut = 1 si non precise)
- `SX - SY` : arete entre deux sommets (graphe non oriente)
---
 
## Compilation et execution
 
### Compilation (g++)
 
```bash
g++ -std=c++17 -o main.exe \
    src/main.cpp \
    src/Fourmiliere.cpp \
    src/Ants.cpp \
    src/ResolutionBFS.cpp \
    src/BidirectionalBFS.cpp \
    src/ResolutionAstar.cpp \
    src/Benchmark.cpp \
    src/Affichage.cpp
```
 
### Execution
 
```bash
./main.exe <nom_du_fichier.txt>
```
 
Exemple :
 
```bash
./main.exe fourmiliere_cinq.txt
```
 
---
 
## Menu de resolution
 
```
==============================
      MENU DE RESOLUTION
==============================
  1. Resolution par BFS
  2. Resolution par BFS Bidirectionnel
  3. Resolution par A*
  4. Benchmark (comparer les 3 algorithmes)
------------------------------
```
 
L'option **4 — Benchmark** execute les trois algorithmes sur la meme fourmiliere (5 runs chacun), affiche les temps moyens et classe les algorithmes du plus rapide au plus lent.
 
---
 
## Exemple de sortie (simulation)
 
```
========== Tour 1 ==========
F1 -> S1
F2 -> S1
 
========== Tour 2 ==========
F1 -> S2
F2 -> S3
F3 -> S1
...
```
 
---
 
## Repartition du travail
 
| Membre | Contributions principales |
|--------|--------------------------|
| Mohaled | Algorithme BFS (ResolutionBFS.cpp), structure generale du projet |
| Daroueche | Algorithme A* (ResolutionAstar.cpp), calcul heuristique |
| Logann | Algorithme BFS Bidirectionnel (BidirectionalBFS.cpp), reconstruction de chemin |
 
---
 
## Auteurs
   
Mohaled — Daroueche — Logann
