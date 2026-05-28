#include "../include/Affichage.hpp"

void afficherEtapes(Fourmiliere fourmiliere, std::vector<std::vector<std::string>> cheminsFourmis, std::vector<int> tempsDepart, int tourMax)
{
    for (int t = 1; t <= tourMax; ++t)
    {
        cout << "========== Tour " << t << " ========== " << endl;
        for (int i = 0; i < fourmiliere.nbFourmis; ++i)
        {
            if (cheminsFourmis[i].empty())
                continue;

            if (t == 0)
            {
                cout << "F" << i + 1 << " -> " << fourmiliere.sommetDepart << endl;
                continue;
            }

            int idx = t - tempsDepart[i];
            if (idx > 0 && (size_t)idx < cheminsFourmis[i].size())
            {
                cout << "F" << i + 1 << " -> " << cheminsFourmis[i][idx] << endl;
            }
        }
        cout << "\n";
    }
}

void afficherMenu()
{
    cout << endl;
    cout << "==============================\n";
    cout << "      MENU DE RESOLUTION      " << endl;
    cout << "==============================\n";
    cout << "Veuillez choisir un algorithme :\n";
    cout << "  1. Resolution par BFS" << endl;
    cout << "  2. Combinaison BFS/DFS" << endl;
    cout << "  3. Combinaison par A*" << endl;
    cout << "------------------------------" << endl;
}