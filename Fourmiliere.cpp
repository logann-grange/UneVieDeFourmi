#include "Fourmiliere.hpp"
#include "Ants.cpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>

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

bool Fourmiliere::chargerDepuisFichier(const std::string& nomFichier) {
    std::ifstream fichier(nomFichier);
    if (!fichier) return false;
    std::string ligne;
    std::regex regexFourmis("^f=([0-9]+)");
        std::regex regexSommet("^(S[0-9]+|Sv|Sd)(?: \\{ *([0-9]+) *\\})?");
    std::regex regexArete("^(S[0-9]+|Sv|Sd) *- *(S[0-9]+|Sv|Sd)");
    std::smatch match;
    while (std::getline(fichier, ligne)) {
        if (std::regex_search(ligne, match, regexFourmis)) {
            nbFourmis = std::stoi(match[1]);
            fourmis.resize(nbFourmis);
        } else if (std::regex_search(ligne, match, regexArete)) {
            std::string d = match[1];
            std::string a = match[2];
            Arete* ar = new Arete(d, a);
            ar->suivant = teteAretes;
            teteAretes = ar;
            // Ajouter les sommets s'ils n'existent pas déjà (pour Sd/Sv non listés)
            for (const std::string& nomSommet : {d, a}) {
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
        } else if (std::regex_search(ligne, match, regexSommet)) {
            std::string nom = match[1];
            int cap = match[2].matched ? std::stoi(match[2]) : 1; // Par défaut, capacité = 1
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
    std::cout << "Nombre de fourmis : " << nbFourmis << std::endl;
    std::cout << "Sommet de départ : " << sommetDepart << std::endl;
    std::cout << "Sommet d'arrivée : " << sommetArrivee << std::endl;
    std::cout << "Sommets :" << std::endl;
        for (Sommet* s = teteSommets; s; s = s->suivant) {
            std::cout << "  " << s->nom << " { " << s->capacite << " }" << std::endl;
        }
    std::cout << "Arêtes :" << std::endl;
    for (Arete* a = teteAretes; a; a = a->suivant) {
        std::cout << "  " << a->depart << " - " << a->arrivee << std::endl;
    }
}

void Fourmiliere::afficherEtapes() const {
    std::cout << "Étapes de déplacement des fourmis :" << std::endl;
    for (size_t i = 0; i < fourmis.size(); ++i) {
        std::cout << "Fourmi " << i + 1 << " : " << fourmis[i].salle << std::endl;
    }
}

struct Fourmis{
    string salle;
    bool bouger;
}