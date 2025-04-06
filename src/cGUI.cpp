#include "cGUI.h"

cGUI::cGUI()
: fm(wex::maker::make())
{
    fm.move({50, 50, GRID_PXL_WIDTH, GRID_PXL_HEIGHT + 50});
    fm.text("Raven's Particle Simulator");
    fm.bgcolor( 0 );

    particle::setGridSize(
        GRID_ROW_COUNT, GRID_COL_COUNT);

    myUpdateTimer = new wex::timer(fm, msStep, 1);

    registerEventHandlers();

    fm.show();
    fm.run();
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
            // s for a stone
            particle::factory(fm.getMouseStatus(), myKeyDown);

            // update position of all particles free to move
            if (particle::moveAll())
            {
                // a particle moved

                static int count = 0;
                if( count++ > 5 ) {
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
}
