#include "../include/Fourmiliere.hpp"
#include "../include/ResolutionBFS.hpp"
#include "../include/BidirectionalBFS.hpp"
#include "../include/ResolutionAStar.hpp" 
#include "../include/Benchmark.hpp"
#include "../include/Affichage.hpp"
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    Fourmiliere f;
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <fichier_fourmiliere.txt>" << std::endl;
        return 1;
    }
    if (f.chargerDepuisFichier("fourmilieres/" + std::string(argv[1])))
    {
        f.afficher();
    }
    else
    {
        std::cerr << "Erreur lors du chargement du fichier." << std::endl;
    }

    afficherMenu();

    int choix;
    bool choixValide = false;

    while (!choixValide)
    {
        cout << "Votre choix : ";
        cin >> choix;
        cout << "------------------------------" << endl;
        switch (choix)
        {
        case 1:
            choixValide = true;
            cout << "\n[INFO] Resolution par BFS selectionnee.\n";
            cout << "[TRACE] Avant appel ResolutionBFS" << endl;
            ResolutionBFS(f);
            cout << "[TRACE] Apres appel ResolutionBFS" << endl;
            break;
        case 2:
            choixValide = true;
            cout << "\n[INFO] RResolution par BFS Bidirectionnel selectionnee.\n";
            ResolutionBidirectionalBFS(f);
            break;
        case 3: // <-- NOUVEAU CASE POUR A*
            choixValide = true;
            cout << "\n[INFO] Resolution par A* selectionnee.\n";
            ResolutionAStar(f);
            break;
        case 4:
            choixValide = true;
            cout << "\n[INFO] Benchmark des algorithmes selectionne.\n";
            // Passe le nom du fichier à ResolutionBenchmark
            ResolutionBenchmark(f, "fourmilieres/" + std::string(argv[1]));
            break;    
        default:
            cout << "\n[ERREUR] Choix invalide. Veuillez relancer le programme et choisir une option valide." << endl;
        }
    }

    return 0;
}