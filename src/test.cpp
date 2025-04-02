#include <string>
#include <vector>
#include "SimParticle.h"

bool particle::test()
{
    particle::setGridSize(
        GRID_ROW_COUNT, GRID_COL_COUNT);

    theGrid[GRID_ROW_COUNT-1][1] = new grain(GRID_ROW_COUNT-1, 1);

    moveAll();

    if (!theGrid[GRID_ROW_COUNT-1][1]->isAtRest())
        return false;

    theGrid[GRID_ROW_COUNT-2][1] = new grain(GRID_ROW_COUNT-2, 1);
    moveAll();

    // check that the particle moved right aside and down
    if (theGrid[8][1] != NULL)
        return false;
    if (theGrid[GRID_ROW_COUNT-1][2] == NULL)
        return false;

        moveAll();
    if (theGrid[GRID_ROW_COUNT-1][2] == NULL)
        return false;

    theGrid[GRID_ROW_COUNT-2][1] = new grain(GRID_ROW_COUNT-2, 1);
    moveAll();
    moveAll();
    moveAll();

    // check that the particle moved left aside and down
    if (!theGrid[GRID_ROW_COUNT-1][0]->isAtRest())
        return false;

    theGrid[GRID_ROW_COUNT-2][1] = new grain(GRID_ROW_COUNT-2, 1);
    moveAll();

    // check that the particle is blocked
    if (!theGrid[GRID_ROW_COUNT-2][1]->isAtRest())
        return false;

    theGrid.clear();
    return true;
}