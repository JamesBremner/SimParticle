#include "cGUI.h"
#include "SimParticle.h"

cGUI::cGUI()
    : fm(wex::maker::make()),
    myfSlowSim( false )
{
    fm.move({50, 50, GRID_PXL_WIDTH, GRID_PXL_HEIGHT + 100});
    fm.text("Raven's Particle Simulator");
    fm.bgcolor(0);

    particle::setGridSize();

    constructMenu();
    registerEventHandlers();

    fm.show(); // display GUI
    fm.run();  // start windows message pump
}

void cGUI::constructMenu()
{
    wex::menubar mbar(fm);

    wex::menu file(fm);
    file.append(
        "Restart Fast",
        [&](const std::string &title)
        {
            myfSlowSim = false;
            particle::setGridSize();
            fm.update();
        });
    file.append(
        "Restart Slow",
        [&](const std::string &title)
        {
            myfSlowSim = true;
            particle::setGridSize();
            fm.update();
        });
    file.append(
        "Unit Tests",
        [this](const std::string &title)
        {
            if (!particle::test())
            {
                wex::msgbox("unit test failed");
                exit(1);
            }
            wex::msgbox("All unit tests passed");
            particle::setGridSize();
            fm.update();
        });

    mbar.append("Run", file);

    wex::menu help(fm);
    help.append(
        "Usage",
        [this](const std::string &title)
        {
            wex::msgbox(
                "g key to enable sand grain generation\n"
                "w key to enable water droplet generation\n"
                "Click left mouse button to generate sand or water particles\n\n"
                "Menus:\n\n"
                "File | Restart Fast. Restart and run fast simulation. Fast is default\n"
                "File | Restart Slow. Restart and run slow simulation\n"
                "File | Run unit tests.  Msg on success, Msg and exit on failure\n\n"
                "Help | Usage. This message\n"
                "Help | About. Credits\n");
        });
    help.append(
        "About",
        [this](const std::string &title)
        {
            wex::msgbox(
                "Raven's Particle Simulator\n\n"
                "Design by Mishalto https://github.com/Mishalto\n"
                "Coded by James Bremner https://github.com/JamesBremner\n"
                "(c) 2025 MIT Licence Repository https://github.com/JamesBremner/SimParticle");
        });
    mbar.append("Help", help);
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
            if( myfSlowSim ) {

                // slow simulation requested

                // on left mouse button down create new particle
                particle::factory(fm.getMouseStatus(), myKeyDown);

                // move all free particles one cell
                particle::moveAll();

                // refresh display
                fm.update();

                // return to waiting
                return;
            }

            // true when there is new particle not displayed
            bool fNewParticle = false;

            // create new particle at mouse cursor position
            // if left mouse button down and last key pressed was
            // g for a grain of sand
            // w for a drop of water

            while (particle::factory(fm.getMouseStatus(), myKeyDown))
            {
                fNewParticle = true;

                // update position of all particles free to move
                // untill all have finally come to rest
                while (particle::moveAll())
                {
                    // a particle moved
                }

                // all particles have come to rest
                // stop and refresh the display
                // if sufficient new particles have been created
                static int count = 0;
                if (count++ > PARTS_DISPLAY)
                {
                    fNewParticle = false;
                    fm.update();
                    count = 0;
                    break;
                }
            }

            // ensure that last particle is displayed
            if (fNewParticle)
                fm.update();
        });

    fm.events().keydown(
        [this](int keyCode)
        {
            myKeyDown = keyCode;
        });

    /*
    The simulation cannot be run flat out
    because the app would be unresponsive
    and the display would never update

    When the simulation stops 
    because PARTS_DISPLAY particles have been created
    a queued timer event will get it going again
    */
    myUpdateTimer = new wex::timer(fm, 20);
}
