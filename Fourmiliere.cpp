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
#include <unordered_set>
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

bool Fourmiliere::chargerDepuisFichier(const std::string &nomFichier) {
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

// == BFS depuis un sommet explicite ==================

std::vector<std::string> Fourmiliere::trouverChemin(
    const std::string& debut,
    const std::string& fin,
    const std::unordered_set<std::string>& sallesBloquees)
{
    if (debut == fin) return { debut };

    std::unordered_map<std::string, std::string> parent;
    std::deque<std::string> file;
    parent[debut] = "";
    file.push_back(debut);

    while (!file.empty()) {
        std::string cur = file.front(); file.pop_front();
        for (const std::string& v : voisins(cur)) {
            if (parent.count(v)) continue;
            // Salle bloquee, sauf si c'est la destination
            if (sallesBloquees.count(v) && v != fin) continue;
            parent[v] = cur;
            if (v == fin) {
                std::vector<std::string> chemin;
                for (std::string c = fin; !c.empty(); c = parent[c])
                    chemin.push_back(c);
                std::reverse(chemin.begin(), chemin.end());
                return chemin;
            }
            file.push_back(v);
        }
    }
    return {};
}

// == Surcharge sans arguments (chemin initial global) ==================

std::vector<std::string> Fourmiliere::trouverChemin(
    const std::unordered_set<std::string>& sallesBloquees)
{
    return trouverChemin(sommetDepart, sommetArrivee, sallesBloquees);
}

// == Simulation ========

void Fourmiliere::resoudre() {
    std::vector<std::string> cheminInitial = trouverChemin({});
    if (cheminInitial.empty()) {
        std::cout << "Aucun chemin trouvé.\n";
        return;
    }

    std::cout << "Chemin initial : ";
    for (size_t i = 0; i < cheminInitial.size(); i++) {
        if (i) std::cout << " -> ";
        std::cout << cheminInitial[i];
    }
    std::cout << " (" << cheminInitial.size() - 1 << " etapes)\n\n";

    std::unordered_map<std::string, int> posRef;
    for (int i = 0; i < (int)cheminInitial.size(); i++)
        posRef[cheminInitial[i]] = i;

    for (int i = 0; i < nbFourmis; i++)
        fourmis[i] = Ants(i + 1, sommetDepart, false);

    int  etape = 0;
    bool toutesArrivees = false;

    while (!toutesArrivees) {
        etape++;
        std::cout << "=== Etape " << etape << " ===\n";

        for (int i = 0; i < nbFourmis; i++)
            fourmis[i].bouger = false;

        // Trier les fourmis : celles les plus avancées bougent en premier
        std::vector<int> ordre;
        for (int i = 0; i < nbFourmis; i++)
            if (!fourmis[i].arrivee) ordre.push_back(i);

        std::sort(ordre.begin(), ordre.end(), [&](int a, int b) {
            int pa = posRef.count(fourmis[a].salle) ? posRef[fourmis[a].salle] : 0;
            int pb = posRef.count(fourmis[b].salle) ? posRef[fourmis[b].salle] : 0;
            return pa > pb;
        });

        bool auMoinsUnMouvement = false;

        for (int i : ordre) {
            Ants& f = fourmis[i];

            if (f.salle == sommetArrivee) { f.arrivee = true; continue; }

            // Construire les salles bloquees :
            // Une salle est bloquee si elle est saturee par des fourmis
            // qui n'ont PAS encore bouge cette etape.
            // Les fourmis ayant deja bouge (bouger==true) ont libere leur ancienne salle.
            std::unordered_map<std::string, int> occupationCourante;
            for (const Ants& a : fourmis) {
                if (a.arrivee) continue;
                if (a.bouger)  continue; // a deja bouge, sa salle est liberee
                occupationCourante[a.salle]++;
            }

            std::unordered_set<std::string> sallesBloquees;
            for (const auto& [salle, occ] : occupationCourante) {
                if (salle == f.salle)       continue; // propre salle toujours accessible
                if (salle == sommetDepart)  continue; // depart toujours accessible
                if (salle == sommetArrivee) continue; // arrivee toujours accessible
                if (occ >= capacite(salle))
                    sallesBloquees.insert(salle);
            }

            // BFS depuis la salle courante de la fourmi (sans muter l'etat global)
            std::vector<std::string> cheminFourmi =
                trouverChemin(f.salle, sommetArrivee, sallesBloquees);

            if (cheminFourmi.size() < 2) {
                f.bouger = false;
                continue;
            }

            std::string prochaine = cheminFourmi[1];

            // Verifier la capacite reelle de la prochaine salle
            // (compte les fourmis deja arrivees sur cette salle ce tour)
            int occup = 0;
            for (const Ants& a : fourmis)
                if (!a.arrivee && a.salle == prochaine) occup++;

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
            std::cout << "  [blocage detecte -- simulation arretee]\n";
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