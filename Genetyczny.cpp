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
Genetyczny::Genetyczny(Graf g, int czas, int wielkoscPopulacji, double wspolczynnikMutacji, double wspolczynnikKrzyzowania) {
    macierzKosztow = g.macierzKosztow;
    liczbaMiast = g.liczbaMiast;
//    this->metodaKrzyzowania = metodaKrzyzowania;
    this->wielkoscPopulacji = wielkoscPopulacji;
    this->wspolczynnikMutacji = wspolczynnikMutacji;
    this->wspolczynnikKrzyzowania = wspolczynnikKrzyzowania;
    czasTrwania = czas;
    najlepszeRozwiazanie.koszt = INT_MAX;
    najlepszeRozwiazanie.chromosom.resize(g.liczbaMiast, -1);
    czasNajlepszegoRozwiazania = 0;
    random_device rd;
    generator.seed(rd());
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
    if (rozmiar < 2) return 0; // Zabezpieczenie przed pustą ścieżką
    for (int i = 0; i < rozmiar - 1; i++) {
        suma += macierzKosztow[sciezka[i]][sciezka[i + 1]];
    }
    suma += macierzKosztow[sciezka[rozmiar - 1]][sciezka[0]];

    return suma;
}

// Wyświetlanie trasy
void Genetyczny::wypiszTrase(vector<int> sciezka) {

    for (int i = 0; i < liczbaMiast; i++) {
        cout << sciezka[i] << "->";
    }
    cout << sciezka[0] << endl;
}

// Mutacja osobnika
Osobnik Genetyczny::Mutacja(Osobnik osobnik) {
    uniform_int_distribution<int> dist(0, liczbaMiast - 1);
    int pierwsza = dist(generator);
    int druga = dist(generator);

    // Prostszy sposób na zapewnienie, że nie są takie same
    if (pierwsza == druga) {
        druga = (pierwsza + 1) % liczbaMiast;
    }

    swap(osobnik.chromosom[pierwsza], osobnik.chromosom[druga]);
    osobnik.koszt = obliczKoszt(osobnik.chromosom);
    return osobnik;
}

//Order Crossover (OX)
Osobnik Genetyczny::KrzyzowanieOX(Osobnik &tata, Osobnik &mama) {
    Osobnik dziecko; // Inicjalizacja dziecka jako nowego osobnika
    int rozmiarRodzica = tata.chromosom.size(); // Pobranie rozmiaru chromosomu rodzica

    uniform_int_distribution<int> dist(0, rozmiarRodzica - 1);
    int punktPoczatkowy = dist(generator);
    int punktKoncowy = dist(generator);

    if (punktPoczatkowy > punktKoncowy) {
        swap(punktPoczatkowy, punktKoncowy); // Zamiana punktów początkowego i końcowego, jeśli początkowy jest większy od końcowego
    }

    dziecko.chromosom.resize(rozmiarRodzica); // Zmiana rozmiaru chromosomu dziecka na rozmiar rodzica (rezerwacja miejsca w pamięci)

    for (int i = punktPoczatkowy; i <= punktKoncowy; ++i) {
        dziecko.chromosom[i] = tata.chromosom[i]; // Kopiowanie fragmentu chromosomu taty do chromosomu dziecka
    }

    int indexDziecka = 0;
    for (int i = 0; i < rozmiarRodzica; ++i) {
        if (indexDziecka == punktPoczatkowy) {
            indexDziecka = punktKoncowy + 1; // Ustawienie indeksu dziecka na pozycję za końcem wstawianego fragmentu
        }

        if (find(dziecko.chromosom.begin(), dziecko.chromosom.end(), mama.chromosom[i]) == dziecko.chromosom.end()) {
            dziecko.chromosom[indexDziecka] = mama.chromosom[i]; // Wstawianie wartości z chromosomu mamy do dziecka, jeśli nie występują jeszcze w miejscu wstawiania
            ++indexDziecka;
        }
    }

    dziecko.koszt = obliczKoszt(dziecko.chromosom); // Obliczenie kosztu trasy dziecka
    return dziecko; // Zwrócenie nowego osobnika
}



