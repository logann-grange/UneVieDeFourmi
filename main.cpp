#include "Fourmiliere.hpp"
#include <iostream>

int main() {
    Fourmiliere f;
    if (f.chargerDepuisFichier("formilliere/fourmiliere_un.txt")) {
        f.afficher();
    } else {
        std::cerr << "Erreur lors du chargement du fichier." << std::endl;
    }
    return 0;
}
