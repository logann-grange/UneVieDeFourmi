#include "../include/Ants.hpp"

Ants::Ants(std::string salle, bool bouger) {
    this->cheminParcouru.push_back(salle);
    this->bouger = bouger;
}