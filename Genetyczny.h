#ifndef PEA_3_GENETYCZNY_H
#define PEA_3_GENETYCZNY_H

#include <vector>
#include <random>
#include "Graf.h"

struct Osobnik {
    vector<int> chromosom;
    int koszt;
};

class Genetyczny {
private:
    vector<vector<int>> macierzKosztow;
    Graf graf;
//    int rozmiar; // ile miast
    int wielkoscPopulacji; // wielkosc populacji
//    int metodaKrzyzowania;
    double wspolczynnikMutacji; // wspolczynnik mutacji
    double wspolczynnikKrzyzowania; // wspolczynnik krzyzowania
    vector<Osobnik> populacja; // chromosom
    // Po zmianie
    Osobnik selekcjaTurniejowa();
    Osobnik Mutacja(Osobnik osobnik);
    Osobnik KrzyzowanieOX(Osobnik &tata, Osobnik &mama);
//    Osobnik KrzyzowaniePBX(Osobnik &tata, Osobnik &mama);
    vector<Osobnik> wygenerujPopulacje();
    static bool porownajKoszty(const Osobnik &pierwsza, const Osobnik &druga);
    int obliczKoszt(const vector<int> &sciezka);
    int liczbaMiast;
    int czasTrwania;
    mt19937 generator;
public:
    double czasNajlepszegoRozwiazania;

    Osobnik najlepszeRozwiazanie; // najlepsze znalezione rozwiazanie
    Genetyczny();
    Genetyczny(Graf graf, int czas, int wielkoscPopulacji, double wspolczynnikMutacji, double wspolczynnikKrzyzowania);
    ~Genetyczny();
    void wypiszTrase(const vector<int> &sciezka);
    void algorytm();

};


#endif //PEA_3_GENETYCZNY_H
