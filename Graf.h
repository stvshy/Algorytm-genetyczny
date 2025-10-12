//
// Created by Mati on 28.12.2023.
//

#ifndef PEA_3_GRAF_H
#define PEA_3_GRAF_H


#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
using namespace std;

class Graf
{
private:
    //
public:
    int liczbaMiast;

    Graf();
    ~Graf();
    void wczytajPlik();
    void wczytajPlik(const string& nazwaPliku);
    vector<vector<int>> macierzKosztow;
    string nazwaZaladowanegoPliku;
};


#endif //PEA_3_GRAF_H
