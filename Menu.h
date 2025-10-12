#ifndef PEA_3_MENU_H
#define PEA_3_MENU_H

#include "Graf.h"
#include <iostream>

class Menu {
private:
    int wielkoscPopulacji;
    int kryteriumStopu;
    double wspolczynnikMutacji;
    double wspolczynnikKrzyzowania;
public:
    Graf graf;
    Menu();
    int mainMenu();
    void wyswietlAktualneParametry();
};

#endif
