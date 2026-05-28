#include "../include/Ants.hpp"
#include "../include/Fourmiliere.hpp"
#include "../include/Affichage.hpp"
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <queue>
#include <algorithm>

using namespace std;

// Structure pour représenter un nœud dans la file de priorité de A*
struct NoeudAStar {
    string nomSommet;
    vector<string> cheminParcouru;
    int g; // Coût du chemin depuis le départ
    int f; // Coût total estimé (g + h)

    // Surcharge de l'opérateur pour que la file de priorité garde le plus petit 'f' au-dessus
    bool operator>(const NoeudAStar& autre) const {
        return f > autre.f;
    }
};

// Fonction pour calculer l'heuristique (Distance la plus courte de chaque sommet à Sd)
// On utilise un BFS partant de Sd pour cartographier le graphe à l'envers
static map<string, int> calculerHeuristique(const string& arrivee, map<string, vector<string>>& adjacence) {
    map<string, int> h;
    queue<string> file;
    
    h[arrivee] = 0;
    file.push(arrivee);

    while (!file.empty()) {
        string courant = file.front();
        file.pop();

        for (const string& voisin : adjacence[courant]) {
            if (h.find(voisin) == h.end()) { // Si pas encore visité
                h[voisin] = h[courant] + 1;
                file.push(voisin);
            }
        }
    }
    return h;
}

void ResolutionAStar(Fourmiliere fourmiliere) {
    map<string, vector<string>> adjacence;

    // Construction de la liste d'adjacence
    for (Arete* a = fourmiliere.teteAretes; a; a = a->suivant) {
        adjacence[a->depart].push_back(a->arrivee);
        adjacence[a->arrivee].push_back(a->depart);
    }

    // Assurer que chaque sommet existe dans la map d'adjacence
    for (Sommet* s = fourmiliere.teteSommets; s; s = s->suivant) {
        if (!adjacence.count(s->nom)) adjacence[s->nom] = {};
    }

    // Calcul des valeurs heuristiques h(n) vers la destination 'Sd'
    map<string, int> heuristique = calculerHeuristique(fourmiliere.sommetArrivee, adjacence);

    map<string, map<int, int>> capacites;
    vector<int> tempsDepart(fourmiliere.nbFourmis, 0);
    vector<vector<string>> cheminsFourmis(fourmiliere.nbFourmis);

    // Recherche de chemin pour chaque fourmi
    for (int i = 0; i < fourmiliere.nbFourmis; ++i) {
        int depart = 0;
        vector<string> cheminFourmi;

        while (cheminFourmi.empty()) {
            // File de priorité (Min-Heap) basée sur la valeur 'f'
            priority_queue<NoeudAStar, vector<NoeudAStar>, greater<NoeudAStar>> filePriorite;
            
            int h_depart = heuristique.count(fourmiliere.sommetDepart) ? heuristique[fourmiliere.sommetDepart] : 0;
            filePriorite.push({fourmiliere.sommetDepart, {fourmiliere.sommetDepart}, 0, h_depart});

            while (!filePriorite.empty()) {
                NoeudAStar actuel = filePriorite.top();
                filePriorite.pop();

                string dernier = actuel.nomSommet;

                if (dernier == fourmiliere.sommetArrivee) {
                    cheminFourmi = actuel.cheminParcouru;
                    break;
                }

                for (const string& voisin : adjacence[dernier]) {
                    // Éviter les cycles locaux dans le chemin actuel
                    if (find(actuel.cheminParcouru.begin(), actuel.cheminParcouru.end(), voisin) == actuel.cheminParcouru.end()) {
                        int tempsPassage = depart + (int)actuel.cheminParcouru.size();
                        int capMax = fourmiliere.getCapacite(voisin);

                        // Vérification des contraintes de capacité au tour par étape
                        if (capacites[voisin][tempsPassage] < capMax) {
                            vector<string> nouveauChemin = actuel.cheminParcouru;
                            nouveauChemin.push_back(voisin);

                            int g_nouveau = actuel.g + 1;
                            int h_voisin = heuristique.count(voisin) ? heuristique[voisin] : 99999; // Pénalité si non atteignable
                            
                            filePriorite.push({voisin, nouveauChemin, g_nouveau, g_nouveau + h_voisin});
                        }
                    }
                }
            }
            if (cheminFourmi.empty()) depart++;
        }

        tempsDepart[i] = depart;
        cheminsFourmis[i] = cheminFourmi;

        // Réservation des capacités pour le chemin de cette fourmi
        for (size_t j = 1; j < cheminFourmi.size(); ++j) {
            capacites[cheminFourmi[j]][depart + (int)j]++;
        }

        fourmiliere.fourmis[i] = Ants(fourmiliere.sommetDepart, false);
    }

    // Calcul du tour maximum global de simulation
    int tourMax = 0;
    for (int i = 0; i < fourmiliere.nbFourmis; ++i) {
        if (cheminsFourmis[i].empty()) continue;
        int fin = tempsDepart[i] + (int)cheminsFourmis[i].size() - 1;
        if (fin > tourMax) tourMax = fin;
    }

    // Affichage des résultats
    afficherEtapes(fourmiliere, cheminsFourmis, tempsDepart, tourMax);
}