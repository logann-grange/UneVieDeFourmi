#include "../include/BidirectionalBFS.hpp"
#include "../include/Ants.hpp"
#include "../include/Fourmiliere.hpp"
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
static vector<string> reconstruireChemin(
    const string& rencontre,
    const map<string, string>& parentsAvant,
    const map<string, string>& parentsArriere,
    const string& depart,
    const string& arrivee)
{
    vector<string> chemin;

    // Moitié avant : rencontre -> depart (on remonte puis on inverse)
    string courant = rencontre;
    while (courant != depart) {
        chemin.push_back(courant);
        courant = parentsAvant.at(courant);
    }
    chemin.push_back(depart);
    reverse(chemin.begin(), chemin.end());

    // Moitié arrière : rencontre -> arrivee
    // On ne repart que si le noeud de rencontre n'est pas déjà l'arrivée
    if (rencontre != arrivee && parentsArriere.count(rencontre)) {
        courant = parentsArriere.at(rencontre);
        while (courant != arrivee) {
            if (courant != chemin.back()) chemin.push_back(courant);
            courant = parentsArriere.at(courant);
        }
        chemin.push_back(arrivee);
    }

    return chemin;
}

// ─────────────────────────────────────────────────────────────────────────────
// Vérifie que chaque noeud du chemin respecte sa capacité à l'instant de passage
// ─────────────────────────────────────────────────────────────────────────────
static bool validerChemin(
    const vector<string>& chemin,
    map<string, map<int, int>>& capacites,
    const Fourmiliere& fourmiliere,
    int tempsDepart)
{
    for (size_t j = 1; j < chemin.size(); ++j) {
        const string& noeud = chemin[j];
        if (noeud == fourmiliere.sommetArrivee) continue;
        int t   = tempsDepart + (int)j;
        int cap = fourmiliere.getCapacite(noeud);
        if (capacites[noeud][t] >= cap) return false;
    }
    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// BFS bidirectionnel — retourne TOUS les chemins de longueur minimale
// pour permettre de choisir celui dont les capacités sont libres.
// ─────────────────────────────────────────────────────────────────────────────
static vector<vector<string>> bidirectionalBFS(
    const map<string, vector<string>>& adjacence,
    map<string, map<int, int>>& capacites,
    const Fourmiliere& fourmiliere,
    int tempsDepart)
{
    const string& src = fourmiliere.sommetDepart;
    const string& dst = fourmiliere.sommetArrivee;

    if (src == dst) return {{src}};

    queue<string> fileAvant, fileArriere;
    map<string, string> parentsAvant, parentsArriere;
    map<string, int> profAvant, profArriere;

    // Initialisation côté source
    fileAvant.push(src);
    parentsAvant[src] = src;
    profAvant[src]    = 0;

    // Initialisation côté destination
    fileArriere.push(dst);
    parentsArriere[dst] = dst;
    profArriere[dst]    = 0;

    // Noeuds de jonction trouvés au niveau courant
    vector<string> noeudsCandidats;

    // Vérifie la capacité côté avant (temps de passage connu précisément)
    auto capaciteDisponible = [&](const string& noeud, int profondeur) -> bool {
        if (noeud == src || noeud == dst) return true;
        int t   = tempsDepart + profondeur;
        int cap = fourmiliere.getCapacite(noeud);
        return capacites[noeud][t] < cap;
    };

    // Alternance niveau par niveau : avant puis arrière
    while (!fileAvant.empty() && !fileArriere.empty() && noeudsCandidats.empty()) {

        // ── Étape avant ──────────────────────────────────────────────────────
        {
            int tailleNiveau = (int)fileAvant.size();
            for (int k = 0; k < tailleNiveau; ++k) {
                string courant = fileAvant.front();
                fileAvant.pop();

                for (const string& voisin : adjacence.at(courant)) {
                    if (parentsAvant.count(voisin)) continue;

                    int profVoisin = profAvant[courant] + 1;
                    if (!capaciteDisponible(voisin, profVoisin)) continue;

                    parentsAvant[voisin] = courant;
                    profAvant[voisin]    = profVoisin;
                    fileAvant.push(voisin);

                    // Jonction : accumuler TOUS les noeuds de rencontre
                    // sans s'arrêter au premier
                    if (parentsArriere.count(voisin)) {
                        noeudsCandidats.push_back(voisin);
                    }
                }
            }
        }

        if (!noeudsCandidats.empty()) break;

        // ── Étape arrière ─────────────────────────────────────────────────────
        {
            int tailleNiveau = (int)fileArriere.size();
            for (int k = 0; k < tailleNiveau; ++k) {
                string courant = fileArriere.front();
                fileArriere.pop();

                for (const string& voisin : adjacence.at(courant)) {
                    if (parentsArriere.count(voisin)) continue;

                    parentsArriere[voisin] = courant;
                    profArriere[voisin]    = profArriere[courant] + 1;
                    fileArriere.push(voisin);

                    if (parentsAvant.count(voisin)) {
                        noeudsCandidats.push_back(voisin);
                    }
                }
            }
        }
    }

    if (noeudsCandidats.empty()) return {};

    // Reconstruire un chemin pour chaque noeud de jonction trouvé
    vector<vector<string>> chemins;
    for (const string& rdv : noeudsCandidats) {
        vector<string> c = reconstruireChemin(rdv, parentsAvant, parentsArriere, src, dst);
        if (!c.empty()) chemins.push_back(c);
    }

    return chemins;
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

        while (cheminFourmi.empty()) {
            // Récupérer tous les chemins de longueur minimale
            vector<vector<string>> candidats =
                bidirectionalBFS(adjacence, capacites, fourmiliere, depart);

            // Choisir le premier chemin valide parmi tous les candidats
            for (const vector<string>& candidat : candidats) {
                if (validerChemin(candidat, capacites, fourmiliere, depart)) {
                    cheminFourmi = candidat;
                    break;
                }
            }

            // Aucun chemin valide à ce temps de départ → retarder
            if (cheminFourmi.empty()) depart++;
        }

        tempsDepart[i]    = depart;
        cheminsFourmis[i] = cheminFourmi;

        // Réserver les capacités sur le chemin trouvé
        for (size_t j = 1; j < cheminFourmi.size(); ++j) {
            capacites[cheminFourmi[j]][depart + (int)j]++;
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

    // Affichage tour par tour
    afficherEtapes(fourmiliere, cheminsFourmis, tempsDepart, tourMax);
}