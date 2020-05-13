#ifndef HAJO_HPP_INCLUDED
#define HAJO_HPP_INCLUDED

#include "widgets.hpp"
#include "teglalap.hpp"
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

    // (x, y) - tabla koordinatak, (w, h) - szelesseg, magassag
    teglalap alak() const { return teglalap(cx, cy, 1+(hossz-1)*vizszintes, 1+(hossz-1)*!vizszintes); }
    bool benne(int cx, int cy);

    void mozgat(int x, int y, int cx_, int cy_) { _x = x; _y = y; cx = cx_; cy = cy_; }
    void forgat();

    virtual void draw() override;
    virtual void handle(genv::event ev) override;
    virtual bool is_selected(int mouse_x, int mouse_y) override;
};

#endif