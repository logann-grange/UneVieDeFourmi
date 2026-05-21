#include "Fourmiliere.hpp"
#include <iostream>
#include "ResolutionBFS.hpp"

int main() {
    Fourmiliere f;
    if (f.chargerDepuisFichier("formilliere/La_hormiguera_de_la_muerte.txt")) {
        f.afficher();
        ResolutionBFS(f);
    } else {
        std::cerr << "Erreur lors du chargement du fichier." << std::endl;
    }
    return 0;
}
