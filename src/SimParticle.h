#pragma once
#include <wex.h>

// "magic numbers"

#define msStep 1   // move update wall clock step milliseconds ( 10 = 100 fps )
#define DSP_SPEED 5  // number moves between display udates

#define GRID_COL_COUNT 500
#define GRID_ROW_COUNT 200

#define GRID_PXL_WIDTH 1000
#define GRID_PXL_HEIGHT 400


class particle;

using  grid_t = std::vector<std::vector<particle *>>;

//////////////////////////////////////
// virtual base class for all particles
class particle
{
public:
    particle();
    particle(int color, int x, int y);

    // reset the grid to empty
    static void setGridSize();

    // Construct particle of required type
    static particle *factory(
        const wex::sMouse &m,
        int keyDown);

    // refresh display of all particles
    static void drawAll(wex::shapes &S);

    /// @brief Move all particles that are not blocked
    /// @return true if any particle moved ( display needs to update )
    static bool moveAll();

    // Each particle moves in a different way
    // This method cannot be compiled
    // but allows the correct method to run for the specialized particles
    virtual void move() = 0;

    // move this particle one cell
    void moveDown();
    void moveLeft();
    void moveRight();

    // true if inside the grid
    bool isOutGrid(int row, int col) const;

    // get particle at grid location
    particle *get(int row, int col) const;

    // particle blocked flag accessors
    // blocked particles are skipped in move update ( performance optimization )
    void setAtRest(bool f = true);
    bool isAtRest() const;

    // attribute accessors
    int color() const;
    std::pair<int, int> location() const;

    // human readable status ( for debugging )
    virtual std::string text() const;

    // draw this particle
    void draw(wex::shapes &S) const;

    // run the unit tests
    static bool test();

protected:

    // attributes of this particle

    int myColor;
    std::pair<int, int> myLocation;
    bool fAtRest;               // true if particle is blocked
    bool myfMoveThis;           // true if this particle moved
    bool myfReplacedWater;      // true if particle is sinking through water

    // static attributes shared by all particles

    static double myGrid2WindowScale;
    static bool myfMove; // true if any of the particles moved

    // store the particles in their grid locations
    static grid_t theGrid;

    /* Clear the fAtRest flags for any particles above

    When a particle moves it will free particles above that were blocked

    This only works properly if the grid is scanned from bottom to top
    https://github.com/JamesBremner/SimParticle/issues/1#issuecomment-2781424962

    */
    void freeGrainsAbove();

    /*  The move flags are set to false at start of each position update

    They prevent unneccessary display updates
    when no particle moved during the update

    They prevent particles moving multiple times during a position update
    ( 'teleporting' see https://github.com/Mishalto/falling_sand_sim/issues/25 )
     when a particle keeps moving ahead of the grid scan

    */
    static void clearMoveFlags();
    void setMoveFlags();

    // unit tests

    static bool testGrainMove();
    static bool testWaterMove();
    static bool testWaterFlow();
};

//////////////////////////////////////
// A grain of sand
class grain : public particle
{
public:
    grain(int x, int y);
    virtual void move();
    virtual std::string text() const;
};
//////////////////////////////////////
// A drop of water
class water : public particle
{
public:
    water(int x, int y);
    virtual void move();
    virtual std::string text() const;
private:

    /// @brief distance water must spread to find its own level
    /// @return distance or INT_MAX if blocked
    int flowDistance();
};
//////////////////////////////////////
// A stone
class stone : public particle
{
public:
    stone(int x, int y);
    virtual void move();
    virtual std::string text() const;
};
