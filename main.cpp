#include "application.hpp"
#include "jatekter.hpp"

using namespace genv;

class Torpedo : public Application
{
protected:
    Jatekter *jt;


public:
    Torpedo() 
    { 
        jt = new Jatekter(this, 15, 0, 600, [&](int x, int y, bool sajat){ jatekter_on_click(x, y, sajat); });
    }

    void jatekter_on_click(int x, int y, bool sajat)
    {
        if (sajat)
        {
            printf("[sajat terfel] %d, %d\n", x, y);
        }
        else
        {
            printf("[ellen terfel] %d, %d\n", x, y);
        }
    }

    void action(std::string s) { }
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