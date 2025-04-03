#pragma once
#include "SimParticle.h"
class cStarterGUI
{
public:

    cStarterGUI(
        const std::string &title,
        const std::vector<int> &vlocation)
        : fm(wex::maker::make())
    {
        fm.move(vlocation);
        fm.text(title);

        fm.events().draw(
            [&](PAINTSTRUCT &ps)
            {
                wex::shapes S(ps);
                draw(S);
            });
    }
    /** Draw nothing
     * 
     * An application should over-ride this method
     * to perform any drawing reuired
     */
    virtual void draw( wex::shapes& S )
    {

    }


protected:
    wex::gui &fm;
};

class cGUI : public cStarterGUI
{
public:
    cGUI();

private:
    wex::timer *myUpdateTimer;

    int myKeyDown;

    // void constructTimers();
    void registerEventHandlers();
};