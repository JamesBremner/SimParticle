#include <wex.h>

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
    static void moveAll();


    // Each particle moves in a different way
    // This method cannot be compiled
    // but allows the correct method to run for the specialized particles
    virtual void move() = 0;

    // Construct particle of required type
    static particle *factory(
        const wex::sMouse &m,
        int keyDown);

    void setAtRest( bool f = true );
    bool isAtRest() const;

    int color() const;
    std::pair<int, int> location() const;

    virtual std::string text() const;

    void draw(wex::shapes &S) const;

    static bool test();

protected:
    int myColor;
    std::pair<int, int> myLocation;
    bool fAtRest;       // true if particle is blocked

    // store the particles in their grid locations
    static grid_t theGrid;

    void freeGrainsAbove(const std::pair<int,int>& location);
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
