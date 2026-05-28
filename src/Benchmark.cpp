#include "../include/Benchmark.hpp"
#include "../include/Fourmiliere.hpp"
#include "../include/Affichage.hpp"
#include "../include/Ants.hpp"

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <queue>
#include <algorithm>
#include <chrono>
#include <functional>
#include <iomanip>

using namespace std;
using namespace std::chrono;

// ─────────────────────────────────────────────────────────────────────────────
// Réimplémentations silencieuses (sans affichage) pour mesurer uniquement
// le temps de calcul des chemins, sans le cout.
// ─────────────────────────────────────────────────────────────────────────────

// ---------- BFS silencieux ----------
static long long benchBFS(Fourmiliere fourmiliere) {
    auto debut = high_resolution_clock::now();

    map<string, vector<string>> adjacence;
    for (Arete* a = fourmiliere.teteAretes; a; a = a->suivant) {
        adjacence[a->depart].push_back(a->arrivee);
        adjacence[a->arrivee].push_back(a->depart);
    }

    map<string, map<int, int>> capacites;
    vector<int> tempsDepart(fourmiliere.nbFourmis, 0);
    vector<vector<string>> cheminsFourmis(fourmiliere.nbFourmis);

    for (int i = 0; i < fourmiliere.nbFourmis; ++i) {
        int depart = 0;
        vector<string> cheminFourmi;
        int compteur_securite = 0;
        const int LIMITE_SECURITE = 10000; // Limite d'itérations pour éviter boucle infinie

        while (cheminFourmi.empty()) {
            if (compteur_securite++ > LIMITE_SECURITE) {
                cout << "[ERREUR] Boucle infinie détectée pour la fourmi " << i << ", depart=" << depart << endl;
                break;
            }
            if (compteur_securite % 1000 == 0) {
                cout << "[DEBUG] Fourmi " << i << ", depart=" << depart << endl;
            }
            queue<vector<string>> file;
            file.push({fourmiliere.sommetDepart});

            while (!file.empty()) {
                vector<string> chemin = file.front();
                file.pop();
                string dernier = chemin.back();

                if (dernier == fourmiliere.sommetArrivee) {
                    cheminFourmi = chemin;
                    break;
                }
                for (const string& voisin : adjacence[dernier]) {
                    if (find(chemin.begin(), chemin.end(), voisin) == chemin.end()) {
                        int tempsPassage = depart + (int)chemin.size();
                        int cap = fourmiliere.getCapacite(voisin);
                        if (capacites[voisin][tempsPassage] < cap) {
                            vector<string> nv = chemin;
                            nv.push_back(voisin);
                            file.push(nv);
                        }
                    }
                }
            }
            if (cheminFourmi.empty()) depart++;
        }

        tempsDepart[i] = depart;
        cheminsFourmis[i] = cheminFourmi;
        for (size_t j = 1; j < cheminFourmi.size(); ++j)
            capacites[cheminFourmi[j]][depart + (int)j]++;
        fourmiliere.fourmis[i] = Ants(fourmiliere.sommetDepart, false);
    }

    auto fin = high_resolution_clock::now();
    return duration_cast<microseconds>(fin - debut).count();
}

