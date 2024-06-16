#include "lib/game.h"
#include <chrono>
#include <ncurses.h>

static constexpr int FPS{60};

class Wall
{
public:
    Wall(int _x, int _y)
    {
        x = _x;
        y = _y;
    }

    void Update()
    {
        grid.SetTile(x, y, ascii);
    }

    static constexpr char ascii = '#';

private:
    int x{};
    int y{};
};

class Nibble
{
public:
    Nibble(int _x, int _y) 
    {
        x = _x;
        y = _y;
    }

    void Update()
    {
        grid.SetTile(x, y, ascii);
    }

    bool Overlaps(int _x, int _y) { return x == _x && y == _y; }

    static constexpr char ascii = '.';

private:
    int x{};
    int y{};
};

class Map
{
public:
    Map()
    {
        srand(time(NULL));
        int xMax = grid.GetWidth()-1;
        int yMax = grid.GetHeight()-1;
        grid.DrawRectangle(0, 0, xMax, yMax, Wall::ascii);
        for (int x = 2; x < grid.GetMidWidth(); x+=2)
        {
            if (x >= grid.GetMidHeight()) break;
            grid.DrawRectangle(x, x, xMax-x, yMax-x, Wall::ascii);
            grid.SetTile(rand() % (xMax-x-x-1)+x+1, x);
            grid.SetTile(rand() % (xMax-x-x-1)+x+1, yMax-x);
        }
            
        for (int x = 0; x < grid.GetWidth(); x++)
            for (int y = 0; y < grid.GetHeight(); y++)
                if (!grid.IsCollision(x, y, Wall::ascii) && rand()%2)
                    nibbles.emplace_back(x,y);
    }                 

    void Update()
    {
        for (auto& nibble : nibbles)
            nibble.Update();
    }

    void Collide(int x, int y)
    {
        for (int i = 0; i < nibbles.size(); i++)
        {
            if (!nibbles[i].Overlaps(x, y)) continue;
            nibbles[i] = nibbles[nibbles.size()-1];
            nibbles.pop_back();
        }
    }

    bool Empty() { return nibbles.empty(); }

private:
    vector<Nibble> nibbles{};
};

class Player
{
public:
    void Update(Map& map)
    {
        grid.SetTile(x, y);

        switch (userInput)
        {
            case UserInput::Left: 
                xSpeed = -1;
                ySpeed = 0;
                break;
            case UserInput::Right:
                xSpeed = 1;
                ySpeed = 0;
                break;
            case UserInput::Up:
                xSpeed = 0;
                ySpeed = -1;
                break;
            case UserInput::Down:
                xSpeed = 0;
                ySpeed = 1;
                break;
            default: 
                break;
        }

        if (frames++ >= framesPerMove)
        {
            int _x = x;
            int _y = y;
            x += xSpeed;
            y += ySpeed;
            if (grid.IsOutOfBounds(x, y) || grid.IsCollision(x, y, Wall::ascii))
            {
                x = _x;
                y = _y;
            }
            else if (grid.IsCollision(x, y, Nibble::ascii))
            {
                map.Collide(x, y);
            }
            frames = 0;
        }

        grid.SetTile(x, y, ascii);
    }

    static constexpr char ascii = '@';

private:
    int x{grid.GetMidWidth()};
    int y{grid.GetMidHeight()};
    int xSpeed{};
    int ySpeed{};
    int frames{};
    int framesPerMove{FPS/20};
};

class Maze : public Game
{
public:
    Maze() : Game(FPS) {}

protected:
    virtual void Update() override
    {
        map.Update();
        player.Update(map);
        if (map.Empty())
            End("You ate all the nibbles! Yum!");
    }

private:
    Player player{};
    Map map{};
};

#include "ncurses.h"

int main()
{
    Maze maze{};
    maze.Start();
    return 0;
}