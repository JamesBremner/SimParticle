#pragma once
#include <wex.h>

// "magic numbers"
#define msStep 20 // update wall clock step milliseconds ( 10 = 100 fps )
#define GRID_ROW_COUNT 400
#define GRID_COL_COUNT 500
#define GRID_PXL_WIDTH 1000
#define GRID_PXL_HEIGHT 800

class particle;
typedef  std::vector<std::vector<particle *>> grid_t;

//////////////////////////////////////
// virtual base class for all particles
class particle
{
public:
    particle();
    particle(int color, int x, int y);

    static void setGridSize(
        int rowCount,
        int colCount    );
    static void drawAll(wex::shapes &S);

    /// @brief Move all particles that are not blocked
    /// @return true if any particle moved ( display need to update )
    static bool moveAll();

    // Each particle moves in a different way
    // This method cannot be compiled
    // but allows the correct method to run for the specialized particles
    virtual void move() = 0;

    // Construct particle of required type
    static particle *factory(
        const wex::sMouse &m,
        int keyDown);

    bool isOutGrid(int row, int col ) const;
    particle* get(int row, int col ) const;

    void setAtRest( bool f = true );
    bool isAtRest() const;

    int color() const;
    std::pair<int, int> location() const;

    virtual std::string text() const;

    // draw this particle
    void draw(wex::shapes &S) const;

    static bool test();

protected:

    // attributes of this particle
    int myColor;
    std::pair<int, int> myLocation;
    bool fAtRest;       // true if particle is blocked
    bool myfMoveThis;    // true if this particle moved 

    // static attributes shared by all particles

    static double myGrid2WindowScale;
    static bool myfMove;                // true if any of the particles moved

    // store the particles in their grid locations
    static grid_t theGrid;

    void freeGrainsAbove(const std::pair<int,int>& location);

    /*  The move flags are set to false at start of each position update

    Theyprevent unneccessary display updates
    when no particle moved during the update

    They prevent particles moving multiple times during a position update
    ( 'teleporting' see https://github.com/Mishalto/falling_sand_sim/issues/25 )
     when a particle keeps moving ahead of the grid scan

    */
    static void clearMoveFlags();


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
