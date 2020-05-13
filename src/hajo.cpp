#include "hajo.hpp"

using namespace genv;

Hajo::Hajo(Application *app, int x, int y, int hossz_, int cella_meret_, bool vizszintes_, std::function<void(Hajo*, int)> kattintasra_)
    : Widget(app, x, y, cella_meret_+vizszintes_*(hossz_-1)*cella_meret_, cella_meret_+!vizszintes_*(hossz_-1)*cella_meret_),
      hossz(hossz_), vizszintes(vizszintes_), kijelolve(false), cx(-1), cy(-1),
      kattintasra(kattintasra_)
{
}

void Hajo::forgat()
{
    int tmp_x = _size_x;
    _size_x = _size_y;
    _size_y = tmp_x;
    vizszintes = !vizszintes;
}

bool Hajo::benne(int cx_, int cy_)
{
    teglalap a = alak();
    a.h -= 1;
    a.w -= 1;
    return a.benne(cx_, cy_);
}

void Hajo::draw()
{
    gout << move_to(_x, _y) << color(50, 230, 50) << box(_size_x, _size_y);
    if (kijelolve)
    {
        gout << color(255, 255, 255);
        gout << move_to(_x, _y) << box(_size_x, 2)
             << box(-2, _size_y) << box(-(_size_x-1), -2) 
             << box(2, -(_size_y-2));
    }
}

void Hajo::handle(event ev)
{
    if (kijelolve)
    {
        if (ev.button == -btn_right)
        {
            kattintasra(this, btn_right);
        }
        else if (ev.button == -btn_left)
        {
            kattintasra(this, btn_left);
        }
    }
}

bool Hajo::is_selected(int mouse_x, int mouse_y)
{
    kijelolve = Widget::is_selected(mouse_x, mouse_y);
    return kijelolve;
}