#pragma once
#include <string>
using namespace std;

class Ants {
public:
    int    id;
    string salle;
    bool   bouger;
    bool   arrivee;

    Ants(int id = 0, string salle = "", bool bouger = false);
};