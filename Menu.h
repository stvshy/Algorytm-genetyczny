//
// Created by Mati on 28.12.2023.
//

#ifndef PEA_3_MENU_H
#define PEA_3_MENU_H

#include "Graf.h"
#include <iostream>

class Menu {

public:
    void zapisDoPliku(string nazwaPliku, vector<int> sciezka);

    Graf graf;

    void wczytajPlik(string nazwaPliku);
    int mainMenu();
};

#endif //PEA_3_MENU_H
