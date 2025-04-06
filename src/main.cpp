#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include "cGUI.h"
#include "SimParticle.h"

#define LROW myLocation.first
#define LCOL myLocation.second

// storage for particle static attributes
grid_t particle::theGrid;
double particle::myGrid2WindowScale;
bool particle::myfMove;

particle::particle()
    : myColor(0x0000FF)
{
}
particle::particle(int color, int row, int col)
    : myColor(color),
      myLocation(std::make_pair(row, col)),
      fAtRest(false),
      myfReplacedWater(false)
{
    theGrid[row][col] = this;
    // std::cout << "created at "<< LROW <<" "<< LCOL <<" "<< row <<" "<< col << "\n";
}
particle *particle::factory(
    const wex::sMouse &m,
    int keydown)
{
    // particle generated only if left mouse button down
    if (!m.left)
        return NULL;

    /* convert mouse cursor position to grid location

    Note that x => row and y => col

    This is strange.
    It seems that there is a mixup somewhere between rows and cols,
    but I cannot find it.

    If this is NOT done bad things happen
    - the particle does NOT appear under the cursor
    - holding mouse button down creates just one particle, need to move cursor slightly
    - particles fall sideways
    */

    int row = m.y / myGrid2WindowScale;
    int col = m.x / myGrid2WindowScale;

    // check within bounds
    // note cannot use isOutGrid method because this method is static
    if (0 > row || row >= theGrid[0].size() ||
        0 > col || col >= theGrid.size())
        return NULL;

    // check location is empty
    if (theGrid[row][col] != NULL)
    {
        // std::cout << "cell occupied " << row << " " << col << "\n";
        return NULL;
    }

    // std::cout << "cell free\n";

    // construct particle type selected by key last pressed
    particle *newParticle;
    switch (keydown)
    {

    case 71:
        newParticle = new grain(row, col);
        break;

    case 83:
        newParticle = new stone(row, col);
        break;

    case 87:
        newParticle = new water(row, col);
        break;

    default:
        return NULL;
    }

    return newParticle;
}

void particle::setGridSize()
{
    theGrid.resize(GRID_ROW_COUNT, std::vector<particle *>(GRID_COL_COUNT));
    myGrid2WindowScale = (double)GRID_PXL_WIDTH / GRID_COL_COUNT;
}

bool particle::isOutGrid(int row, int col) const
{
    return (0 > col || col >= theGrid[0].size() ||
            0 > row || row >= theGrid.size());
}

particle *particle::get(int row, int col) const
{
    if (isOutGrid(row, col))
        return NULL;
    return theGrid[row][col];
}

void particle::setAtRest(bool f)
{
    fAtRest = f;
}
bool particle::isAtRest() const
{
    return fAtRest;
}

void particle::clearMoveFlags()
{
    myfMove = false;
    for (auto &row : theGrid)
        for (particle *p : row)
            if (p)
                p->myfMoveThis = false;
}

void particle::setMoveFlags()
{
    myfMove = true;     // request display refresh
    myfMoveThis = true; // prevent multiple moves ( teleporting )
}

void particle::moveDown()
{
    // std::cout << "down " << LROW << " " << LCOL;
    theGrid[LROW][LCOL] = NULL;
    theGrid[LROW + 1][LCOL] = this;
    LROW++;
    setMoveFlags();
    freeGrainsAbove();
    // std::cout << " > " << LROW << " " << LCOL << "\n";
}
void particle::moveLeft()
{
    // std::cout << "left " << LROW << " " << LCOL;
    theGrid[LROW][LCOL] = NULL;
    theGrid[LROW][LCOL - 1] = this;
    LCOL--;
    setMoveFlags();
    freeGrainsAbove();
    // std::cout << " > " << LROW << " " << LCOL << "\n";
}
void particle::moveRight()
{
    // std::cout << "right " << LROW << " " << LCOL;
    theGrid[LROW][LCOL] = NULL;
    theGrid[LROW][LCOL + 1] = this;
    LCOL++;
    setMoveFlags();
    freeGrainsAbove();
    // std::cout << " > " << LROW << " " << LCOL << "\n";
}

void particle::freeGrainsAbove()
{
    // ensure grain above, if present, is free
    auto n = get(LROW - 1, LCOL);
    if (n != nullptr)
        n->setAtRest(false);

    // ensure grain above left, if present, is free
    n = get(LROW - 1, LCOL - 1);
    if (n != nullptr)
        n->setAtRest(false);

    // ensure grain above right, if present, is free
    n = get(LROW - 1, LCOL + 1);
    if (n != nullptr)
        n->setAtRest(false);
}

int particle::color() const
{
    return myColor;
}
std::pair<int, int> particle::location() const
{
    return myLocation;
}
std::string particle::text() const
{
    std::stringstream ss;
    ss << myLocation.first << " " << myLocation.second << ", ";
    return ss.str();
}

void particle::draw(wex::shapes &S) const
{
    S.color(myColor);
    S.fill();
    // std::cout << "draw " << LCOL <<" "<< LROW <<" "<<myGrid2WindowScale << "\n";
    S.rectangle(
        {(int)(LCOL * myGrid2WindowScale), (int)(LROW * myGrid2WindowScale),
         3, 3});
}

void particle::drawAll(wex::shapes &S)
{
    // loop over grid
    for (auto &row : theGrid)
        for (particle *p : row)
        {
            // if particle present, draw it
            if (p)
            {
                p->draw(S);
            }
        }
}

