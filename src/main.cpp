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
    particle( int color );

    // Each particle moves in a different way
    // This method cannot be compiled
    // but allows the correct method to run for the specialized particles
    virtual void move() = 0;
    
    int color() const;

    protected:
    int myColor;
    std::pair<int,int> myLocation;
};

// A grain of sand
class grain : public particle
{
public:
    grain();
    virtual void move();
};

// A drop of water
class water : public particle
{
public:
    water();
    virtual void move();
};

// A stone
class stone : public particle
{
public:
    stone();
    virtual void move();
};

particle::particle()
: myColor( 0x0000FF )
{}
particle::particle(int color)
: myColor( color )
{}

int particle::color() const
{
    return myColor;
}

grain::grain()
: particle( 0x00FFFF)
{}
void grain::move()
{
    myLocation.second++;
}

water::water()
: particle(0xFF0000)
{ }
void water::move()
{
    myLocation.first++;
}

stone::stone()
: particle(0x000000)
{
}
void stone::move()
{
    myLocation.first--;
}


class cGUI : public cStarterGUI
{
public:
    cGUI()
        : cStarterGUI(
              "Starter",
              {50, 50, 1000, 500}),
          lb(wex::maker::make < wex::label >(fm))
    {
        lb.move(50, 50, 100, 30);
        lb.text("Hello World");

        show();
        run();
    }

private:
    wex::label &lb;
};

main()
{
    cGUI theGUI;
    return 0;
}
