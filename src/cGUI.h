#pragma once
#include "SimParticle.h"

/////////////////////////////////////////////////////////
// Look after user interactions

class cGUI
{
public:
    cGUI();

private:
    wex::gui &fm;
    wex::timer *myUpdateTimer;

    int myKeyDown;
    bool myfSlowSim;

    void constructMenu();
    void registerEventHandlers();
};