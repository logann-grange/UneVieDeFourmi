#include "Fourmiliere.hpp"
#include <iostream>
#include "ResolutionBFS.hpp"

int main(int argc, char* argv[]) {
    Fourmiliere f;
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <fichier_fourmiliere>" << std::endl;
        return 1;
    }
    if (f.chargerDepuisFichier("formilliere/" + std::string(argv[1]))) {
        f.afficher();
        ResolutionBFS(f);
    } else {
        std::cerr << "Erreur lors du chargement du fichier." << std::endl;
    }
    return 0;
}
