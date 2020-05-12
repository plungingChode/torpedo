#ifndef HAJO_HPP_INCLUDED
#define HAJO_HPP_INCLUDED

#include "widgets.hpp"
#include "koord.hpp"
#include <functional>

class Hajo : public Widget
{
protected:
    int hossz;
    bool vizszintes, kijelolve;
    int cx, cy;

    std::function<void(Hajo*, int)> kattintasra;

public:
    Hajo(Application *app, int x, int y, int hossz, int cella_meret, bool vizszintes, std::function<void(Hajo*, int)> kattintasra);

    koord poz() const { return koord(_x, _y); }
    koord rel_poz() const { return koord(cx, cy); }
    koord meret() const { return koord(_size_x, _size_y); }
    koord rel_meret() const { return koord(1+(hossz-1)*vizszintes, 1+(hossz-1)*!vizszintes); }

    void kijeloles(bool val) { kijelolve = val; }
    void mozgat(int x, int y, int cx_, int cy_) { _x = x; _y = y; cx = cx_; cy = cy; }
    void forgat();

    virtual void draw() override;
    virtual void handle(genv::event ev) override;
};

#endif