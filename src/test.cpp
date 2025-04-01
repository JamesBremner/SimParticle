#include <string>
#include <vector>
#include "SimParticle.h"

extern std::vector<std::vector<particle *>> theGrid;

bool particle::test()
{
    theGrid.resize(10, std::vector<particle *>(10));
    theGrid[1][1] = new grain(1, 1);
    theGrid[1][1]->move();

    // check that the particle fell straight down
    if (theGrid[1][1] != NULL)
        return false;
    if (theGrid[2][1] == NULL)
        return false;

    theGrid[1][1] = new grain(1, 1);
    theGrid[1][1]->move();

    // check that the particle moved aside
    if (theGrid[1][1] != NULL)
        return false;
    if (theGrid[1][2] == NULL)
        return false;

    theGrid.clear();
    return true;
}