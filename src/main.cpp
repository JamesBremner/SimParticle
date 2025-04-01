#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include "cStarterGUI.h"
#include "SimParticle.h"

class cGUI : public cStarterGUI
{
public:
    cGUI();
    
private:

    wex::timer *myUpdateTimer;

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
      myLocation(std::make_pair(x, y))
{
}
particle *particle::factory(
    const wex::sMouse &m,
    int keydown)
{
    // particle generated only if left mouse button down
    if (!m.left)
        return NULL;

    std::cout << "keydown " << keydown << "\n";

    // grid location of mouse cursor
    int ix = m.x / 5;
    int iy = m.y / 5;

    // particle type selected by key last pressed
    switch (keydown)
    {

    case 71:
        return new grain(ix, iy);

    case 83:
        return new stone(ix, iy);

    case 87:
        return new water(ix, iy);

    default:
        return NULL;
    }
}

void particle::set_at_rest( bool f )
{
    fAtRest = f;
}

void particle::free_grains_above(const std::pair<int,int>& location)
 {
    // free grains that may have been blocked;

    // check if grain is on window bottom - always blocked
    if (location.second - 1 < 0) { return; }

    // ensure grain above, if present, is free
    auto n = theGrid[location.second - 1][location.first];
    if (n != nullptr)
        n->set_at_rest(false);

    // ensure grain above left, if present, is free
    if (location.first - 1 >= 0) {
        n = theGrid[location.second - 1][location.first - 1];
        if (n != nullptr)
            n->set_at_rest(false);
    }

    // ensure grain above right, if present, is free
    // check for right boundary ( fix TID20 )
    if (location.first + 1 < theGrid[0].size()) {
        n = theGrid[location.second - 1][location.first + 1];
        if (n != nullptr)
            n->set_at_rest(false);
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
    S.rectangle({myLocation.first, myLocation.second,
                 10, 10});
}

grain::grain(int x, int y)
    : particle(0x00FFFF, x, y)
{
    myLocation = std::make_pair(x, y);
}
void grain::move()
{
    myLocation.second++;
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
      {50, 50, 1000, 500})
{
    theGrid.resize(500, std::vector<particle *>(500));

fm.events().draw(
    [&](PAINTSTRUCT &ps)
    {
        wex::shapes S(ps);
        draw(S);
        // fm.update();
    });

myUpdateTimer = new wex::timer(fm, 50);
fm.events().timer(
    [this](int id)
    {
        auto *p = particle::factory(fm.getMouseStatus(), myKeyDown);
        if (p)
        {
            auto loc = p->location();
            theGrid[loc.first][loc.second] = p;
        }

        move();

        fm.update();
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
                p->draw(S);
        }
}

void cGUI::move()
{
    // loop over grid
    for (auto &row : theGrid)
        for (particle *p : row)
        {
            // if particle present, draw it
            if (p)
                p->move();
        }
}

main()
{
    cGUI theGUI;
    return 0;
}
