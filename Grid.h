#ifndef GRID_H
#define GRID_H

#include "Cell.h"
#include <vector>

class Grid {
public:
    static const int GRID_SIZE = 8;
    std::vector<std::vector<Cell>> grid;

    Grid();
    void placeDevice();
    void placeEnergySource();
    void draw(sf::RenderWindow& window);
};

#endif // GRID_H
