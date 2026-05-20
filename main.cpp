#include "Fourmiliere.hpp"
#include <iostream>

int main() {
    Fourmiliere f;
    if (f.chargerDepuisFichier("formilliere/fourmiliere_cinq.txt")) {
        f.afficher();
        f.resoudreBFS();
    } else {
        std::cerr << "Erreur lors du chargement du fichier." << std::endl;
    }
    return 0;
}
