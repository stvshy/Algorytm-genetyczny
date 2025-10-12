#include "Genetyczny.h"
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <vector>
#include <numeric>
#include <random>
#include <unordered_set>
#include <optional>

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
    najlepszeRozwiazanie.koszt = INT_MAX; // Maksmymalna wartość na start, Osobnik to chromosom (trasa) + koszt (całej trasy)
    najlepszeRozwiazanie.chromosom.resize(g.liczbaMiast, -1); // Inicjalizacja chromosomu, resize do liczby miast, wypełnienie -1
    czasNajlepszegoRozwiazania = 0; // Inicjalizacja czasu (start od 0)
    random_device rd; // Pobierz losowe ziarno z systemu (za każdym uruchomieniem programu inne)
    generator.seed(rd()); // Ustaw to ziarno w generatorze, żeby nie generował tych samych liczb co ostatnio
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
bool Genetyczny::porownajKoszty(const Osobnik &pierwsza, const Osobnik &druga) { // dzięki referencji unikamy kopiowania całych osobników (oszczędność pamięci i czasu), const bo nie zmieniamy ich
    return (pierwsza.koszt < druga.koszt);
}

// Obliczanie kosztu trasy
int Genetyczny::obliczKoszt(const vector<int> &sciezka) {
    int suma = 0;
    int rozmiar = sciezka.size();
    if (rozmiar < 2) return 0; // Zabezpieczenie przed pustą ścieżką
    for (int i = 0; i < rozmiar - 1; i++) {
        suma += macierzKosztow[sciezka[i]][sciezka[i + 1]];  // Dodaj koszt przejścia z miasta i do i+1
    }
    suma += macierzKosztow[sciezka[rozmiar - 1]][sciezka[0]]; // Dodaj koszt powrotu do miasta startowego z ostatniego miasta

    return suma;
}

// Wyświetlanie trasy
void Genetyczny::wypiszTrase(const vector<int> &sciezka) {

    for (int i = 0; i < liczbaMiast; i++) {
        cout << sciezka[i] << "->";
    }
    cout << sciezka[0] << endl;
}

// Mutacja osobnika
Osobnik Genetyczny::Mutacja(Osobnik osobnik) {
    uniform_int_distribution<int> dist(0, liczbaMiast - 1); // Narzędzie do losowania liczb z zakresu [0, liczbaMiast-1], gdzie każda liczba ma równą szansę
    int pierwsza = dist(generator); // Losowanie pierwszego indeksu do zamiany z zakresem od 0 do liczbaMiast-1
    int druga = dist(generator); // Losowanie drugiego indeksu do zamiany z zakresem od 0 do liczbaMiast-1

    // sposób na zapewnienie, że nie są takie same
    if (pierwsza == druga) {
        druga = (pierwsza + 1) % liczbaMiast; // jeśli są takie same, przesuwamy drugi indeks o 1 (zawijając się do 0, jeśli przekroczy liczbę miast)
    }

    swap(osobnik.chromosom[pierwsza], osobnik.chromosom[druga]);
    osobnik.koszt = obliczKoszt(osobnik.chromosom);
    return osobnik;
}

