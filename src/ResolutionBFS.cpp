#include "../include/Ants.hpp"
#include "../include/Fourmiliere.hpp"
#include "../include/Affichage.hpp"
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>
#include <climits>
#include <queue>
#include <algorithm>

void ResolutionBFS(Fourmiliere fourmiliere) {
    std::map<std::string, std::vector<std::string>> adjacence;

    for (Arete* a = fourmiliere.teteAretes; a; a = a->suivant) {
        adjacence[a->depart].push_back(a->arrivee);
        adjacence[a->arrivee].push_back(a->depart);
    }

    std::map<std::string, std::map<int, int>> capacites;
    std::vector<int> tempsDepart(fourmiliere.nbFourmis, 0);
    std::vector<std::vector<std::string>> cheminsFourmis(fourmiliere.nbFourmis);

    for (int i = 0; i < fourmiliere.nbFourmis; ++i) {
        int depart = 0;
        std::vector<std::string> cheminFourmi;

        while (cheminFourmi.empty()) {
            std::queue<std::vector<std::string>> file;
            file.push({fourmiliere.sommetDepart});

            while (!file.empty()) {
                std::vector<std::string> chemin = file.front();
                file.pop();
                std::string dernier = chemin.back();

                if (dernier == fourmiliere.sommetArrivee) {
                    cheminFourmi = chemin;
                    break;
                }

                for (const std::string& voisin : adjacence[dernier]) {
                    if (std::find(chemin.begin(), chemin.end(), voisin) == chemin.end()) {
                        int tempsPassage = depart + (int)chemin.size();
                        int cap = fourmiliere.getCapacite(voisin);
                        if (capacites[voisin][tempsPassage] < cap) {
                            std::vector<std::string> nouveauChemin = chemin;
                            nouveauChemin.push_back(voisin);
                            file.push(nouveauChemin);
                        }
                    }
                }
            }
            if (cheminFourmi.empty()) depart++;
        }

        tempsDepart[i] = depart;
        cheminsFourmis[i] = cheminFourmi;
        for (size_t j = 1; j < cheminFourmi.size(); ++j) {
            capacites[cheminFourmi[j]][depart + j]++;
        }
        fourmiliere.fourmis[i] = Ants(fourmiliere.sommetDepart, false);
    }

    // Calcul du tour max
    int tourMax = 0;
    for (int i = 0; i < fourmiliere.nbFourmis; ++i) {
        if (cheminsFourmis[i].empty()) continue;
        int fin = tempsDepart[i] + (int)cheminsFourmis[i].size() - 1;
        if (fin > tourMax) tourMax = fin;
    }

    // Affichage
    afficherEtapes(fourmiliere, cheminsFourmis, tempsDepart, tourMax);
}