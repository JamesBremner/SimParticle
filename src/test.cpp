#include <string>
#include <vector>
#include "SimParticle.h"

bool particle::test()
{
    particle::setGridSize(10,10);

    theGrid[9][1] = new grain(9, 1);

    moveAll();

    if (!theGrid[9][1]->isAtRest())
        return false;

    theGrid[8][1] = new grain(8, 1);
    moveAll();

    // check that the particle moved right aside and down
    if (theGrid[8][1] != NULL)
        return false;
    if (theGrid[9][2] == NULL)
        return false;

        moveAll();
    if (theGrid[9][2] == NULL)
        return false;

    theGrid[8][1] = new grain(8, 1);
    moveAll();
    moveAll();
    moveAll();

    // check that the particle moved left aside and down
    if (!theGrid[9][0]->isAtRest())
        return false;

    theGrid[8][1] = new grain(8, 1);
    moveAll();

    // check that the particle is blocked
    if (!theGrid[8][1]->isAtRest())
        return false;

    theGrid.clear();
    return true;
}