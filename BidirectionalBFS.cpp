#include "BidirectionalBFS.hpp"
#include "Ants.hpp"
#include "Fourmiliere.hpp"
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <queue>
#include <algorithm>

using namespace std;

// ─────────────────────────────────────────────────────────────────────────────
// Reconstruit le chemin complet à partir des deux maps de parents
// ─────────────────────────────────────────────────────────────────────────────
static std::vector<std::string> reconstruireChemin(
    const std::string& rencontre,
    const std::map<std::string, std::string>& parentsAvant,
    const std::map<std::string, std::string>& parentsArriere,
    const std::string& depart,
    const std::string& arrivee)
{
    std::vector<std::string> chemin;

    // Moitié avant : rencontre -> depart (on remonte puis on inverse)
    std::string courant = rencontre;
    while (courant != depart) {
        chemin.push_back(courant);
        courant = parentsAvant.at(courant);
    }
    chemin.push_back(depart);
    std::reverse(chemin.begin(), chemin.end());

    // Moitié arrière : rencontre -> arrivee
    // On ne repart que si le nœud de rencontre n'est pas déjà l'arrivée
    if (rencontre != arrivee && parentsArriere.count(rencontre)) {
        courant = parentsArriere.at(rencontre);
        while (courant != arrivee) {
            // Éviter les doublons avec la moitié avant
            if (courant != chemin.back()) chemin.push_back(courant);
            courant = parentsArriere.at(courant);
        }
        chemin.push_back(arrivee);
    }

    return chemin;
}

// ─────────────────────────────────────────────────────────────────────────────
// BFS bidirectionnel 
// ─────────────────────────────────────────────────────────────────────────────
static vector<string> bidirectionalBFS(
    const map<string, vector<string>>& adjacence,
    map<string, map<int, int>>& capacites,
    const Fourmiliere& fourmiliere,
    int tempsDepart)
{
    const string& src = fourmiliere.sommetDepart;
    const string& dst = fourmiliere.sommetArrivee;

    if (src == dst) return {src};

    // Files d'exploration (une par direction)
    queue<string> fileAvant, fileArriere;

    // Parents pour reconstruction du chemin
    map<string, string> parentsAvant, parentsArriere;

    // Profondeur de chaque nœud depuis son côté de départ
    map<string, int> profAvant, profArriere;

    // Initialisation côté source
    fileAvant.push(src);
    parentsAvant[src] = src;
    profAvant[src]    = 0;

    // Initialisation côté destination
    fileArriere.push(dst);
    parentsArriere[dst] = dst;
    profArriere[dst]    = 0;

    string noeudRendezVous = "";

    // Vérifie si un nœud est disponible (capacité non saturée) à un instant t
    auto capaciteDisponible = [&](const string &noeud, int profondeur) -> bool {
        if (noeud == src || noeud == dst) return true;
        int t = tempsDepart + profondeur;
        int cap = fourmiliere.getCapacite(noeud);
        return capacites[noeud][t] < cap;
    };

    // Alternance niveau par niveau : avant puis arrière
    while (!fileAvant.empty() && !fileArriere.empty() && noeudRendezVous.empty()) {

        // ── Étape avant ──────────────────────────────────────────────────────
        {
            int tailleNiveau = (int)fileAvant.size();
            for (int k = 0; k < tailleNiveau && noeudRendezVous.empty(); ++k) {
                string courant = fileAvant.front();
                fileAvant.pop();

                for (const string &voisin : adjacence.at(courant)) {
                    if (parentsAvant.count(voisin)) continue; // déjà visité

                    int profVoisin = profAvant[courant] + 1;
                    if (!capaciteDisponible(voisin, profVoisin)) continue;

                    parentsAvant[voisin] = courant;
                    profAvant[voisin]    = profVoisin;
                    fileAvant.push(voisin);

                    if (parentsArriere.count(voisin)) {
                        noeudRendezVous = voisin; // jonction trouvée
                        break;
                    }
                }
            }
        }

        if (!noeudRendezVous.empty()) break;

        // ── Étape arrière ─────────────────────────────────────────────────────
        {
            int tailleNiveau = (int)fileArriere.size();
            for (int k = 0; k < tailleNiveau && noeudRendezVous.empty(); ++k) {
                string courant = fileArriere.front();
                fileArriere.pop();

                for (const string& voisin : adjacence.at(courant)) {
                    if (parentsArriere.count(voisin)) continue;

                    parentsArriere[voisin] = courant;
                    profArriere[voisin]    = profArriere[courant] + 1;
                    fileArriere.push(voisin);

                    if (parentsAvant.count(voisin)) {
                        noeudRendezVous = voisin;
                        break;
                    }
                }
            }
        }
    }

    if (noeudRendezVous.empty()) return {}; // aucun chemin

    return reconstruireChemin(noeudRendezVous, parentsAvant, parentsArriere, src, dst);
}

// ─────────────────────────────────────────────────────────────────────────────
// Point d'entrée principal
// ─────────────────────────────────────────────────────────────────────────────
void ResolutionBidirectionalBFS(Fourmiliere fourmiliere) {

    // Construction de la liste d'adjacence (graphe non orienté)
    map<string, vector<string>> adjacence;
    for (Arete* a = fourmiliere.teteAretes; a; a = a->suivant) {
        adjacence[a->depart].push_back(a->arrivee);
        adjacence[a->arrivee].push_back(a->depart);
    }
    // Garantir une entrée pour chaque sommet (même isolé)
    for (Sommet* s = fourmiliere.teteSommets; s; s = s->suivant) {
        if (!adjacence.count(s->nom)) adjacence[s->nom] = {};
    }

    map<string, map<int, int>> capacites;
    vector<int> tempsDepart(fourmiliere.nbFourmis, 0);
    vector<vector<string>> cheminsFourmis(fourmiliere.nbFourmis);

    // Calcul du chemin de chaque fourmi
    for (int i = 0; i < fourmiliere.nbFourmis; ++i) {
        int depart = 0;
        vector<string> cheminFourmi;

        // On retarde le départ jusqu'à trouver un chemin sans conflit de capacité
        while (cheminFourmi.empty()) {
            cheminFourmi = bidirectionalBFS(adjacence, capacites, fourmiliere, depart);
            if (cheminFourmi.empty()) depart++;
        }

        tempsDepart[i] = depart;
        cheminsFourmis[i] = cheminFourmi;

        // Réserver les capacités sur le chemin trouvé
        for (size_t j = 1; j < cheminFourmi.size(); ++j) {
            capacites[cheminFourmi[j]][depart + (int)j]++;
        }

        fourmiliere.fourmis[i] = Ants(fourmiliere.sommetDepart, false);
    }

    // Calcul du tour max (dernier moment où une fourmi est encore en mouvement)
    int tourMax = 0;
    for (int i = 0; i < fourmiliere.nbFourmis; ++i) {
        if (cheminsFourmis[i].empty()) continue;
        int fin = tempsDepart[i] + (int)cheminsFourmis[i].size() - 1;
        if (fin > tourMax) tourMax = fin;
    }

    // Affichage tour par tour
    afficherEtapes(fourmiliere, cheminsFourmis, tempsDepart, tourMax);
}