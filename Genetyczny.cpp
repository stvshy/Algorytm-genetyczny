#include "Genetyczny.h"
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <vector>
#include <numeric>
#include <random>
#include <unordered_set>

using namespace std;

Genetyczny::Genetyczny() {}

// Konstruktor
Genetyczny::Genetyczny(Graf g, int czas, int wielkoscPopulacji, double wspolczynnikMutacji, double wspolczynnikKrzyzowania, int metodaKrzyzowania) {
    macierzKosztow = g.macierzKosztow;
    liczbaMiast = g.liczbaMiast;
    this->metodaKrzyzowania = metodaKrzyzowania;
    this->wielkoscPopulacji = wielkoscPopulacji;
    this->wspolczynnikMutacji = wspolczynnikMutacji;
    this->wspolczynnikKrzyzowania = wspolczynnikKrzyzowania;
    czasTrwania = czas;
    najlepszeRozwiazanie.koszt = INT_MAX;
    najlepszeRozwiazanie.chromosom.resize(g.liczbaMiast, -1);
    czasNajlepszegoRozwiazania = 0;
}

// Destruktor
Genetyczny::~Genetyczny() {
    populacja.clear();
    macierzKosztow.clear();
    najlepszeRozwiazanie.chromosom.clear();
    najlepszeRozwiazanie.koszt = 0;
    czasNajlepszegoRozwiazania = 0;
}

// Funkcja pomocnicza do porównywania kosztów osobników
bool Genetyczny::porownajKoszty(const Osobnik &pierwsza, const Osobnik &druga) {
    return (pierwsza.koszt < druga.koszt);
}

// Obliczanie kosztu trasy
int Genetyczny::obliczKoszt(vector<int> &sciezka) {
    int suma = 0;
    int rozmiar = sciezka.size();

    for (int i = 0; i < rozmiar - 1; i++) {
        suma += macierzKosztow[sciezka[i]][sciezka[i + 1]];
    }
    suma += macierzKosztow[sciezka[rozmiar - 1]][sciezka[0]];

    return suma;
}

// Wyœwietlanie trasy
void Genetyczny::wypiszTrase(vector<int> sciezka) {

    for (int i = 0; i < liczbaMiast; i++) {
        cout << sciezka[i] << "->";
    }
    cout << sciezka[0] << endl;
}

// Mutacja osobnika
Osobnik Genetyczny::Mutacja(Osobnik osobnik) {

    int pierwsza, druga;

    do {
        pierwsza = rand() % liczbaMiast;
        druga = rand() % liczbaMiast;
    } while (pierwsza == druga);

    swap(osobnik.chromosom[pierwsza], osobnik.chromosom[druga]);

    osobnik.koszt = obliczKoszt(osobnik.chromosom);

    return osobnik;
}

//Order Crossover (OX)
Osobnik Genetyczny::KrzyzowanieOX(Osobnik &tata, Osobnik &mama) {
    Osobnik dziecko; // Inicjalizacja dziecka jako nowego osobnika
    int rozmiarRodzica = tata.chromosom.size(); // Pobranie rozmiaru chromosomu rodzica
    int punktPoczatkowy = rand() % rozmiarRodzica; // Losowanie punktu pocz¹tkowego krzy¿owania
    int punktKoncowy = rand() % rozmiarRodzica; // Losowanie punktu koñcowego krzy¿owania

    if (punktPoczatkowy > punktKoncowy) {
        swap(punktPoczatkowy, punktKoncowy); // Zamiana punktów pocz¹tkowego i koñcowego, jeœli pocz¹tkowy jest wiêkszy od koñcowego
    }

    dziecko.chromosom.resize(rozmiarRodzica); // Zmiana rozmiaru chromosomu dziecka na rozmiar rodzica (rezerwacja miejsca w pamiêci)

    for (int i = punktPoczatkowy; i <= punktKoncowy; ++i) {
        dziecko.chromosom[i] = tata.chromosom[i]; // Kopiowanie fragmentu chromosomu taty do chromosomu dziecka
    }

    int indexDziecka = 0;
    for (int i = 0; i < rozmiarRodzica; ++i) {
        if (indexDziecka == punktPoczatkowy) {
            indexDziecka = punktKoncowy + 1; // Ustawienie indeksu dziecka na pozycjê za koñcem wstawianego fragmentu
        }

        if (find(dziecko.chromosom.begin(), dziecko.chromosom.end(), mama.chromosom[i]) == dziecko.chromosom.end()) {
            dziecko.chromosom[indexDziecka] = mama.chromosom[i]; // Wstawianie wartoœci z chromosomu mamy do dziecka, jeœli nie wystêpuj¹ jeszcze w miejscu wstawiania
            ++indexDziecka;
        }
    }

    dziecko.koszt = obliczKoszt(dziecko.chromosom); // Obliczenie kosztu trasy dziecka
    return dziecko; // Zwrócenie nowego osobnika
}


