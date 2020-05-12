#include "jatekter.hpp"

using namespace genv;

Jatekter::Jatekter(Application *app, int x, int y, int size_x, std::function<void(int, int, bool)> on_click_)
    : Widget(app, x, y, size_x, size_x/2-30), CELLA_MERET((size_x/2 - 50)/10), kattintasra(on_click_),
      ellen_terfel(_x+30, _y+16, 10*CELLA_MERET, 10*CELLA_MERET),
      sajat_terfel(_x+30+size_x/2+10, _y+16, 10*CELLA_MERET, 10*CELLA_MERET)
{
    hatter.open(_size_x, _size_y);
    // hatter << move_to(0, 0) << color(100, 100, 100) << box(_size_x, _size_y); 
    hatteret_rajzol(30, color(230, 50, 50));
    hatteret_rajzol(30+_size_x/2+10, color(50, 230, 50));
}

void Jatekter::hatteret_rajzol(int x_offset, genv::color c)
{
    hatter << c;
    for (int i = 0; i < 10; i++)
    {
        hatter << move_to(x_offset+i*CELLA_MERET, 16) << line(0, 10*CELLA_MERET);
        hatter << move_to(x_offset+i*CELLA_MERET+10, hatter.cascent()) << text(char(65+i)); // betuk

        hatter << move_to(x_offset, i*CELLA_MERET+16) << line(10*CELLA_MERET, 0);

        std::string num = std::to_string(i+1);
        hatter << move_to(x_offset-hatter.twidth(num)-5, i*CELLA_MERET+35) << text(num); // szamok
    }
    hatter << move_to(x_offset+10*CELLA_MERET, 16) << line(0, 10*CELLA_MERET);
    hatter << move_to(x_offset, 10*CELLA_MERET+16) << line(10*CELLA_MERET+1, 0);
}

void Jatekter::draw()
{
    gout << stamp(hatter, _x, _y);
}

void Jatekter::handle(event ev)
{
    if (ev.button == btn_left)
    {
        if (sajat_terfel.benne(ev.pos_x, ev.pos_y))
        {
            int rel_x = (ev.pos_x - sajat_terfel.x) / CELLA_MERET;
            int rel_y = (ev.pos_y - sajat_terfel.y) / CELLA_MERET;
            kattintasra(rel_x, rel_y, true);
        }
        else if (ellen_terfel.benne(ev.pos_x, ev.pos_y))
        {
            int rel_x = (ev.pos_x - ellen_terfel.x) / CELLA_MERET;
            int rel_y = (ev.pos_y - ellen_terfel.y) / CELLA_MERET;
            kattintasra(rel_x, rel_y, true);
        }
    }
}