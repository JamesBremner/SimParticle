#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include "cStarterGUI.h"

// virtual base class for all particles
class particle
{
public:
    particle();
    particle(int color, int x, int y);

    // Each particle moves in a different way
    // This method cannot be compiled
    // but allows the correct method to run for the specialized particles
    virtual void move() = 0;

    // Construct particle of required type
    static particle *factory(
        const wex::sMouse &m,
        int keyDown);

    int color() const;
    std::pair<int, int> location() const;

    virtual std::string text() const;

    void draw(wex::shapes &S) const;

protected:
    int myColor;
    std::pair<int, int> myLocation;
};

// A grain of sand
class grain : public particle
{
public:
    grain(int x, int y);
    virtual void move();
    virtual std::string text() const;
};

// A drop of water
class water : public particle
{
public:
    water(int x, int y);
    virtual void move();
    virtual std::string text() const;
};

// A stone
class stone : public particle
{
public:
    stone(int x, int y);
    virtual void move();
    virtual std::string text() const;
};

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
    int ix = m.x / 100;
    int iy = m.y / 100;

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
    S.rectangle({myLocation.first, myLocation.second,
                 2, 2});
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

class cGUI : public cStarterGUI
{
public:
    cGUI()
        : cStarterGUI(
              "Particle Simulator",
              {50, 50, 1000, 500})
    {
        myGrid.resize(100, std::vector<particle *>(100));

        fm.events().draw(
            [&](PAINTSTRUCT &ps)
            {
                wex::shapes S(ps);
                draw(S);
                // fm.update();
            });

        myUpdateTimer = new wex::timer(fm, 500);
        fm.events().timer(
            [this](int id)
            {
                std::cout << " timer ";
                auto *p = particle::factory(fm.getMouseStatus(), myKeyDown);
                if (p)
                {
                    auto loc = p->location();
                    myGrid[loc.first][loc.second] = p;
                }

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

private:
    std::vector<std::vector<particle *>> myGrid;

    wex::timer *myUpdateTimer;

    int myKeyDown;

    void draw(wex::shapes &S);
};

void cGUI::draw(wex::shapes &S)
{
    for (auto &row : myGrid)
    {
        for (particle *p : row)
        {
            if (p)
            {
                std::cout << p->text();
                p->draw(S);
            }
        }
    }
    std::cout << "\n";
}

main()
{
    cGUI theGUI;
    return 0;
}
