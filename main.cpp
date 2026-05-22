#include "Fourmiliere.hpp"
#include <iostream>
#include "ResolutionBFS.hpp"
#include "BidirectionalBFS.hpp"
using namespace std;

int main(int argc, char* argv[]) {
    Fourmiliere f;
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <fichier_fourmiliere.txt>" << std::endl;
        return 1;
    }
    if (f.chargerDepuisFichier("fourmilieres/" + std::string(argv[1]))) {
        f.afficher();
    } else {
        std::cerr << "Erreur lors du chargement du fichier." << std::endl;
    }

    cout << "==============================\n";
    cout << "      MENU DE RESOLUTION      " << endl;
    cout << "==============================\n";
    cout << "Veuillez choisir un algorithme :\n";
    cout << "  1. Resolution par BFS" << endl;
    cout << "  2. Resolution par DFS" << endl;
    cout << "  3. Combinaison BFS/DFS" << endl;
    cout << "------------------------------" << endl;

    int choix;
    bool choixValide = false;

    while (!choixValide) {
        cout << "Votre choix : ";
        cin >> choix;
        cout << "------------------------------" << endl;
        switch (choix) {
            case 1:
                choixValide = true;
                cout << "\n[INFO] Resolution par BFS selectionnee.\n";
                ResolutionBFS(f);
                break;
            case 2:
                choixValide = true;
                cout << "\n[INFO] Resolution par DFS selectionnee.\n";
                // f.resoudreDFS();
                cout << "DFS non implémenté." << endl;
                break;
            case 3:
                choixValide = true;
                cout << "\n[INFO] RResolution par BFS Bidirectionnel selectionnee.\n";
                ResolutionBidirectionalBFS(f);
                //cout << "Combinaison non implémentée." << endl;
                break;
            default:
                cout << "\n[ERREUR] Choix invalide. Veuillez relancer le programme et choisir une option valide." << endl;
        }
    }
    
    return 0;
}