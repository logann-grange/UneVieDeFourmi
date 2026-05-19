#ifndef FOURMILIERE_HPP
#define FOURMILIERE_HPP
#include "Ants.cpp"
#include <vector>
#include <string>

struct Sommet {
    std::string nom;
    int capacite;
    Sommet* suivant;
    Sommet(const std::string& n, int c = -1) : nom(n), capacite(c), suivant(nullptr) {}
};

struct Arete {
    std::string depart;
    std::string arrivee;
    Arete* suivant;
    Arete(const std::string& d, const std::string& a) : depart(d), arrivee(a), suivant(nullptr) {}
};

class Fourmiliere {
public:
    int nbFourmis;
    std::string sommetDepart;
    std::string sommetArrivee;
    Sommet* teteSommets;
    Arete* teteAretes;
    std::vector<Ants> fourmis;

    Fourmiliere();
    ~Fourmiliere();
    void resoudre();
    bool chargerDepuisFichier(const std::string& nomFichier);
    void afficher() const;
    void afficherEtapes() const;
};

#endif // FOURMILIERE_HPP
