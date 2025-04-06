#include <string>
#include <vector>
#include "SimParticle.h"

bool particle::testWaterFlow()
{
    // test that water flows far enough to find its own level

    particle::setGridSize();
    const int rowBottom = GRID_ROW_COUNT - 1;
    const int row1Up = GRID_ROW_COUNT - 2;

    // test water finds its own level

    theGrid.clear();
    particle::setGridSize();
    new water(rowBottom, 1);
    new water(rowBottom, 2);
    new water(rowBottom, 3);
    new water(row1Up, 2);
    moveAll();

    // check that water flowed to left of pool
    if (theGrid[rowBottom][0] == NULL)
        return false;

    new water(row1Up, 2);
    moveAll();

    // check that water flowed to right of pool
    if (theGrid[rowBottom][4] == NULL)
        return false;

    new water(row1Up, 2);
    moveAll();

    if (theGrid[rowBottom][5] == NULL)
        return false;

    theGrid.clear();
    return true;
}

bool particle::testGrainMove()
{
    particle::setGridSize();
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
    particle::setGridSize();
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
    if (theGrid[rowBottom][0] == NULL)
        return false;

    moveAll();
    moveAll();
    if (theGrid[rowBottom][0] == NULL)
        return false;

    theGrid[row1Up][1] = new water(row1Up, 1);
    moveAll();

    if (theGrid[row1Up][1] != NULL)
        return false;
    if (theGrid[rowBottom][2] == NULL)
        return false;

    // test water barrier
    theGrid.clear();
    particle::setGridSize();

    theGrid[rowBottom][1] = new water(rowBottom, 1);
    theGrid[row1Up][1] = new water(row1Up, 1);
    theGrid[rowBottom][0] = new grain(rowBottom, 0);
    theGrid[row1Up][0] = new grain(row1Up, 0);
    auto p = new grain(rowBottom, 4);
    p->setAtRest();
    p = new grain(row1Up, 4);
    p->setAtRest();
    moveAll();

    // check water barred from going left
    if (theGrid[rowBottom][2] == NULL)
        return false;

    theGrid.clear();
    return true;
}

bool particle::test()
{
    if (!testWaterFlow())
        return false;
    if (!testGrainMove())
        return false;
    if (!testWaterMove())
        return false;

    return true;
}