bool particle::moveAll()
{
    particle::clearMoveFlags();

    /* loop over grid

    The grid must be scanned upwards to prevent precipes forming
    https://github.com/JamesBremner/SimParticle/issues/1#issuecomment-2781424962
    */
    for (int krow = theGrid.size()-1; krow >= 0; krow--)
        for (particle *p : theGrid[krow])
        {
            // if particle present, move it
            if (p)
                p->move();
        }

    return myfMove;
}

grain::grain(int row, int col)
    : particle(0x00FFFF, row, col)
{
}
void grain::move()
{
    // move grain if free to fall downwards

    /* skip if grain is blocked

    Most grains will have found a final resting place
    Only the grains that are moving need to be processed
    This gives the simulation a significant performance boost
    */
    if (isAtRest())
        return;

    // skip if already moved in this update ( prevent 'teleporting')
    if (myfMoveThis)
        return;

    // check if grain is resting directly on the bottom
    // std::cout << "move  "<< LROW+1 <<" "<< GRID_ROW_COUNT << ", ";
    if (LROW >= GRID_ROW_COUNT - 1)
    {
        setAtRest();
        return;
    }

    // check if cell below is empty so grain can fall straight down
    if (theGrid[LROW + 1][LCOL] == NULL)
    {
        moveDown();
        return;
    }

    // check if cell below contains water,
    // so grain will continue sinking through water
    if (dynamic_cast<water *>(theGrid[LROW + 1][LCOL]))
    {
        auto prevLocation = myLocation;
        moveDown();

        if (myfReplacedWater)
        {
            // sinking through water
            // so replace water that was displaced
            auto w = new water(prevLocation.first, prevLocation.second);
            w->setAtRest();
            theGrid[prevLocation.first][prevLocation.second] = w;
        }

        // remember to replace the water
        myfReplacedWater = true;
        return;
    }

    if (
        LCOL + 1 < theGrid[0].size() &&
        theGrid[LROW][LCOL + 1] == NULL &&
        theGrid[LROW + 1][LCOL + 1] == NULL)
    {
        // cells on right and down right are available
        // move right

        moveRight();
        return;
    }

    if (
        LCOL - 1 >= 0 &&
        theGrid[LROW][LCOL - 1] == NULL &&
        theGrid[LROW + 1][LCOL - 1] == NULL)
    {
        // cells on left and down left are available
        // move left
        moveLeft();
        return;
    }

    // grain is blocked
    setAtRest();
}

std::string grain::text() const
{
    std::string ret(" grain ");
    return ret + particle::text();
}

water::water(int x, int y)
    : particle(0xFDFB73, x, y)
{
}
void water::move()
{
    // skip if blocked
    if (isAtRest())
        return;

    // skip if already moved once
    if (myfMoveThis)
        return;

    // check if resting directly on the bottom
    if (LROW >= GRID_ROW_COUNT - 1)
    {
        setAtRest();
        return;
    }

    // check if there is empty cell directly below
    if (theGrid[LROW + 1][LCOL] == NULL)
    {
        moveDown();
        return;
    }

    // flow left and right until water finds its own level
    // search for nearest empty spot in either direction
    int flowDistance;
    bool flowLeft = true;
    bool flowRight = true;
    for (flowDistance = 1; flowDistance < GRID_COL_COUNT; flowDistance++)
    {
        // std::cout << (int)flowLeft << " " << (int)flowRight << " " << flowDistance << " > ";

        // check for blocked on both sides
        if ((!flowLeft) && (!flowRight))
            break;

        // check if still spreading on left
        if (flowLeft)
        {
            // check for beyond grid
            if (LCOL - flowDistance < 0)
            {
                // std::cout << "left bound\n";
                flowLeft = false;
            }
            else
            {
                particle *part = get(LROW + 1, LCOL - flowDistance);
                if (part == NULL)
                {
                    // found a spot
                    flowDistance *= -1;
                    break;
                }
                else
                {
                    if (!dynamic_cast<water *>(part))
                    {
                        // reached a water barrier
                        flowLeft = false;
                    }
                }
            }
        }

        // check if still spreading on right
        if (flowRight)
        {
            // check for beyond grid
            if (LCOL + flowDistance >= GRID_COL_COUNT)
            {
                flowRight = false;
            }
            else
            {
                // check for empty spot at pool limit on right
                particle *part = get(LROW + 1, LCOL + flowDistance);
                if (part == NULL)
                {
                    // found a spot
                    break;
                }
                else
                {
                    if (!dynamic_cast<water *>(part))
                    {
                        // reached a water barrier
                        flowRight = false;
                    }
                }
            }
        }
    }

    // check if further spread blocked on both sides
    if ((!flowLeft) && (!flowRight))
    {
        // start a new water level where water fell
        fAtRest = true;
        return;
    }

    if (!flowRight)
    {
        // the nearest empty spot on left
        flowDistance *= -1;
    }

    // found a spot
    theGrid[LROW+1][LCOL + flowDistance] = this;
    theGrid[LROW][LCOL] = NULL;
    LROW++;
    LCOL += flowDistance;
    setMoveFlags();
    // std::cout << "distance " << flowDistance
    //           << " water moves to " << LROW << " " << LCOL << "\n";
}

std::string water::text() const
{
    std::string ret(" water ");
    return ret + particle::text();
}

stone::stone(int row, int col)
    : particle(0x000000, row, col)
{
}
void stone::move()
{
    myLocation.first--;
}
std::string stone::text() const
{
    std::string ret(" stone ");
    return ret + particle::text();
}

main()
{
    // run the unit tests
    // window appears if tests all passed
    // app exits and message on console id test fails
    if (!particle::test())
    {
        std::cout << "unit test failed\n";
        exit(1);
    }

    // construct the user interface window
    cGUI theGUI;

    return 0;
}
