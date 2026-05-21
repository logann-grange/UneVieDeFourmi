#pragma once
#ifndef FOURMILIERE_HPP
#define FOURMILIERE_HPP
#include "Ants.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

struct Sommet {
    std::string nom;
    int capacite;
    Sommet* suivant;
    Sommet(const std::string &n, int c = 1)
        : nom(n), capacite(c), suivant(nullptr) {}
};

struct Arete {
    std::string depart;
    std::string arrivee;
    Arete* suivant;
    Arete(const std::string &d, const std::string &a)
        : depart(d), arrivee(a), suivant(nullptr) {}
};

class Fourmiliere {
public:
    // === Attributs ===
    int nbFourmis;
    std::string sommetDepart;
    std::string sommetArrivee;
    Sommet* teteSommets;
    Arete*  teteAretes;
    std::vector<Ants> fourmis;

    // === Constructeur / Destructeur ===
    Fourmiliere();
    ~Fourmiliere();

    // === Chargement / Affichage ===
    bool chargerDepuisFichier(const std::string &nomFichier);
    void afficher() const;
    void afficherEtapes() const;
    void resoudre();

    // === Recherche de chemin ===
    std::vector<std::string> trouverChemin(
        const std::string& debut,
        const std::string& fin,
        const std::unordered_set<std::string>& sallesBloquees = {}
    );
    std::vector<std::string> trouverChemin(
        const std::unordered_set<std::string>& sallesBloquees = {}
    );

    std::vector<std::vector<std::string>> trouverTousChemins();

    std::vector<std::string> trouverCheminResidual(
        const std::unordered_map<std::string, std::unordered_map<std::string, int>> &cap
    );

    // === Helpers ===
    std::vector<std::string> voisins(const std::string &nom) const;
    int capacite(const std::string &nom) const;
    int occupation(const std::string &nom) const;
};

#endif