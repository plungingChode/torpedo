#ifndef LOVES_HPP_INCLUDED
#define LOVES_HPP_INCLUDED

#include "widgets.hpp"

class Loves : public Widget
{
protected:
    bool sajat, talalt;

    void x_rajzol();

public:
    Loves(Application *app, int x, int y, int meret, bool sajat);

    void set_talalt(bool val) { talalt = val; }
    void mozgat(int x, int y) { _x = x; _y = y; }

    virtual void draw() override;
    virtual void handle(genv::event ev) override;
};

#endif