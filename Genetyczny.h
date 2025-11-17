#ifndef PEA_3_GENETYCZNY_H
#define PEA_3_GENETYCZNY_H

#include <vector>
#include <random>
#include "Graf.h"

struct Osobnik {
    vector<int> chromosom;
    int koszt;
};
enum class MetodaSelekcji {
    TURNIEJOWA,
    RULETKOWA,
    RANKINGOWA
};

enum class MetodaKrzyzowania {
    OX, // Order Crossover
    PBX // Position-Based Crossover
};

enum class MetodaMutacji {
    ZAMIANA,    // Swap
    INWERSJA,   // Inversion
    WSTAWIANIE  // Insert
};
class Genetyczny {
private:
    vector<vector<int>> macierzKosztow;
    int wielkoscPopulacji; // wielkosc populacji

    double wspolczynnikMutacji; // wspolczynnik mutacji
    double wspolczynnikKrzyzowania; // wspolczynnik krzyzowania
    vector<Osobnik> populacja; // populacja osobnikow

    MetodaSelekcji wybranaMetodaSelekcji;
    MetodaKrzyzowania wybranaMetodaKrzyzowania;
    MetodaMutacji wybranaMetodaMutacji;

    Osobnik selekcjaTurniejowa();
    Osobnik selekcjaRuletkowa();
    Osobnik selekcjaRankingowa();

    void MutacjaZamiana(Osobnik &osobnik);
    void MutacjaInwersja(Osobnik &osobnik);
    void MutacjaWstawienie(Osobnik &osobnik);

    Osobnik KrzyzowanieOX(const Osobnik &tata, const Osobnik &mama);
    Osobnik KrzyzowaniePBX(const Osobnik &tata, const Osobnik &mama);

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
    Genetyczny(Graf& graf, int czas, int wielkoscPopulacji, double wspolczynnikMutacji, double wspolczynnikKrzyzowania,
               MetodaSelekcji metodaSelekcji, MetodaKrzyzowania metodaKrzyzowania, MetodaMutacji metodaMutacji);
    ~Genetyczny();
    void wypiszTrase(const vector<int> &sciezka);
    void algorytm(bool trybCichy = false);
};


#endif //PEA_3_GENETYCZNY_H
