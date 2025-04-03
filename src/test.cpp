#include <string>
#include <vector>
#include "SimParticle.h"

bool particle::testGrainMove()
{
    particle::setGridSize(
        GRID_ROW_COUNT, GRID_COL_COUNT);
    const int rowBottom = GRID_ROW_COUNT - 1;
    const int row1Up = GRID_ROW_COUNT - 2;

    theGrid[GRID_ROW_COUNT - 1][1] = new grain(GRID_ROW_COUNT - 1, 1);

    moveAll();

    if (!theGrid[GRID_ROW_COUNT - 1][1]->isAtRest())
        return false;

    particle::clearMoveFlags();
    theGrid[row1Up][1] = new grain(row1Up, 1);
    moveAll();

    // check that the particle moved right aside
    if (theGrid[row1Up][1] != NULL)
        return false;
    if (theGrid[row1Up][2] == NULL)
        return false;

    // and then down
    moveAll();
    if (theGrid[rowBottom][2] == NULL)
        return false;

    particle::clearMoveFlags();
    theGrid[GRID_ROW_COUNT - 2][1] = new grain(GRID_ROW_COUNT - 2, 1);
    moveAll();
    moveAll();
    moveAll();

    // check that the particle moved left aside and down
    if (!theGrid[GRID_ROW_COUNT - 1][0]->isAtRest())
        return false;

    theGrid[GRID_ROW_COUNT - 2][1] = new grain(GRID_ROW_COUNT - 2, 1);
    moveAll();

    // check that the particle is blocked
    if (!theGrid[GRID_ROW_COUNT - 2][1]->isAtRest())
        return false;

    theGrid.clear();
    return true;
}

bool particle::testWaterMove()
{
    particle::setGridSize(
        GRID_ROW_COUNT, GRID_COL_COUNT);
    const int rowBottom = GRID_ROW_COUNT - 1;
    const int row1Up = GRID_ROW_COUNT - 2;

    theGrid[rowBottom][1] = new water(rowBottom, 1);

    moveAll();

    if (!theGrid[rowBottom][1]->isAtRest())
        return false;

    theGrid[row1Up][1] = new water(row1Up, 1);
    moveAll();

    if (theGrid[row1Up][1] != NULL)
        return false;
    if (theGrid[row1Up][0] == NULL)
        return false;

    moveAll();
    moveAll();
    if (theGrid[rowBottom][0] == NULL)
        return false;    

    theGrid[row1Up][1] = new water(row1Up, 1);
    moveAll();

    if (theGrid[row1Up][1] != NULL)
        return false;
    if (theGrid[row1Up][2] == NULL)
        return false;

    theGrid.clear();
    return true;
}

bool particle::test()
{
    if (!testGrainMove())
        return false;
    if (!testWaterMove())
        return false;

    return true;
}