// ---------- BFS Bidirectionnel silencieux ----------
static vector<string> reconstruireChemin(
    const string& rencontre,
    const map<string, string>& parentsAvant,
    const map<string, string>& parentsArriere,
    const string& depart,
    const string& arrivee)
{
    vector<string> chemin;
    string courant = rencontre;
    while (courant != depart) {
        chemin.push_back(courant);
        courant = parentsAvant.at(courant);
    }
    chemin.push_back(depart);
    reverse(chemin.begin(), chemin.end());

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

static vector<string> bidirBFS(
    const map<string, vector<string>>& adjacence,
    map<string, map<int, int>>& capacites,
    const Fourmiliere& fourmiliere,
    int tempsDepart)
{
    const string& src = fourmiliere.sommetDepart;
    const string& dst = fourmiliere.sommetArrivee;
    if (src == dst) return {src};

    queue<string> fileAvant, fileArriere;
    map<string, string> parentsAvant, parentsArriere;
    map<string, int> profAvant, profArriere;

    fileAvant.push(src);
    parentsAvant[src] = src;
    profAvant[src] = 0;

    fileArriere.push(dst);
    parentsArriere[dst] = dst;
    profArriere[dst] = 0;

    string noeudRdv = "";

    auto capDispo = [&](const string& noeud, int prof) -> bool {
        if (noeud == src || noeud == dst) return true;
        int t = tempsDepart + prof;
        int cap = fourmiliere.getCapacite(noeud);
        return capacites.count(noeud) == 0 ||
               capacites.at(noeud).count(t) == 0 ||
               capacites.at(noeud).at(t) < cap;
    };

    while (!fileAvant.empty() && !fileArriere.empty() && noeudRdv.empty()) {
        {
            int n = (int)fileAvant.size();
            for (int k = 0; k < n && noeudRdv.empty(); ++k) {
                string courant = fileAvant.front(); fileAvant.pop();
                for (const string& v : adjacence.at(courant)) {
                    if (parentsAvant.count(v)) continue;
                    int pv = profAvant[courant] + 1;
                    if (!capDispo(v, pv)) continue;
                    parentsAvant[v] = courant;
                    profAvant[v] = pv;
                    fileAvant.push(v);
                    if (parentsArriere.count(v)) { noeudRdv = v; break; }
                }
            }
        }
        if (!noeudRdv.empty()) break;
        {
            int n = (int)fileArriere.size();
            for (int k = 0; k < n && noeudRdv.empty(); ++k) {
                string courant = fileArriere.front(); fileArriere.pop();
                for (const string& v : adjacence.at(courant)) {
                    if (parentsArriere.count(v)) continue;
                    parentsArriere[v] = courant;
                    profArriere[v] = profArriere[courant] + 1;
                    fileArriere.push(v);
                    if (parentsAvant.count(v)) { noeudRdv = v; break; }
                }
            }
        }
    }
    if (noeudRdv.empty()) return {};
    return reconstruireChemin(noeudRdv, parentsAvant, parentsArriere, src, dst);
}

static long long benchBidirBFS(Fourmiliere fourmiliere) {
    auto debut = high_resolution_clock::now();

    map<string, vector<string>> adjacence;
    for (Arete* a = fourmiliere.teteAretes; a; a = a->suivant) {
        adjacence[a->depart].push_back(a->arrivee);
        adjacence[a->arrivee].push_back(a->depart);
    }
    for (Sommet* s = fourmiliere.teteSommets; s; s = s->suivant)
        if (!adjacence.count(s->nom)) adjacence[s->nom] = {};

    map<string, map<int, int>> capacites;
    vector<int> tempsDepart(fourmiliere.nbFourmis, 0);
    vector<vector<string>> cheminsFourmis(fourmiliere.nbFourmis);

    for (int i = 0; i < fourmiliere.nbFourmis; ++i) {
        int depart = 0;
        vector<string> cheminFourmi;
        while (cheminFourmi.empty()) {
            cheminFourmi = bidirBFS(adjacence, capacites, fourmiliere, depart);
            if (cheminFourmi.empty()) depart++;
        }
        tempsDepart[i] = depart;
        cheminsFourmis[i] = cheminFourmi;
        for (size_t j = 1; j < cheminFourmi.size(); ++j)
            capacites[cheminFourmi[j]][depart + (int)j]++;
        fourmiliere.fourmis[i] = Ants(fourmiliere.sommetDepart, false);
    }

    auto fin = high_resolution_clock::now();
    return duration_cast<microseconds>(fin - debut).count();
}

// ---------- A* silencieux ----------
struct NoeudAStar {
    string nomSommet;
    vector<string> cheminParcouru;
    int g, f;
    bool operator>(const NoeudAStar& o) const { return f > o.f; }
};

static map<string, int> calcHeuristique(const string& arrivee, map<string, vector<string>>& adj) {
    map<string, int> h;
    queue<string> file;
    h[arrivee] = 0;
    file.push(arrivee);
    while (!file.empty()) {
        string courant = file.front(); file.pop();
        for (const string& v : adj[courant])
            if (!h.count(v)) { h[v] = h[courant] + 1; file.push(v); }
    }
    return h;
}

static long long benchAStar(Fourmiliere fourmiliere) {
    auto debut = high_resolution_clock::now();

    map<string, vector<string>> adjacence;
    for (Arete* a = fourmiliere.teteAretes; a; a = a->suivant) {
        adjacence[a->depart].push_back(a->arrivee);
        adjacence[a->arrivee].push_back(a->depart);
    }
    for (Sommet* s = fourmiliere.teteSommets; s; s = s->suivant)
        if (!adjacence.count(s->nom)) adjacence[s->nom] = {};

    map<string, int> heuristique = calcHeuristique(fourmiliere.sommetArrivee, adjacence);
    map<string, map<int, int>> capacites;
    vector<int> tempsDepart(fourmiliere.nbFourmis, 0);
    vector<vector<string>> cheminsFourmis(fourmiliere.nbFourmis);

    for (int i = 0; i < fourmiliere.nbFourmis; ++i) {
        int depart = 0;
        vector<string> cheminFourmi;

        while (cheminFourmi.empty()) {
            priority_queue<NoeudAStar, vector<NoeudAStar>, greater<NoeudAStar>> fp;
            int h0 = heuristique.count(fourmiliere.sommetDepart) ? heuristique[fourmiliere.sommetDepart] : 0;
            fp.push({fourmiliere.sommetDepart, {fourmiliere.sommetDepart}, 0, h0});

            while (!fp.empty()) {
                NoeudAStar actuel = fp.top(); fp.pop();
                if (actuel.nomSommet == fourmiliere.sommetArrivee) {
                    cheminFourmi = actuel.cheminParcouru;
                    break;
                }
                for (const string& v : adjacence[actuel.nomSommet]) {
                    if (find(actuel.cheminParcouru.begin(), actuel.cheminParcouru.end(), v) == actuel.cheminParcouru.end()) {
                        int tp = depart + (int)actuel.cheminParcouru.size();
                        if (capacites[v][tp] < fourmiliere.getCapacite(v)) {
                            vector<string> nc = actuel.cheminParcouru;
                            nc.push_back(v);
                            int g2 = actuel.g + 1;
                            int hv = heuristique.count(v) ? heuristique[v] : 99999;
                            fp.push({v, nc, g2, g2 + hv});
                        }
                    }
                }
            }
            if (cheminFourmi.empty()) depart++;
        }

        tempsDepart[i] = depart;
        cheminsFourmis[i] = cheminFourmi;
        for (size_t j = 1; j < cheminFourmi.size(); ++j)
            capacites[cheminFourmi[j]][depart + (int)j]++;
        fourmiliere.fourmis[i] = Ants(fourmiliere.sommetDepart, false);
    }

    auto fin = high_resolution_clock::now();
    return duration_cast<microseconds>(fin - debut).count();
}

// ─────────────────────────────────────────────────────────────────────────────
// Affichage de la barre de progression
// ─────────────────────────────────────────────────────────────────────────────
static void afficherBarre(const string& nom, long long us, long long maxUs) {
    const int LARGEUR = 40;
    int rempli = (maxUs > 0) ? (int)((double)us / maxUs * LARGEUR) : 0;

    cout << "  " << left << setw(20) << nom << " | ";
    for (int i = 0; i < LARGEUR; ++i)
        cout << (i < rempli ? "#" : ".");

    // Affichage adaptatif : us ou ms selon la magnitude
    if (us < 1000)
        cout << " | " << right << setw(8) << us << " us\n";
    else
        cout << " | " << right << setw(7) << fixed << setprecision(2) << us / 1000.0 << " ms\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// Point d'entrée du benchmark
// ─────────────────────────────────────────────────────────────────────────────
void ResolutionBenchmark(Fourmiliere, const std::string& nomFichier) {
    const int NB_RUNS = 5; // Nombre de runs pour moyenner les mesures

    cout << "\n";
    cout << "+----------------------------------------------------+\n";
    cout << "|            BENCHMARK DES ALGORITHMES              |\n";
    cout << "+----------------------------------------------------+\n";
    cout << "|  " << NB_RUNS << " runs par algorithme - resultat = moyenne       |\n";
    cout << "+----------------------------------------------------+\n\n";

    auto moyenne = [&](function<long long(Fourmiliere)> fn) -> long long {
        long long total = 0;
        for (int r = 0; r < NB_RUNS; ++r) {
            Fourmiliere copie;
            copie.chargerDepuisFichier(nomFichier);
            total += fn(copie);
        }
        return total / NB_RUNS;
    };

    cout << "  Mesure BFS...               ";
    cout.flush();
    long long tBFS = moyenne(benchBFS);
    cout << "OK\n";

    cout << "  Mesure BFS Bidirectionnel...";
    cout.flush();
    long long tBidir = moyenne(benchBidirBFS);
    cout << "OK\n";

    cout << "  Mesure A*...                ";
    cout.flush();
    long long tAStar = moyenne(benchAStar);
    cout << "OK\n\n";

    long long maxT = max({tBFS, tBidir, tAStar});

    cout << "--------------------------------------------------------------\n";
    cout << "  Resultats (moyenne sur " << NB_RUNS << " runs)\n";
    cout << "--------------------------------------------------------------\n";
    afficherBarre("BFS",              tBFS,   maxT);
    afficherBarre("BFS Bidirectionnel", tBidir, maxT);
    afficherBarre("A*",               tAStar, maxT);
    cout << "--------------------------------------------------------------\n\n";

    // Classement
    vector<pair<long long, string>> classement = {
        {tBFS,   "BFS"},
        {tBidir, "BFS Bidirectionnel"},
        {tAStar, "A*"}
    };
    sort(classement.begin(), classement.end());

    cout << "   Classement (du plus rapide au plus lent) :\n\n";
    const string medailles[] = {"  1", "  2", "  3"};
    for (int i = 0; i < 3; ++i) {
        long long us = classement[i].first;
        cout << medailles[i] << "  " << left << setw(22) << classement[i].second;
        if (us < 1000)
            cout << right << setw(8) << us << " us\n";
        else
            cout << right << setw(7) << fixed << setprecision(2) << us / 1000.0 << " ms\n";
    }

    // Rapport de vitesse
    cout << "\n  Rapport de vitesse :\n";
    if (classement[0].first > 0) {
        for (int i = 1; i < 3; ++i) {
            double ratio = (double)classement[i].first / classement[0].first;
            cout << "  * " << classement[i].second << " est "
                 << fixed << setprecision(2) << ratio
                 << "x plus lent que " << classement[0].second << "\n";
        }
    }
    cout << "\n";
}
