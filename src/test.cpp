#include <string>
#include <vector>
#include "SimParticle.h"

extern std::vector<std::vector<particle *>> theGrid;

void testMove()
{
    // loop over grid
    for (int krow = theGrid.size(); krow >= 0; krow--)
        for (particle *p : theGrid[krow])
        {
            // if particle present, move it
            if (p)
                p->move();
        }
}

bool particle::test()
{
    theGrid.resize(10, std::vector<particle *>(10));

    theGrid[9][1] = new grain(9, 1);
    testMove();

    if (!theGrid[9][1]->isAtRest())
        return false;

    theGrid[8][1] = new grain(8, 1);
    testMove();

    // check that the particle moved right aside and down
    if (theGrid[8][1] != NULL)
        return false;
    if (theGrid[9][2] == NULL)
        return false;

    testMove();
    if (theGrid[9][2] == NULL)
        return false;

    theGrid[8][1] = new grain(8, 1);
    testMove();
    testMove();
    testMove();

    // check that the particle moved left aside and down
    if (!theGrid[9][0]->isAtRest())
        return false;

    theGrid[8][1] = new grain(8, 1);
    testMove();

    // check that the particle is blocked
    if (!theGrid[8][1]->isAtRest())
        return false;

    theGrid.clear();
    return true;
}