#ifndef KOORD_HPP_INCLUDED
#define KOORD_HPP_INCLUDED

struct koord
{
    int x, y;
    koord(int x_, int y_) : x(x_), y(y_) {}
    koord() : x(0), y(0) {}
};

#endif