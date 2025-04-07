#include "cGUI.h"
#include "SimParticle.h"

cGUI::cGUI()
: fm(wex::maker::make())
{
    fm.move({50, 50, GRID_PXL_WIDTH, GRID_PXL_HEIGHT + 100});
    fm.text("Raven's Particle Simulator");
    fm.bgcolor( 0 );

    particle::setGridSize();

    constructMenu();
    registerEventHandlers();

    fm.show();          // display GUI
    fm.run();           // start windows message pump
}

void cGUI::constructMenu()
{
    wex::menubar mbar(fm);

    wex::menu file(fm);
    file.append(
        "Restart Simulation",
        [&](const std::string &title)
        {
            particle::setGridSize();
            fm.update();
        });
    file.append(
        "Unit Tests",
        [this](const std::string &title)
        {
            if (!particle::test())
            {
                wex::msgbox( "unit test failed" );
                exit(1);
            }
            wex::msgbox( "All unit tests passed" );
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
                "File | Restart Simulation\n"
                "File | Run unit tests.  Msg on success, Msg and exit on failure\n\n"
                "Help | Usage. This message\n"
                "Help | About. Credits\n"
            );
        });
    help.append(
        "About",
        [this](const std::string &title)
        {
            wex::msgbox(
                "Raven's Particle Simulator\n\n"
                "Design by Mishalto https://github.com/Mishalto\n"
                "Coded by James Bremner https://github.com/JamesBremner\n"
                "(c) 2025 MIT Licence Repository https://github.com/JamesBremner/SimParticle"
            );
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
            // create new particle at mouse cursor position
            // if left mouse button down and last key pressed was
            // g for a grain of sand
            // w for a drop of water

            particle::factory(fm.getMouseStatus(), myKeyDown);

            // update position of all particles free to move
            if (particle::moveAll())
            {
                // a particle moved

                /* To optimize simulation speed
                refresh the display only after several moves
                since each move is tiny. 

                For debugging, slow things down
                and let every move show
                by setting DSP_SPEED to 1
                */

                static int count = 0;
                if( count++ > DSP_SPEED ) {
                    fm.update();
                    count = 0;
                }
            }
        });

    fm.events().keydown(
        [this](int keyCode)
        {
            myKeyDown = keyCode;
        });

    myUpdateTimer = new wex::timer(fm, msStep, 1);
}
