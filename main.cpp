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

    // lehetseges allapotok
    enum status_t
    {
        HAJO_FELRAKAS = -1,
        CELZAS = 0,
        LOVES_VOLT,
        VEGE
    };

    // terfelek, loves poziciohoz
    koord sajat_tf, ellen_tf;
    int cella_meret;

    int allapot = HAJO_FELRAKAS;
    bool p1_jatekban = true;
    int p1_hp, p2_hp;

    // legutobbi celzas koordinatai
    koord celzas;
    
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

        p1_hp = hajok_letrehoz(p1_hajok);
        p2_hp = hajok_letrehoz(p2_hajok);
    }

    int hajok_letrehoz(vector<Hajo*> &hajok)
    {
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

        h = new Hajo(this, 200, 380, 2, cella_meret, true, hajo_callback);
        widgets.pop_back();
        hajok.push_back(h);

        return 5 + 3 + 3 + 2 + 2;
    }

    // A jelenlegi jatekosnak hany hajoja nincs meg jatekban
    int jatekon_kivuli_hajok()
    {
        return std::count_if(
            rajzolt_hajok->begin(), 
            rajzolt_hajok->end(), 
            [](Hajo* h) { return h->alak().x == -1; }
        );
    }

    // Adott jatekos hajoinak, loveseinek kirajzolasa
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

    // Cella kepernyo koordinatai
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

    // Loves elokeszitese
    void uj_celzas(int x, int y)
    {
        // Piros terfel megjelolese
        koord k = cella(x, y, false);
        sajat_celzas = new Loves(this, k.x, k.y, cella_meret, true);
        widgets.pop_back();

        // Ellenfel zold terfelere is kell
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

    // (x, y) pontban van-e 'tamadott' hajo?
    bool talalt(int x, int y, const vector<Hajo*> &tamadott)
    {
        for (Hajo *h : tamadott)
        {
            if (h->benne(x, y))
            {
                return true;
            }
        }
        return false;
    }

    void lezar_kattintas()
    {
        if (allapot == CELZAS && sajat_celzas)
        {

            int *hp;
            vector<Hajo*> *tamadott;
            if (p1_jatekban)
            {
                hp = &p2_hp;
                tamadott = &p2_hajok;
            }
            else
            {
                hp = &p1_hp;
                tamadott = &p1_hajok;
            }
            if (talalt(celzas.x, celzas.y, *tamadott))
            {
                printf("[loves       ] %c%d talalt\n", char(65+celzas.x), celzas.y + 1);
                
                --(*hp);
                sajat_celzas->set_talalt(true);
                ellen_celzas->set_talalt(true);
            }
            else
            {
                printf("[loves       ] %c%d\n", char(65+celzas.x), celzas.y + 1);
            }

            // nezzuk meg, nyert-e valaki?
            if (p1_hp == 0)
            {
                printf("[allapot valt] CELZAS >> VEGE\n");
                printf("[allapot valt] P2 nyert\n");

                allapot = VEGE;
            }
            else if (p2_hp == 0)
            {
                printf("[allapot valt] CELZAS >> VEGE\n");
                printf("[allapot valt] P1 nyert\n");

                allapot = VEGE;
            }
            else
            {
                printf("[allapot valt] CELZAS >> LOVES_VOLT\n");

                sajat_celzas = nullptr;
                ellen_celzas = nullptr;

                allapot = LOVES_VOLT;
            }
        }

        if (allapot == HAJO_FELRAKAS && jatekon_kivuli_hajok() == 0)
        {
            printf("[allapot valt] masik jatekos\n");
            if (!p1_jatekban)
            {
                printf("[allapot valt] HAJO_FELRAKAS >> CELZAS\n");   
                allapot = CELZAS;
            }
            p1_jatekban = !p1_jatekban;
        }
    }

    void atad_kattintas()
    {
        if (allapot == LOVES_VOLT)
        {
            printf("[allapot valt] masik jatekos\n");
            printf("[allapot valt] LOVES_VOLT >> CELZAS\n");

            p1_jatekban = !p1_jatekban;
            allapot = CELZAS;
        }
    }

    bool elfer_forgatva(Hajo *h)
    {
        teglalap alak = h->alak();

        int uj_szel = alak.h;
        int uj_mag = alak.w;

        return alak.x + uj_szel <= 10 && alak.y + uj_mag <= 10 && !atfed(h, alak.x, alak.y, uj_szel, uj_mag);
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
            teglalap h_alak = h->alak();
            if (hajo != h)
            {
                bool atfed = 
                   x+szel > h_alak.x && h_alak.x+h_alak.w > x &&
                   y+mag  > h_alak.y && h_alak.y+h_alak.h > y;

                if (atfed) return true;
            }
        }
        return false;
    }

    void hajo_kattintas(Hajo *h, int btn)
    {
        if (btn == btn_left)
        {           
            kijel_hajo = h;
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
            if (allapot == HAJO_FELRAKAS && kijel_hajo)
            {
                teglalap alak = kijel_hajo->alak();
                x = std::min(x, 10-alak.w);
                y = std::min(y, 10-alak.h);
                if (!atfed(kijel_hajo, x, y, alak.w, alak.h))
                {
                    printf("[hajo mozgat ] %c%d\n", char(65+x), y+1);
                    kijel_hajo->mozgat(sajat_tf.x+x*cella_meret, sajat_tf.y+y*cella_meret, x, y);
                }
            }
        }
        else
        {
            if (allapot == CELZAS)
            {
                celzas.x = x;
                celzas.y = y;
                if (!sajat_celzas)
                {
                    printf("[celzas      ] %c%d\n", char(65+x), y+1);
                    uj_celzas(x, y);
                }
                else
                {
                    printf("[celzas atrak] %c%d\n", char(65+x), y+1);
                    celzast_mozgat(x, y);
                }
            }            
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