//Order Crossover (OX)
Osobnik Genetyczny::KrzyzowanieOX(Osobnik &tata, Osobnik &mama) {
    Osobnik dziecko;
    dziecko.chromosom.resize(liczbaMiast);

    // Krok 1: Wybieramy losowe punkty fragmentu do wycięcia
    uniform_int_distribution<int> dist(0, liczbaMiast - 1);
    int punktPoczatkowy = dist(generator);
    int punktKoncowy = dist(generator);

    if (punktPoczatkowy > punktKoncowy) {
        swap(punktPoczatkowy, punktKoncowy);
    }

    // Krok 2: Tworzymy pomocniczą tablicę do śledzenia użytych genów (miast)
    vector<bool> uzyteGeny(liczbaMiast, false); // Tworzy wektor flag (początkowo false) o rozmiarze liczbaMiast
    // Indeksy w uzyteGeny odpowiadają miejscom (genom) w chromosomie, np. 'uzyteGeny[3] = true' oznacza, że miasto 3 zostało już użyte w dziecku

    // Krok 3: Kopiujemy fragment chromosomu z taty do dziecka
    // i oznaczamy skopiowane geny jako użyte
    for (int i = punktPoczatkowy; i <= punktKoncowy; ++i) {
        int gen = tata.chromosom[i];
        dziecko.chromosom[i] = gen;
        uzyteGeny[gen] = true; // Oznaczamy gen (miasto) jako użyty, np. uzyteGeny = {miasto0: false, miasto1: true, miasto2: false, ...}
    }

    // Krok 4: Uzupełnij resztę chromosomu dziecka genami z mamy zaczynając od punktu końcowego
    int indeksDziecka = (punktKoncowy + 1) % liczbaMiast; // % operator modulo zapewnia zawijanie do początku chromosomu jak przekroczymy jego długość
    int indeksMamy = (punktKoncowy + 1) % liczbaMiast;

    // Przejdź przez chromosom mamy, aby znaleźć brakujące geny
    while (indeksDziecka != punktPoczatkowy) {
        int genMamy = mama.chromosom[indeksMamy];

        // Jeśli gen z mamy nie był jeszcze użyty, dodaj go do dziecka
        if (!uzyteGeny[genMamy]) {
            dziecko.chromosom[indeksDziecka] = genMamy;
            uzyteGeny[genMamy] = true;
            indeksDziecka = (indeksDziecka + 1) % liczbaMiast;
        }

        // Zawsze przechodź do następnego genu w chromosomie mamy
        indeksMamy = (indeksMamy + 1) % liczbaMiast;
    }

    dziecko.koszt = obliczKoszt(dziecko.chromosom);
    return dziecko;
}


// Generowanie początkowej populacji
vector<Osobnik> Genetyczny::wygenerujPopulacje() { // Populacja początkowa - zbiór losowych tras, które będziemy optymalizować
// Ocena tras zależy od danych z pliku (macierz kosztów)
    vector<Osobnik> populacja;
    Osobnik p;

    for (int i = 0; i < wielkoscPopulacji; i++) {
        for (int j = 0; j < liczbaMiast; j++) {
            p.chromosom.push_back(j);
        }
        shuffle(p.chromosom.begin(), p.chromosom.end(), generator); // Mieszamy trasę losowo używając naszego generatora z klasy
        p.koszt = obliczKoszt(p.chromosom);
        populacja.push_back(p);
        p.chromosom.clear();
    }

    return populacja;
}

// Selekcja osobników w populacji metodą turniejową
Osobnik Genetyczny::selekcjaTurniejowa() {
    int rozmiarTurnieju = 5; // Liczba uczestników w turnieju
    if (populacja.empty()) return Osobnik(); // zwracamy pustego osobnika jeśli populacja jest pusta

    // Używamy naszego generatora z klasy
    uniform_int_distribution<int> dist(0, populacja.size() - 1);  // Narzędzie do losowania indeksów z zakresu populacji

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

    double czas = 0;
    clock_t start = clock(); // Pobierz czas startu algorytmu

    populacja = wygenerujPopulacje(); // Inicjalizacja populacji początkowej

    // Główna pętla ewolucji
    do {
        // 1. Stwórz pustą populację na następną generację
        vector<Osobnik> nastepnaPopulacja;

        // 2. Elitaryzm: Zachowaj najlepszego osobnika z obecnej populacji
        sort(populacja.begin(), populacja.end(), porownajKoszty); // Sortujemy populację według kosztów (najlepszy na początku)
        nastepnaPopulacja.push_back(populacja.front());

        // 3. Wypełnij resztę nowej populacji przez selekcję, krzyżowanie i mutację
        while (nastepnaPopulacja.size() < wielkoscPopulacji) {
            // Wybierz rodziców za pomocą selekcji turniejowej z obecnej populacji
            Osobnik tata = selekcjaTurniejowa();
            Osobnik mama = selekcjaTurniejowa();

            Osobnik dziecko;

            // Krzyżowanie z zadanym prawdopodobieństwem
            uniform_real_distribution<> dist(0.0, 1.0); // Narzędzie do losowania liczb zmiennoprzecinkowych z zakresu [0.0, 1.0)
            if (dist(generator) < wspolczynnikKrzyzowania) {
                dziecko = KrzyzowanieOX(tata, mama);
            } else {
                // Jeśli nie ma krzyżowania, dziecko jest kopią jednego z rodziców
                dziecko = tata;
            }

            // Mutacja z zadanym prawdopodobieństwem
            if (dist(generator) < wspolczynnikMutacji) { // losowa liczba z zakresu [0.0, 1.0)
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