//Position-based crossover (PBX)
Osobnik Genetyczny::KrzyzowaniePBX(Osobnik &tata, Osobnik &mama) {
    int rozmiarRodzica = tata.chromosom.size();
    Osobnik dziecko;
    dziecko.chromosom.resize(rozmiarRodzica, -1); // Inicjalizacja chromosomu dziecka z wartoœciami -1

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, rozmiarRodzica - 1); // Inicjalizacja generatora liczb losowych

    int iloscPunktow = dis(gen); // Losowanie iloœci punktów do krzy¿owania

    unordered_set<int> wybraneWartosci; // Utworzenie zbioru przechowuj¹cego wybrane wartoœci z chromosomu matki
    while (wybraneWartosci.size() < iloscPunktow) {
        int randomIndex = dis(gen);
        wybraneWartosci.insert(mama.chromosom[randomIndex]); // Wybór losowych wartoœci z chromosomu matki
    }

    for (int punkt : wybraneWartosci) {
        auto index = find(tata.chromosom.begin(), tata.chromosom.end(), punkt);
        dziecko.chromosom[index - tata.chromosom.begin()] = punkt; // Wstawianie wybranych wartoœci z matki do dziecka na tych samych pozycjach
    }

    vector<int> dostepnePozycje;
    for (int i = 0; i < rozmiarRodzica; ++i) {
        if (dziecko.chromosom[i] == -1) {
            dostepnePozycje.push_back(i); // Zbieranie pozycji w chromosomie dziecka, gdzie nie ma jeszcze wartoœci
        }
    }

    int index = 0;
    for (int i = 0; i < rozmiarRodzica; ++i) {
        if (wybraneWartosci.find(mama.chromosom[i]) == wybraneWartosci.end()) {
            if (index < dostepnePozycje.size()) {
                dziecko.chromosom[dostepnePozycje[index]] = mama.chromosom[i]; // Wstawianie wartoœci z chromosomu matki do wolnych miejsc w chromosomie dziecka
                ++index;
            } else {
                break; // Przerwanie, jeœli osi¹gniêto maksymalny rozmiar dziecka
            }
        }
    }
    dziecko.koszt = obliczKoszt(dziecko.chromosom); // Obliczenie kosztu trasy dziecka
    return dziecko; // Zwrócenie nowego osobnika
}


// Generowanie pocz¹tkowej populacji
vector<Osobnik> Genetyczny::wygenerujPopulacje() {
    vector<Osobnik> populacja;
    Osobnik p;

    for (int i = 0; i < wielkoscPopulacji; i++) {
        for (int j = 0; j < liczbaMiast; j++) {
            p.chromosom.push_back(j);
        }
        random_shuffle(p.chromosom.begin(), p.chromosom.end());
        p.koszt = obliczKoszt(p.chromosom);
        populacja.push_back(p);
        p.chromosom.clear();
    }

    return populacja;
}

// Selekcja osobników w populacji metod¹ proporcjonaln¹
void Genetyczny::Selekcja(vector<Osobnik> &populacja) {
    // Sortowanie populacji wzglêdem kosztu
    sort(populacja.begin(), populacja.end(), porownajKoszty);



    int rozmiarPopulacji = populacja.size();

    // Tworzenie tablicy zawieraj¹cej wartoœci odwrotnych rang
    vector<double> odwroconeRangi;
    for (int i = 0; i < rozmiarPopulacji; ++i) {                // (N = 100)
        odwroconeRangi.push_back(double(rozmiarPopulacji - i)); //100, 99, 98...
    }

    // Tworzenie tablicy zawieraj¹cej prawdopodobieñstwa wyboru dla poszczególnych osobników
    vector<double> prawdopodobienstwa;

    // Tworzenie nowej populacji na podstawie prawdopodobieñstw wyboru
    vector<Osobnik> nowaPopulacja;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);

    for (size_t i = 0; i < rozmiarPopulacji; ++i) {
        double losowaWartosc = dis(gen); // Losowa wartoœæ od 0 do 1

        // Wybieranie osobnika z populacji na podstawie wartoœci losowej i prawdopodobieñstw wyboru
        for (size_t j = 0; j < rozmiarPopulacji; ++j) {
            if (losowaWartosc <= odwroconeRangi[j]/rozmiarPopulacji) {
                nowaPopulacja.push_back(populacja[j]); // Dodanie wybranego osobnika do nowej populacji
                break;
            }
        }
    }

    // Przypisanie nowej populacji do populacji pierwotnej
    populacja = nowaPopulacja;
}

// G³ówny algorytm genetyczny
void Genetyczny::algorytm() {
    srand((unsigned)time(NULL));

    double czas = 0;
    clock_t start = clock();

    vector<Osobnik> populacja = wygenerujPopulacje();
    Osobnik dziecko;
    do {
        Selekcja(populacja);

        while(populacja.size()<wielkoscPopulacji) {
            int x, y;
            do {
                x = rand() % wielkoscPopulacji;
                y = rand() % wielkoscPopulacji;
            } while (x == y);

            if (double(rand()) / (double(RAND_MAX)) < wspolczynnikKrzyzowania) {
                if (metodaKrzyzowania == 1){
                    dziecko = KrzyzowanieOX(populacja[x], populacja[y]);
                    populacja.push_back(dziecko);
                }
                else {
                    dziecko = KrzyzowaniePBX(populacja[x], populacja[y]);
                    populacja.push_back(dziecko);
                }
                x++, y++;
            }
        }

        for (int j = 0; j < populacja.size(); j++) {
            if ((double)rand() / RAND_MAX < wspolczynnikMutacji) {
                populacja[j] = Mutacja(populacja[j]);
            }
        }

        sort(populacja.begin(), populacja.end(), porownajKoszty);

        if (populacja.front().koszt < najlepszeRozwiazanie.koszt) {
            najlepszeRozwiazanie.koszt = populacja.front().koszt;
            cout << najlepszeRozwiazanie.koszt <<" " << (clock() - start) / (double)CLOCKS_PER_SEC << endl;
            najlepszeRozwiazanie.chromosom = populacja.front().chromosom;
            czasNajlepszegoRozwiazania = (clock() - start) / (double)CLOCKS_PER_SEC;
        }

        czas = (clock() - start) / (double)CLOCKS_PER_SEC;

    } while(czas < czasTrwania);
}
