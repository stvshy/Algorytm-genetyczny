//
// Created by Mati on 28.12.2023.
//

#ifndef PEA_3_GRAF_H
#define PEA_3_GRAF_H


#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>

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
    void generujLosowo();
    int obliczKoszt(int skad, int dokad);
    void wyczysc();
//    void dodajKrawedz(int row, int value);
    void dodajKrawedz(int skad, int dokad, int koszt);
    void wypiszGraf();
    vector<vector<int>> macierzKosztow;

    void generujLosowo(int n);
};


#endif //PEA_3_GRAF_H
