#include "Ants.hpp"

Ants::Ants(int id, string salle, bool bouger) {
    this->id    = id;
    this->salle = salle;
    this->bouger = bouger;
    this->arrivee = false;
}