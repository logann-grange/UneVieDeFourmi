#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;


class Ants {
    private:
    bool bouger;
    std::vector<std::string>cheminParcouru;
    int tempsDepart;

public:
    Ants( std::string salle="", bool bouger=false);    
};