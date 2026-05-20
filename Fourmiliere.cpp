#include "Fourmiliere.hpp"
#include "Ants.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>
#include <vector>
#include <string>
#include <deque>
#include <unordered_map>
#include <algorithm>
#include <climits>

// == Chargement ====================

Fourmiliere::Fourmiliere() : nbFourmis(0), teteSommets(nullptr), teteAretes(nullptr) {}

Fourmiliere::~Fourmiliere() {
    while (teteSommets) {
        Sommet* tmp = teteSommets;
        teteSommets = teteSommets->suivant;
        delete tmp;
    }
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
            std::string d = match[1], a = match[2];
            Arete* ar = new Arete(d, a);
            ar->suivant = teteAretes;
            teteAretes = ar;
            for (const std::string& nomS : {d, a}) {
                bool existe = false;
                for (Sommet* s = teteSommets; s; s = s->suivant)
                    if (s->nom == nomS) { existe = true; break; }
                if (!existe) {
                    Sommet* s = new Sommet(nomS, 1);
                    s->suivant = teteSommets;
                    teteSommets = s;
                }
            }
        } else if (std::regex_search(ligne, match, regexSommet)) {
            std::string nom = match[1];
            int cap = match[2].matched ? std::stoi(match[2]) : 1;
            bool existe = false;
            for (Sommet* s = teteSommets; s; s = s->suivant)
                if (s->nom == nom) { existe = true; break; }
            if (!existe) {
                Sommet* s = new Sommet(nom, cap);
                s->suivant = teteSommets;
                teteSommets = s;
            }
        }
    }
    sommetDepart = sommetArrivee = "";
    for (Sommet* s = teteSommets; s; s = s->suivant) {
        if (s->nom == "Sv") sommetDepart  = s->nom;
        if (s->nom == "Sd") sommetArrivee = s->nom;
    }
    return true;
}

void Fourmiliere::afficher() const {
    std::cout << "Nombre de fourmis : " << nbFourmis << std::endl;
    std::cout << "Sommet de depart  : " << sommetDepart << std::endl;
    std::cout << "Sommet d'arrivee  : " << sommetArrivee << std::endl;
    std::cout << "Sommets :" << std::endl;
    for (Sommet* s = teteSommets; s; s = s->suivant)
        std::cout << "  " << s->nom << " { " << s->capacite << " }" << std::endl;
    std::cout << "Aretes :" << std::endl;
    for (Arete* a = teteAretes; a; a = a->suivant)
        std::cout << "  " << a->depart << " - " << a->arrivee << std::endl;
}

// == Helpers ===================

int Fourmiliere::capacite(const std::string &nom) const {
    if (nom == sommetDepart || nom == sommetArrivee) return INT_MAX;
    for (Sommet* s = teteSommets; s; s = s->suivant)
        if (s->nom == nom) return s->capacite;
    return 1;
}

int Fourmiliere::occupation(const std::string &nom) const {
    int count = 0;
    for (const Ants &a : fourmis)
        if (a.salle == nom && !a.arrivee) count++;
    return count;
}

std::vector<std::string> Fourmiliere::voisins(const std::string &nom) const {
    std::vector<std::string> v;
    for (Arete* a = teteAretes; a; a = a->suivant) {
        if (a->depart  == nom) v.push_back(a->arrivee);
        else if (a->arrivee == nom) v.push_back(a->depart);
    }
    return v;
}

// == Bidirectional BFS ==================

