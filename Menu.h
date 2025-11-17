#ifndef PEA_3_MENU_H
#define PEA_3_MENU_H

#include "Graf.h"
#include <iostream>
#include "Genetyczny.h"

class Menu {
private:
    int wielkoscPopulacji;
    int kryteriumStopu;
    double wspolczynnikMutacji;
    double wspolczynnikKrzyzowania;

    MetodaSelekcji wybranaMetodaSelekcji;
    MetodaKrzyzowania wybranaMetodaKrzyzowania;
    MetodaMutacji wybranaMetodaMutacji;

    string selekcjaToString(MetodaSelekcji metoda);
    string krzyzowanieToString(MetodaKrzyzowania metoda);
    string mutacjaToString(MetodaMutacji metoda);
public:
    Graf graf;
    Menu();
    int mainMenu();
    void wyswietlAktualneParametry();
};

#endif
