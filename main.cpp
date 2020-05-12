#include "application.hpp"
#include "jatekter.hpp"
#include "loves.hpp"
#include "gomb.hpp"

using namespace std;
using namespace genv;

class Torpedo : public Application
{
protected:
    enum status_t
    {
        CELZAS = 0,
        LOVES_VOLT
    };

    koord sajat_tf, ellen_tf;
    int cella_meret;

    int allapot = CELZAS;
    bool p1_jatekban = true;
    vector<vector<Loves*>> p1_lovesek, p2_lovesek;

    Jatekter *jt;
    Loves *sajat_celzas, *ellen_celzas;
    Gomb *lezar, *atad;

public:
    Torpedo() 
    { 
        p1_lovesek = p2_lovesek = vector<vector<Loves*>>(10, vector<Loves*>(10, nullptr));

        jt = new Jatekter(this, 15, 0, 600, [&](int x, int y, bool sajat){ jatekter_kattintas(x, y, sajat); });
        sajat_tf = jt->sajat_tf_balfelso();
        ellen_tf = jt->ellen_tf_balfelso();
        cella_meret = jt->cella_meret();

        sajat_celzas = nullptr;
        ellen_celzas = nullptr;

        lezar = new Gomb(this, 30, 300, 100, 30, "Lezar", [&]() { lezar_kattintas(); });
        atad  = new Gomb(this, 140, 300, 100, 30, "Atad", [&]() { atad_kattintas(); });
    }

    void jatekost_rajzol(bool p1)
    {
        vector<vector<Loves*>> *lovesek;
        if (p1)
        {
            lovesek = &p1_lovesek; 
        }
        else
        {
            lovesek = &p2_lovesek; 
        }

        for (const vector<Loves*> &v : *lovesek)
            for (Loves *l : v)
                if (l) l->draw();
    }

    koord cella(int x, int y, bool sajat)
    {
        koord k;
        if (sajat)
        {
            k.x = sajat_tf.x+x*cella_meret;
            k.y = sajat_tf.y+y*cella_meret;
        }
        else
        {
            k.x = ellen_tf.x+x*cella_meret;
            k.y = ellen_tf.y+y*cella_meret;
        }
        return k;
    }

    void uj_celzas(int x, int y)
    {
        koord k = cella(x, y, false);
        sajat_celzas = new Loves(this, k.x, k.y, cella_meret, true);
        widgets.pop_back();

        k = cella(x, y, true);
        ellen_celzas = new Loves(this, k.x, k.y, cella_meret, false);
        widgets.pop_back();

        if (p1_jatekban)
        {
            p1_lovesek[x][y] = sajat_celzas;
            p2_lovesek[x][y] = ellen_celzas;
        }
        else
        {
            p2_lovesek[x][y] = sajat_celzas;
            p1_lovesek[x][y] = ellen_celzas;
        }
    }

    void celzast_mozgat(int x, int y)
    {
        koord k = cella(x, y, false);
        sajat_celzas->mozgat(k.x, k.y);

        k = cella(x, y, true);
        ellen_celzas->mozgat(k.x, k.y);
    }

    void lezar_kattintas()
    {
        if (allapot == CELZAS)
        {
            // clear aim
            sajat_celzas = nullptr;
            ellen_celzas = nullptr;

            // check hit

            // prevent further aim
            allapot = LOVES_VOLT;
        }
    }

    void atad_kattintas()
    {
        if (allapot == LOVES_VOLT)
        {
            // change player
            p1_jatekban = !p1_jatekban;

            // allow aim
            allapot = CELZAS;
        }
    }

    void jatekter_kattintas(int x, int y, bool sajat)
    {
        if (sajat)
        {
            printf("[sajat terfel] %d, %d\n", x, y);
        }
        else
        {
            if (allapot == CELZAS)
            {
                if (!sajat_celzas)
                {
                    uj_celzas(x, y);
                }
                else
                {
                    celzast_mozgat(x, y);
                }
            }
            
            printf("[ellen terfel] %d, %d\n", x, y);
        }
    }

    void event_loop() override
    {
        for (Widget * w : widgets) {
            w->draw();
        }
        gout << refresh;

        event ev;
        int focus = -1;
        while(gin >> ev ) {
            if (ev.type == ev_mouse && ev.button==btn_left) {
                for (size_t i=0;i<widgets.size();i++) {
                    if (widgets[i]->is_selected(ev.pos_x, ev.pos_y)) {
                        focus = i;
                    }
                }
            }
            if (focus!=-1) {
                widgets[focus]->handle(ev);
            }
            for (Widget * w : widgets) {
                w->draw();
            }

            jatekost_rajzol(p1_jatekban);

            if (ev.keycode == key_enter)
            {
                action("enter");
                p1_jatekban = !p1_jatekban;
            }
            gout << refresh;
        }
    }

    void action(std::string s) override 
    { 

    }

    void futtat()
    {
        gout.open(640, 480);
        event_loop();
    }
};


int main()
{
    Torpedo t;
    t.futtat();
    return 0;
}