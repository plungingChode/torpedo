#ifndef JATEKTER_HPP_INCLUDED
#define JATEKTER_HPP_INCLUDED

#include "widgets.hpp"
#include "teglalap.hpp"
#include "koord.hpp"
#include <functional>

class Jatekter : public Widget
{
protected:
    const int CELLA_MERET;
    std::function<void(int, int, bool)> kattintasra;
    genv::canvas hatter;
    teglalap ellen_terfel, sajat_terfel;


    void hatteret_rajzol(int x_offset, genv::color c);

public:
    Jatekter(Application *app, int x, int y, int size_x, std::function<void(int, int, bool)> on_click);

    koord sajat_tf_balfelso() const { return koord(sajat_terfel.x, sajat_terfel.y); }
    koord ellen_tf_balfelso() const { return koord(ellen_terfel.x, ellen_terfel.y); }
    int cella_meret() const { return CELLA_MERET; }

    virtual void draw() override;
    virtual void handle(genv::event ev) override;    
};


#endif