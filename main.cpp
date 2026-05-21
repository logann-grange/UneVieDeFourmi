#include "Fourmiliere.hpp"
#include <iostream>

int main() {
    Fourmiliere f;
    if (f.chargerDepuisFichier("fourmilieres/salle_d_at-ant.txt")) {
        f.afficher();
        std::cout << "\n";
        f.resoudre();
    } else {
        std::cerr << "Erreur lors du chargement du fichier.\n";
    }
    return 0;
}