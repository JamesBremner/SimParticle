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
#define msStep 100 // update wall clock step milliseconds ( 100 = 10 fps )

class cGUI : public cStarterGUI
{
public:
    cGUI();

private:
    wex::timer *myUpdateTimer;
    wex::timer *myMoveTimer;

    int myKeyDown;

    void draw(wex::shapes &S);
    void move();
};

// store the particles in their grid locations
std::vector<std::vector<particle *>> theGrid;

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

    // std::cout << "keydown " << keydown << "\n";

    // grid location of mouse cursor
    int ix = m.x / 5;
    int iy = m.y / 5;

    // check within bounds
    if (ix < 0 || iy < 0)
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
    auto n = theGrid[location.second - 1][location.first];
    if (n != nullptr)
        n->setAtRest(false);

    // ensure grain above left, if present, is free
    if (location.first - 1 >= 0)
    {
        n = theGrid[location.second - 1][location.first - 1];
        if (n != nullptr)
            n->setAtRest(false);
    }

    // ensure grain above right, if present, is free
    // check for right boundary ( fix TID20 )
    if (location.first + 1 < theGrid[0].size())
    {
        n = theGrid[location.second - 1][location.first + 1];
        if (n != nullptr)
            n->setAtRest(false);
    }
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
    S.rectangle({LX, LY,
                 10, 10});
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

    Most grain will have found a final resting place
    Only the grains that are moving need to be processed
    This gives the simulation a significant performance boost
    */
    if (isAtRest())
        return;

    // check if grain is resting directly on the bottom
    if (LY + 1 >= theGrid.size())
    {
        setAtRest();
        return;
    }

    // try moving grain down
    auto prevLocation = myLocation;
    bool fMoved = false;
    if (theGrid[LX + 1][LY] == NULL)
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
          {50, 50, 1000, 800})
{
    theGrid.resize(500, std::vector<particle *>(500));

    fm.events().draw(
        [&](PAINTSTRUCT &ps)
        {
            wex::shapes S(ps);
            draw(S);
        });

    myUpdateTimer = new wex::timer(fm, msStep, 1);
    myMoveTimer = new wex::timer(fm, msStep, 2);

    fm.events().timer(
        [this](int id)
        {
            switch (id)
            {
            case 2:
                // create new partical at mouse cursor position
                // if left mouse button down and last key pressed was
                // g for a grain of sand
                // w for a drop of water
                // s for a stone
                particle::factory(fm.getMouseStatus(), myKeyDown);

                // update position of all particles free to move
                move();
                fm.update();
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

    show();
    run();
}

void cGUI::draw(wex::shapes &S)
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

void cGUI::move()
{
    // loop over grid
    for (int krow = theGrid.size(); krow >= 0; krow-- )
        for (particle *p : theGrid[krow])
        {
            // if particle present, draw it
            if (p)
                p->move();
        }
}

main()
{
    // run the unit tests
    if( ! particle::test() ) {
        std::cout << "unit test failed\n";
        exit(1);
    }

    cGUI theGUI;
    return 0;
}
