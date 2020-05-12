#include "application.hpp"
#include "jatekter.hpp"
#include "loves.hpp"
#include "gomb.hpp"
#include "hajo.hpp"

using namespace std;
using namespace genv;

class Torpedo : public Application
{
protected:
    const int WND_X = 640;
    const int WND_Y = 480;

    enum status_t
    {
        HAJO_FELRAKAS = -1,
        CELZAS = 0,
        LOVES_VOLT
    };

    koord sajat_tf, ellen_tf;
    int cella_meret;

    int allapot = HAJO_FELRAKAS;
    bool p1_jatekban = true;
    vector<vector<Loves*>> p1_ki_lovesek, p1_be_lovesek;
    vector<vector<Loves*>> p2_ki_lovesek, p2_be_lovesek;

    Jatekter *jt;
    Loves *sajat_celzas, *ellen_celzas;
    Gomb *lezar, *atad;
    Hajo *kijel_hajo;

public:
    Torpedo() 
    { 
        p1_ki_lovesek = p1_be_lovesek = 
            p2_ki_lovesek = p2_be_lovesek = 
                vector<vector<Loves*>>(10, vector<Loves*>(10, nullptr));

        jt = new Jatekter(this, 15, 0, 600, [&](int x, int y, bool sajat){ jatekter_kattintas(x, y, sajat); });
        sajat_tf = jt->sajat_tf_balfelso();
        ellen_tf = jt->ellen_tf_balfelso();
        cella_meret = jt->cella_meret();

        sajat_celzas = nullptr;
        ellen_celzas = nullptr;
        kijel_hajo = nullptr;

        lezar = new Gomb(this, 30, 300, 100, 30, "Lezar", [&]() { lezar_kattintas(); });
        atad  = new Gomb(this, 140, 300, 100, 30, "Atad", [&]() { atad_kattintas(); });

        Hajo *h1 = new Hajo(this, 30, 350, 5, cella_meret, true, [&](Hajo* h, int btn) { hajo_kattintas(h, btn); });
        Hajo *h2 = new Hajo(this, 30, 380, 3, cella_meret, true, [&](Hajo* h, int btn) { hajo_kattintas(h, btn); });
    }

    void jatekost_rajzol(bool p1)
    {
        vector<vector<Loves*>> *ki_lovesek, *be_lovesek;
        if (p1)
        {
            ki_lovesek = &p1_ki_lovesek; 
            be_lovesek = &p1_be_lovesek;
        }
        else
        {
            ki_lovesek = &p2_ki_lovesek;
            be_lovesek = &p2_be_lovesek; 
        }

        for (const vector<Loves*> &v : *ki_lovesek)
            for (Loves *l : v)
                if (l) l->draw();

        for (const vector<Loves*> &v : *be_lovesek)
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
            p1_ki_lovesek[x][y] = sajat_celzas;
            p2_be_lovesek[x][y] = ellen_celzas;
        }
        else
        {
            p2_ki_lovesek[x][y] = sajat_celzas;
            p1_be_lovesek[x][y] = ellen_celzas;
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
        if (sajat_celzas && allapot == CELZAS)
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

    bool elfer(int x, int y)
    {
        return true;
    }

    void hajo_kattintas(Hajo *h, int btn)
    {
        if (btn == btn_left)
        {
            if (kijel_hajo)
            {
                kijel_hajo->kijeloles(false);
            }
            
            kijel_hajo = h;
            kijel_hajo->kijeloles(true);

            koord meret = h->rel_meret();
            koord poz = h->poz();

            printf("x=%d, y=%d [%d x %d]\n", poz.x, poz.y, meret.x, meret.y);
        }
        else if (btn == btn_right)
        {
            // if not placed or has space
            h->forgat();
        }
    }

    void jatekter_kattintas(int x, int y, bool sajat)
    {
        if (sajat)
        {
            printf("[sajat terfel] %d, %d\n", x, y);
            if (allapot == HAJO_FELRAKAS && kijel_hajo)
            {
                kijel_hajo->mozgat(sajat_tf.x+x*cella_meret, sajat_tf.y+y*cella_meret, x, y);
            }
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
        canvas cls(WND_X, WND_Y);
        cls << move_to(0, 0) << color(40, 40, 40) << box(WND_X, WND_Y);

        gout << stamp(cls, 0, 0);
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

            gout << stamp(cls, 0, 0);
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
        gout.open(WND_X, WND_Y);
        event_loop();
    }
};


int main()
{
    Torpedo t;
    t.futtat();
    return 0;
}