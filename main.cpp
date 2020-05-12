#include "application.hpp"
#include "jatekter.hpp"
#include "loves.hpp"
#include "gomb.hpp"
#include "hajo.hpp"
#include <algorithm>

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
    vector<Hajo*> p1_hajok, p2_hajok, *rajzolt_hajok;

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


        hajok_letrehoz(p1_hajok);
        hajok_letrehoz(p2_hajok);
    }

    void hajok_letrehoz(vector<Hajo*> &hajok)
    {
        vector<int> meretek = {5, 3, 3, 2, 2, 2};
        auto hajo_callback = [&](Hajo *h, int btn)
        {
            hajo_kattintas(h, btn);
        };

        Hajo *h;
        h = new Hajo(this, 30, 350, 5, cella_meret, true, hajo_callback);
        widgets.pop_back();
        hajok.push_back(h);

        h = new Hajo(this, 30, 380, 3, cella_meret, true, hajo_callback);
        widgets.pop_back();
        hajok.push_back(h);

        h = new Hajo(this, 30, 410, 3, cella_meret, true, hajo_callback);
        widgets.pop_back();
        hajok.push_back(h);

        h = new Hajo(this, 200, 350, 2, cella_meret, true, hajo_callback);
        widgets.pop_back();
        hajok.push_back(h);
    }

    int jatekon_kivuli_hajok()
    {
        return std::count_if(
            rajzolt_hajok->begin(), 
            rajzolt_hajok->end(), 
            [](Hajo* h) { return h->rel_poz().x == -1; }
        );
    }

    void jatekost_rajzol(bool p1)
    {
        vector<vector<Loves*>> *ki_lovesek, *be_lovesek;
        if (p1)
        {
            ki_lovesek = &p1_ki_lovesek; 
            be_lovesek = &p1_be_lovesek;
            rajzolt_hajok = &p1_hajok;
        }
        else
        {
            ki_lovesek = &p2_ki_lovesek;
            be_lovesek = &p2_be_lovesek;
            rajzolt_hajok = &p2_hajok;
        }

        for (Hajo *h : *rajzolt_hajok)
            h->draw();

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
        if (allapot == CELZAS && sajat_celzas)
        {
            // clear aim
            sajat_celzas = nullptr;
            ellen_celzas = nullptr;

            // check hit

            // prevent further aim
            allapot = LOVES_VOLT;
        }

        if (allapot == HAJO_FELRAKAS && jatekon_kivuli_hajok() == 0)
        {
            if (!p1_jatekban)
            {
                allapot = CELZAS;
            }
            p1_jatekban = !p1_jatekban;
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

    bool elfer_forgatva(Hajo *h)
    {
        koord poz = h->rel_poz();
        koord meret = h->rel_meret();

        int uj_szel = meret.y;
        int uj_mag = meret.x;

        return poz.x + uj_szel <= 10 && poz.y + uj_mag <= 10 && !atfed(h, poz.x, poz.y, uj_szel, uj_mag);
    }

    bool atfed(Hajo *hajo, int x, int y, int szel, int mag)
    {
        vector<Hajo*> *hajok;
        if (p1_jatekban)
        {
            hajok = &p1_hajok;
        }
        else
        {
            hajok = &p2_hajok;
        }

        for (Hajo *h : *hajok)
        {
            koord h_poz = h->rel_poz();
            koord h_mer = h->rel_meret();
            if (hajo != h)
            {
                bool atfed = 
                   x+szel > h_poz.x && h_poz.x+h_mer.x > x &&
                   y+mag  > h_poz.y && h_poz.y+h_mer.y > y;

                if (atfed) return true;
            }
        }
        return false;
    }

    void hajo_kattintas(Hajo *h, int btn)
    {
        if (btn == btn_left)
        {
            // if (kijel_hajo)
            // {
            //     kijel_hajo->kijeloles(false);
            // }
            
            kijel_hajo = h;
            // kijel_hajo->kijeloles(true);
        }
        else if (btn == btn_right)
        {
            if (elfer_forgatva(h))
            {
                h->forgat();
            }
        }
    }

    void jatekter_kattintas(int x, int y, bool sajat)
    {
        if (sajat)
        {
            printf("[sajat terfel] %d, %d\n", x, y);
            if (allapot == HAJO_FELRAKAS && kijel_hajo)
            {
                koord meret = kijel_hajo->rel_meret();
                x = std::min(x, 10-meret.x);
                y = std::min(y, 10-meret.y);
                if (!atfed(kijel_hajo, x, y, meret.x, meret.y))
                {
                    kijel_hajo->mozgat(sajat_tf.x+x*cella_meret, sajat_tf.y+y*cella_meret, x, y);
                }
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
        jatekost_rajzol(p1_jatekban);
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

            // ez igy nagyon nem jo...
            if (allapot == HAJO_FELRAKAS)
            {
                for (Hajo *h : *rajzolt_hajok)
                {
                    if (h->is_selected(ev.pos_x, ev.pos_y))
                    {
                        h->handle(ev);
                        break;
                    }
                }
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