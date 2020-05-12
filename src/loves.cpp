#include "loves.hpp"

using namespace genv;

Loves::Loves(Application *app, int x, int y, int meret, bool sajat_)
    : Widget(app, x, y, meret, meret), sajat(sajat_), talalt(false)
{
}

void Loves::x_rajzol()
{
    gout << move_to(_x, _y) << line(_size_x, _size_x);
    gout << move_to(_x+_size_x, _y) << line(-_size_x, _size_x);
};

void Loves::draw()
{
    if (sajat)
    {
        if (talalt)
        {
            gout << move_to(_x+1, _y+1) << color(230, 50, 50) << box(_size_x-1, _size_y-1);
            gout << color(250, 250, 250);
        }
        else
        {
            gout << color(250, 250, 250);
            // gout << color(230, 50, 50);
        }
    }
    else
    {
        gout << color(250, 250, 250);
        // gout << color(50, 230, 50);
    }
    x_rajzol();
}

void Loves::handle(genv::event ev)
{
}
