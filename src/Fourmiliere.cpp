#include "../include/Fourmiliere.hpp"
#include "../include/Ants.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>
#include <string>
#include <map>
#include <climits>
#include <vector>
#include <queue>

using namespace std;


Fourmiliere::Fourmiliere() : nbFourmis(0), teteSommets(nullptr), teteAretes(nullptr) {}

Fourmiliere::~Fourmiliere() {
    // Libérer la liste chaînée des sommets
    while (teteSommets) {
        Sommet* tmp = teteSommets;
        teteSommets = teteSommets->suivant;
        delete tmp;
    }
    // Libérer la liste chaînée des arêtes
    while (teteAretes) {
        Arete* tmp = teteAretes;
        teteAretes = teteAretes->suivant;
        delete tmp;
    }
}

bool Fourmiliere::chargerDepuisFichier(const string &nomFichier) {
    ifstream fichier(nomFichier);
    if (!fichier) return false;
    string ligne;
    regex regexFourmis("^[Ff]=([0-9]+)");
    regex regexSommet("^(S[0-9]+|Sv|Sd)(?: \\{ *([0-9]+) *\\})?");
    regex regexArete("^(S[0-9]+|Sv|Sd) *- *(S[0-9]+|Sv|Sd)");
    smatch match;
    while (getline(fichier, ligne)) {
        if (regex_search(ligne, match, regexFourmis)) {
            nbFourmis = stoi(match[1]);
            fourmis.resize(nbFourmis);
        } else if (regex_search(ligne, match, regexArete)) {
            string d = match[1];
            string a = match[2];
            Arete* ar = new Arete(d, a);
            ar->suivant = teteAretes;
            teteAretes = ar;
            // Ajouter les sommets s'ils n'existent pas déjà
            for (const string& nomSommet : {d, a}) {
                bool existe = false;
                for (Sommet* s = teteSommets; s; s = s->suivant) {
                    if (s->nom == nomSommet) {
                        existe = true;
                        break;
                    }
                }
                if (!existe) {
                    Sommet* s = new Sommet(nomSommet, 1);
                    s->suivant = teteSommets;
                    teteSommets = s;
                }
            }
        } else if (regex_search(ligne, match, regexSommet)) {
            string nom = match[1];
            int cap = match[2].matched ? stoi(match[2]) : 1; // Par défaut, capacité = 1
            // Vérifier si le sommet existe déjà
            bool existe = false;
            for (Sommet* s = teteSommets; s; s = s->suivant) {
                if (s->nom == nom) {
                    existe = true;
                    break;
                }
            }
            if (!existe) {
                Sommet* s = new Sommet(nom, cap);
                s->suivant = teteSommets;
                teteSommets = s;
            }
        }
    }
    // Détecter sommet de départ et d'arrivée
    sommetDepart = "";
    sommetArrivee = "";
    for (Sommet* s = teteSommets; s; s = s->suivant) {
        if (s->nom == "Sv") sommetDepart = s->nom;
        if (s->nom == "Sd") sommetArrivee = s->nom;
    }
    return true;
}

void Fourmiliere::afficher() const {
    cout << endl << "Nombre de fourmis : " << nbFourmis << endl;
    cout << "Sommets :" << endl;
        for (Sommet* s = teteSommets; s; s = s->suivant) {
            cout << "  " << s->nom << " { " << s->capacite << " }" << endl;
        }
    cout << "Aretes :" << endl;
    for (Arete* a = teteAretes; a; a = a->suivant) {
        cout << "  " << a->depart << " - " << a->arrivee << endl;
    }
}

int Fourmiliere::getCapacite(const string &nom) const {
    // Sv et Sd = capacité illimitée
    if (nom == sommetDepart || nom == sommetArrivee) {
        return INT_MAX;
    }
    for (Sommet* s = teteSommets; s; s = s->suivant) {
        if (s->nom == nom) return s->capacite;
    }
    return 1; // par défaut
}