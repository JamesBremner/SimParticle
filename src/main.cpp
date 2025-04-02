#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include "cStarterGUI.h"
#include "SimParticle.h"

#define LX myLocation.second
#define LY myLocation.first

// storage for particle static attributes
grid_t particle::theGrid;
double particle::myGrid2WindowScale;

class cGUI : public cStarterGUI
{
public:
    cGUI();

private:
    wex::timer *myUpdateTimer;
    wex::timer *myMoveTimer;

    int myKeyDown;

    void constructTimers();
    void registerEventHandlers();
};

particle::particle()
    : myColor(0x0000FF)
{
}
particle::particle(int color, int x, int y)
    : myColor(color),
      myLocation(std::make_pair(x, y)),
      fAtRest(false)
{
}
particle *particle::factory(
    const wex::sMouse &m,
    int keydown)
{
    // particle generated only if left mouse button down
    if (!m.left)
        return NULL;

    //std::cout << "keydown " << keydown << "\n";

    // grid location of mouse cursor
    const double scale = (double)GRID_COL_COUNT / GRID_PXL_WIDTH;
    int ix = m.y * scale;
    int iy = m.x * scale;

    // check within bounds
    // note cannot use isOutGrid method because this method is static
    if (0 > ix || ix >= theGrid[0].size() ||
            0 > iy || iy >= theGrid.size())
        return NULL;

    // check location is empty
    if (theGrid[iy][ix] != NULL)
        return NULL;

    // construct particle type selected by key last pressed
    particle *newParticle;
    switch (keydown)
    {

    case 71:
        newParticle = new grain(ix, iy);
        break;

    case 83:
        newParticle = new stone(ix, iy);
        break;

    case 87:
        newParticle = new water(ix, iy);
        break;

    default:
        return NULL;
    }

    // place particle in grid
    theGrid[iy][ix] = newParticle;

    std::cout << "created at " << newParticle->text() << " ";

    return newParticle;
}

void particle::setGridSize(
    int rowCount,
    int colCount)
{
    theGrid.resize(rowCount, std::vector<particle *>(colCount));
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

void particle::freeGrainsAbove(const std::pair<int, int> &location)
{
    // free grains that may have been blocked;

    // check if grain is on window bottom - always blocked
    if (location.second - 1 < 0)
    {
        return;
    }

    // ensure grain above, if present, is free
    auto n = get(LY - 1, LX);
    if (n != nullptr)
        n->setAtRest(false);

    // ensure grain above left, if present, is free
    n = get(LY - 1,LX - 1);
    if (n != nullptr)
        n->setAtRest(false);

    // ensure grain above right, if present, is free
    // check for right boundary ( fix TID20 )
    n = get(LY - 1,LX + 1);
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
    //std::cout << "draw " << LX <<" "<< LY <<" "<<myGrid2WindowScale << ", ";
    S.rectangle(
        {(int)(LX * myGrid2WindowScale), (int)(LY * myGrid2WindowScale),
                 3, 3});
}

grain::grain(int x, int y)
    : particle(0x00FFFF, x, y)
{
    myLocation = std::make_pair(x, y);
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

    // check if grain is resting directly on the bottom
    //std::cout << "move  "<< LY+1 <<" "<< GRID_ROW_COUNT << ", ";
    if (LY >= GRID_ROW_COUNT - 1 )
    {
        setAtRest();
        return;
    }

    // try moving grain down
    auto prevLocation = myLocation;
    bool fMoved = false;
    if (theGrid[LY + 1][LX] == NULL)
    {
        // cell below is empty so grain can fall straight down

        theGrid[LY + 1][LX] = this;
        theGrid[LY][LX] = NULL;
        LY++;
        fMoved = true;
    }
    else if (
        LX + 1 < theGrid[0].size() &&
        theGrid[LY][LX + 1] == NULL &&
        theGrid[LY + 1][LX + 1] == NULL)
    {
        // cells on right and down right are available
        // move right
        theGrid[LY][LX + 1] = this;
        theGrid[LY][LX] = NULL;
        LX++;
        fMoved = true;
    }
    else if (
        LX - 1 >= 0 &&
        theGrid[LY][LX - 1] == NULL &&
        theGrid[LY + 1][LX - 1] == NULL)
    {
        // cells on left and down left are available
        // move left
        theGrid[LY][LX - 1] = this;
        theGrid[LY][LX] = NULL;
        LX--;
        fMoved = true;
    }

    if (fMoved)
    {
        // free grains that may have been blocked;
        freeGrainsAbove(prevLocation);

        // std::cout << " moved " << text() << " ";
    }
    else
    {
        // grain is blocked
        setAtRest();
    }
}

std::string grain::text() const
{
    std::string ret(" grain ");
    return ret + particle::text();
}

water::water(int x, int y)
    : particle(0xFF0000, x, y)
{
}
void water::move()
{
    myLocation.first++;
}
std::string water::text() const
{
    std::string ret(" water ");
    return ret + particle::text();
}

stone::stone(int x, int y)
    : particle(0x000000, x, y)
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

cGUI::cGUI()
    : cStarterGUI(
          "Particle Simulator",
          {50, 50, GRID_PXL_WIDTH, GRID_PXL_HEIGHT+50})
{
    particle::setGridSize(
        GRID_ROW_COUNT, GRID_COL_COUNT);

    constructTimers();

    registerEventHandlers();

    show();
    run();
}

void cGUI::constructTimers()
{
    /*
    One updates the display with the latest particle position
    Two moves the particle positions

    Two timers are needed so that the windows message pump
    will alternate particle moves and display updates
    */
    myUpdateTimer = new wex::timer(fm, msStep, 1);
    myMoveTimer = new wex::timer(fm, msStep, 2);
}

void cGUI::registerEventHandlers()
{
    fm.events().draw(
        [&](PAINTSTRUCT &ps)
        {
            wex::shapes S(ps);
            particle::drawAll(S);
        });

    fm.events().timer(
        [this](int id)
        {
            switch (id)
            {
            case 2:
                // create new particle at mouse cursor position
                // if left mouse button down and last key pressed was
                // g for a grain of sand
                // w for a drop of water
                // s for a stone
                particle::factory(fm.getMouseStatus(), myKeyDown);

                // update position of all particles free to move
                particle::moveAll();
                break;

            case 1:
                fm.update();
                break;
            }
        });

    fm.events().keydown(
        [this](int keyCode)
        {
            myKeyDown = keyCode;
        });
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

void particle::moveAll()
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

main()
{
    // run the unit tests
    // window appears if tests all passed
    // app exits and message on console id test fails
    // if (!particle::test())
    // {
    //     std::cout << "unit test failed\n";
    //     exit(1);
    // }

    // construct the user interface window
    cGUI theGUI;

    return 0;
}