std::vector<std::string> Fourmiliere::trouverChemin() {
    if (sommetDepart.empty() || sommetArrivee.empty()) return {};
    if (sommetDepart == sommetArrivee) return { sommetDepart };

    std::unordered_map<std::string, std::string> parentFront, parentBack;
    std::deque<std::string> fileFront, fileBack;

    parentFront[sommetDepart] = "";
    parentBack[sommetArrivee] = "";
    fileFront.push_back(sommetDepart);
    fileBack.push_back(sommetArrivee);

    auto reconstruire = [&](const std::string& jonction) {
        std::vector<std::string> chemin;
        for (std::string c = jonction; !c.empty(); c = parentFront[c])
            chemin.push_back(c);
        std::reverse(chemin.begin(), chemin.end());
        std::string c = parentBack.count(jonction) ? parentBack[jonction] : "";
        while (!c.empty()) {
            chemin.push_back(c);
            c = parentBack.count(c) ? parentBack[c] : "";
        }
        return chemin;
    };

    while (!fileFront.empty() || !fileBack.empty()) {
        if (!fileFront.empty()) {
            std::string cur = fileFront.front(); fileFront.pop_front();
            for (const std::string& v : voisins(cur)) {
                if (!parentFront.count(v)) {
                    parentFront[v] = cur;
                    fileFront.push_back(v);
                }
                if (parentBack.count(v)) return reconstruire(v);
            }
        }
        if (!fileBack.empty()) {
            std::string cur = fileBack.front(); fileBack.pop_front();
            for (const std::string& v : voisins(cur)) {
                if (!parentBack.count(v)) {
                    parentBack[v] = cur;
                    fileBack.push_back(v);
                }
                if (parentFront.count(v)) return reconstruire(v);
            }
        }
    }
    return {};
}

// == Simulation ========

void Fourmiliere::resoudre() {
    std::vector<std::string> chemin = trouverChemin();
    if (chemin.empty()) {
        std::cout << "Aucun chemin trouvé.\n";
        return;
    }

    std::cout << "Chemin : ";
    for (size_t i = 0; i < chemin.size(); i++) {
        if (i) std::cout << " -> ";
        std::cout << chemin[i];
    }
    std::cout << " (" << chemin.size() - 1 << " etapes)\n\n";

    std::unordered_map<std::string, int> positionDansChemin;
    for (int i = 0; i < (int)chemin.size(); i++)
        positionDansChemin[chemin[i]] = i;

    for (int i = 0; i < nbFourmis; i++)
        fourmis[i] = Ants(i + 1, sommetDepart, false);

    int  etape = 0;
    bool toutesArrivees = false;

    while (!toutesArrivees) {
        etape++;
        std::cout << "=== Etape " << etape << " ===\n";

        // Indices des fourmis pas encore arrivées, triées les plus avancées d'abord
        std::vector<int> ordre;
        for (int i = 0; i < nbFourmis; i++)
            if (!fourmis[i].arrivee) ordre.push_back(i);

        std::sort(ordre.begin(), ordre.end(), [&](int a, int b) {
            return positionDansChemin[fourmis[a].salle]
                 > positionDansChemin[fourmis[b].salle];
        });

        bool auMoinsUnMouvement = false;

        for (int i : ordre) {
            Ants& f = fourmis[i];
            int pos = positionDansChemin[f.salle];

            if (pos == (int)chemin.size() - 1) {
                f.arrivee = true;
                continue;
            }

            std::string prochaine = chemin[pos + 1];

            int occup = 0;
            for (const Ants& a : fourmis)
                if (a.salle == prochaine && !a.arrivee) occup++;

            bool peutAvancer = (prochaine == sommetArrivee)
                             || (occup < capacite(prochaine));

            if (peutAvancer) {
                std::cout << "  Fourmi " << f.id
                          << " : " << f.salle << " -> " << prochaine << "\n";
                f.salle  = prochaine;
                f.bouger = true;
                if (prochaine == sommetArrivee) f.arrivee = true;
                auMoinsUnMouvement = true;
            } else {
                f.bouger = false;
            }
        }

        if (!auMoinsUnMouvement) {
            std::cout << "  [blocage détecté — simulation arrêtée]\n";
            break;
        }

        int nbArrivees = 0;
        for (const Ants& a : fourmis) if (a.arrivee) nbArrivees++;
        std::cout << "  -> " << nbArrivees << "/" << nbFourmis
                  << " fourmis arrivees\n\n";

        toutesArrivees = (nbArrivees == nbFourmis);
    }

    std::cout << "Termine en " << etape << " etapes.\n\n";
}

void Fourmiliere::afficherEtapes() const {}