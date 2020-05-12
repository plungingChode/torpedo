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

bool Hajo::atfed(Hajo *h)
{
    return _x+_size_x >= h->_x && h->_x+h->_size_x >= _x
        && _y+_size_y >= h->_y && h->_y+h->_size_y >= _y;
}

void Hajo::draw()
{
    if (kijelolve)
    {
        gout << move_to(_x-2, _y-2) << color(255, 255, 255) << box(_size_x+4, _size_y+4);
    }
    gout << move_to(_x, _y) << color(50, 230, 50) << box(_size_x, _size_y);
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