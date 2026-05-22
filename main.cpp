#include "Fourmiliere.hpp"
#include <iostream>
#include "ResolutionBFS.hpp"
using namespace std;

int main(int argc, char* argv[]) {
    Fourmiliere f;
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <fichier_fourmiliere>" << std::endl;
        return 1;
    }
    if (f.chargerDepuisFichier("formilliere/" + std::string(argv[1]))) {
        f.afficher();
    } else {
        std::cerr << "Erreur lors du chargement du fichier." << std::endl;
    }

    cout << "==============================\n";
    cout << "      MENU DE RÉSOLUTION      " << endl;
    cout << "==============================\n";
    cout << "Veuillez choisir un algorithme :\n";
    cout << "  1. Résolution par BFS" << endl;
    cout << "  2. Résolution par DFS" << endl;
    cout << "  3. Combinaison BFS/DFS" << endl;
    cout << "------------------------------" << endl;
    cout << "Votre choix : ";
    int choix;
    cin >> choix;
    cout << "------------------------------" << endl;
    switch (choix) {
        case 1:
            cout << "\n[INFO] Résolution par BFS sélectionnée.\n";
            ResolutionBFS(f);
            break;
        case 2:
            cout << "\n[INFO] Résolution par DFS sélectionnée.\n";
            // f.resoudreDFS();
            cout << "DFS non implémenté." << endl;
            break;
        case 3:
            cout << "\n[INFO] Résolution par combinaison BFS/DFS sélectionnée.\n";
            // f.resoudreCombinaison();
            cout << "Combinaison non implémentée." << endl;
            break;
        default:
            cout << "\n[ERREUR] Choix invalide. Veuillez relancer le programme et choisir une option valide." << endl;
    }
    return 0;
}
