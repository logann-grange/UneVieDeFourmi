#include "Fourmiliere.hpp"
#include <iostream>
#include "BidirectionalBFS.hpp"
using namespace std;

int main() {
    Fourmiliere f;
    if (f.chargerDepuisFichier("fourmilieres/fourmiliere_deux.txt")) {
        f.afficher();
        std::cout << "\n";
        ResolutionBidirectionalBFS(f);
    } else {
        std::cerr << "Erreur lors du chargement du fichier.\n";
    }
    return 0;
}