// Generowanie początkowej populacji
vector<Osobnik> Genetyczny::wygenerujPopulacje() {
    vector<Osobnik> populacja;
    Osobnik p;

    for (int i = 0; i < wielkoscPopulacji; i++) {
        for (int j = 0; j < liczbaMiast; j++) {
            p.chromosom.push_back(j);
        }
        shuffle(p.chromosom.begin(), p.chromosom.end(), generator);
        p.koszt = obliczKoszt(p.chromosom);
        populacja.push_back(p);
        p.chromosom.clear();
    }

    return populacja;
}

// Selekcja osobników w populacji metodą proporcjonalną
// Nowa, poprawna wersja funkcji w Genetyczny.cpp
Osobnik Genetyczny::selekcjaTurniejowa() {
    int rozmiarTurnieju = 5;
    if (populacja.empty()) return Osobnik(); // Zabezpieczenie na wszelki wypadek

    // Używamy naszego generatora z klasy
    uniform_int_distribution<int> dist(0, populacja.size() - 1);

    // Pierwszy losowy uczestnik turnieju staje się tymczasowym zwycięzcą
    Osobnik najlepszyWTurnieju = populacja[dist(generator)];

    // Losujemy resztę uczestników i porównujemy
    for (int j = 1; j < rozmiarTurnieju; ++j) {
        Osobnik kandydat = populacja[dist(generator)];
        if (kandydat.koszt < najlepszyWTurnieju.koszt) {
            najlepszyWTurnieju = kandydat;
        }
    }
    // Zwracamy JEDNEGO zwycięzcę turnieju
    return najlepszyWTurnieju;
}
// Główny algorytm genetyczny
void Genetyczny::algorytm() {
    // srand() nie jest już potrzebne, bo używamy generatora mt19937
    // srand((unsigned)time(NULL));

    double czas = 0;
    clock_t start = clock();

    populacja = wygenerujPopulacje(); // Użyj zmiennej członkowskiej 'populacja'

    // Główna pętla ewolucji
    do {
        // 1. Stwórz pustą populację na następną generację
        vector<Osobnik> nastepnaPopulacja;

        // 2. Elitaryzm: Zachowaj najlepszego osobnika z obecnej populacji
        sort(populacja.begin(), populacja.end(), porownajKoszty);
        nastepnaPopulacja.push_back(populacja.front());

        // 3. Wypełnij resztę nowej populacji przez selekcję, krzyżowanie i mutację
        while (nastepnaPopulacja.size() < wielkoscPopulacji) {
            // Wybierz rodziców za pomocą selekcji turniejowej z obecnej populacji
            Osobnik tata = selekcjaTurniejowa();
            Osobnik mama = selekcjaTurniejowa();

            Osobnik dziecko;

            // Krzyżowanie z zadanym prawdopodobieństwem
            uniform_real_distribution<> dist(0.0, 1.0);
            if (dist(generator) < wspolczynnikKrzyzowania) {
                dziecko = KrzyzowanieOX(tata, mama);
            } else {
                // Jeśli nie ma krzyżowania, dziecko jest kopią jednego z rodziców
                dziecko = tata;
            }

            // Mutacja z zadanym prawdopodobieństwem
            if (dist(generator) < wspolczynnikMutacji) {
                dziecko = Mutacja(dziecko);
            }

            nastepnaPopulacja.push_back(dziecko);
        }

        // 4. Zastąp starą populację nową
        populacja = nastepnaPopulacja;

        // 5. Zaktualizuj najlepsze globalne rozwiązanie
        sort(populacja.begin(), populacja.end(), porownajKoszty);
        if (populacja.front().koszt < najlepszeRozwiazanie.koszt) {
            najlepszeRozwiazanie = populacja.front();
            czasNajlepszegoRozwiazania = (clock() - start) / (double)CLOCKS_PER_SEC;
            cout << "Nowy najlepszy koszt: " << najlepszeRozwiazanie.koszt
                 << " w czasie: " << czasNajlepszegoRozwiazania << "s" << endl;
        }

        czas = (clock() - start) / (double)CLOCKS_PER_SEC;

    } while (czas < czasTrwania);
}