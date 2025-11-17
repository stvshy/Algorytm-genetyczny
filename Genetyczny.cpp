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

// Konstruktor
Genetyczny::Genetyczny(Graf& g, int czas, int wielkoscPopulacji, double wspolczynnikMutacji, double wspolczynnikKrzyzowania,
                       MetodaSelekcji metodaSelekcji, MetodaKrzyzowania metodaKrzyzowania, MetodaMutacji metodaMutacji) {
    macierzKosztow = g.macierzKosztow;
    liczbaMiast = g.liczbaMiast;
//    this->metodaKrzyzowania = metodaKrzyzowania;
    this->wielkoscPopulacji = wielkoscPopulacji;
    this->wspolczynnikMutacji = wspolczynnikMutacji;
    this->wspolczynnikKrzyzowania = wspolczynnikKrzyzowania;
    czasTrwania = czas;
    this->wybranaMetodaSelekcji = metodaSelekcji;
    this->wybranaMetodaKrzyzowania = metodaKrzyzowania;
    this->wybranaMetodaMutacji = metodaMutacji;
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
void Genetyczny::MutacjaZamiana(Osobnik &osobnik) {
    uniform_int_distribution<int> dist(0, liczbaMiast - 1); // Narzędzie do losowania liczb z zakresu [0, liczbaMiast-1], gdzie każda liczba ma równą szansę
    int pierwsza = dist(generator); // Losowanie pierwszego indeksu do zamiany z zakresem od 0 do liczbaMiast-1
    int druga = dist(generator); // Losowanie drugiego indeksu do zamiany z zakresem od 0 do liczbaMiast-1

    swap(osobnik.chromosom[pierwsza], osobnik.chromosom[druga]);
    osobnik.koszt = obliczKoszt(osobnik.chromosom);
}
void Genetyczny::MutacjaInwersja(Osobnik &osobnik) {
    uniform_int_distribution<int> dist(0, liczbaMiast - 1); // Narzędzie do losowania liczb z zakresu [0, liczbaMiast-1]
    int punktPoczatkowy = dist(generator);
    int punktKoncowy = dist(generator);

    if (punktPoczatkowy > punktKoncowy) {
        swap(punktPoczatkowy, punktKoncowy);
    }

    // Odwróć fragment chromosomu
    reverse(osobnik.chromosom.begin() + punktPoczatkowy, osobnik.chromosom.begin() + punktKoncowy + 1); // .begin() to iterator na początek wektora, więc dodajemy indeksy, aby uzyskać odpowiednie pozycje
    //  punktKoncowy + 1, ponieważ reverse działa na przedziale [first, last), gdzie last jest wyłączone

    osobnik.koszt = obliczKoszt(osobnik.chromosom);
}

void Genetyczny::MutacjaWstawienie(Osobnik &osobnik) {
    uniform_int_distribution<int> dist(0, liczbaMiast - 1); // Narzędzie do losowania liczb z zakresu [0, liczbaMiast-1]
    int indeksDoPrzeniesienia = dist(generator); // Losowanie indeksu do przeniesienia
    int indeksDocelowy = dist(generator); // Losowanie docelowego indeksu

    // Krok 1: Zapamiętaj i usuń element
    int miasto = osobnik.chromosom[indeksDoPrzeniesienia]; // Zapamiętanie miasta do przeniesienia
    osobnik.chromosom.erase(osobnik.chromosom.begin() + indeksDoPrzeniesienia); // Usunięcie miasta z oryginalnej pozycji

    // Krok 2: Wstaw element na nowej pozycji
    osobnik.chromosom.insert(osobnik.chromosom.begin() + indeksDocelowy, miasto); // Wstawienie miasta na nową pozycję

    osobnik.koszt = obliczKoszt(osobnik.chromosom);
}
//Order Crossover (OX)
Osobnik Genetyczny::KrzyzowanieOX(const Osobnik &tata, const Osobnik &mama) {
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
        uzyteGeny[gen] = true; // Oznaczamy gen (miasto) jako użyty, indeks tego wektora odpowiada numerowi miasta, np. uzyteGeny = {miasto0: false, miasto1: true, miasto2: false, ...}
    } // w uzyteGeny chodzi nie o sprawdzenie czy pozycja jest zajęta, ale czy dane miasto już występuje w dziecku bo tata i mama mogą mieć różne miasta na tych samych pozycjach

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
// Position-Based Crossover (PBX)
Osobnik Genetyczny::KrzyzowaniePBX(const Osobnik &tata, const Osobnik &mama) {
    Osobnik dziecko;
    dziecko.chromosom.resize(liczbaMiast, -1); // Wypełnij -1, żeby łatwo znaleźć puste miejsca

    // Krok 1: Wylosuj zbiór pozycji z taty
    uniform_int_distribution<int> dist_liczba(1, liczbaMiast - 1); // Tworzymy dystrybucję do losowania liczby genów do skopiowania
    int liczbaPozycji = dist_liczba(generator); // Losujemy, ile genów weźmiemy od taty

    unordered_set<int> pozycjeTaty; // Używamy unordered_set, żeby uniknąć duplikatów
    uniform_int_distribution<int> dist_pozycja(0, liczbaMiast - 1); // Dystrybucja do losowania pozycji w chromosomie taty
    while (pozycjeTaty.size() < liczbaPozycji) {
        pozycjeTaty.insert(dist_pozycja(generator));
    }

    // Krok 2: Skopiuj wybrane geny z taty do dziecka na tych samych pozycjach
    vector<bool> uzyteGeny(liczbaMiast, false); // Tablica booli do śledzenia użytych genów
    for (int pozycja : pozycjeTaty) {
        int gen = tata.chromosom[pozycja]; // Pobierz gen z taty (czyli miasto)
        dziecko.chromosom[pozycja] = gen; // Skopiuj gen do dziecka na tej samej pozycji
        uzyteGeny[gen] = true; // Oznaczamy gen (miasto) jako użyty, indeks tego wektora odpowiada numerowi miasta, np. uzyteGeny = {miasto0: false, miasto1: true, miasto2: false, ...}
    } // w uzyteGeny chodzi nie o sprawdzenie czy pozycja jest zajęta, ale czy dane miasto już występuje w dziecku bo tata i mama mogą mieć różne miasta na tych samych pozycjach

    // Krok 3: Uzupełnij resztę chromosomu dziecka genami z mamy
    int indeksMamy = 0;                      // Zaczynamy od początku chromosomu mamy
    for (int i = 0; i < liczbaMiast; ++i) {  // Przechodzimy przez cały chromosom dziecka od początku
        if (dziecko.chromosom[i] == -1) { // Jeśli to miejsce jest puste
            while (uzyteGeny[mama.chromosom[indeksMamy]]) { // Dopóki gen z mamy jest już użyty (bool)
                indeksMamy++; // Przechodzimy do następnego genu w chromosomie mamy
            }
            dziecko.chromosom[i] = mama.chromosom[indeksMamy]; // Wstaw ten gen do dziecka (bo jeszcze go nie ma)
            indeksMamy++;
        }
    }

    dziecko.koszt = obliczKoszt(dziecko.chromosom);
    return dziecko;
}

// Generowanie początkowej populacji
vector<Osobnik> Genetyczny::wygenerujPopulacje() {
    vector<Osobnik> nowaPopulacja;
    nowaPopulacja.reserve(wielkoscPopulacji); // Rezerwacja pamięci, by uniknąć realokacji podczas dodawania po kolei nowych osobników przez push_back

    vector<int> bazowyChromosom(liczbaMiast);
    iota(bazowyChromosom.begin(), bazowyChromosom.end(), 0); // Wypełnia wektor wartościami 0, 1, 2, ...

    for (int i = 0; i < wielkoscPopulacji; i++) { // Tworzenie każdego osobnika populacji
        Osobnik p; // Osobnik to chromosom (jedna trasa) + koszt (całej trasy)
        p.chromosom = bazowyChromosom; // Kopiujemy bazowy chromosom
        shuffle(p.chromosom.begin(), p.chromosom.end(), generator); // Mieszamy chromosom, aby uzyskać losową trasę
        p.koszt = obliczKoszt(p.chromosom);
        nowaPopulacja.push_back(p);
    }

    return nowaPopulacja;
}

// Selekcja osobników w populacji metodą turniejową
Osobnik Genetyczny::selekcjaTurniejowa() {
    int rozmiarTurnieju = 5; // Liczba uczestników w turnieju
    if (populacja.empty()) return Osobnik(); // zwracamy pustego osobnika jeśli populacja jest pusta

    // Używamy naszego generatora z klasy
    uniform_int_distribution<int> dist(0, populacja.size() - 1);  // Narzędzie do losowania indeksów z zakresu populacji, -1 bo indeksy od 0

    // Pierwszy losowy uczestnik turnieju staje się tymczasowym zwycięzcą
    int najlepszyIndeks = dist(generator);

    // Losujemy resztę uczestników i porównujemy
    for (int j = 1; j < rozmiarTurnieju; ++j) { // zaczynamy od 1 bo pierwszy już wylosowany
        int kandydatIndeks = dist(generator); // Losowanie indeksu kandydata
        if (populacja[kandydatIndeks].koszt < populacja[najlepszyIndeks].koszt) { // Porównanie kosztów z najlepszym dotychczas
            najlepszyIndeks = kandydatIndeks; // Aktualizacja najlepszego indeksu
        }
    }
    // Zwracamy kopię tylko raz, na samym końcu
    return populacja[najlepszyIndeks];
}

Osobnik Genetyczny::selekcjaRuletkowa() {
    if (populacja.empty()) return Osobnik();

    // Krok 1: Przekształć koszty na wartości dopasowania (fitness)
    // Chcemy, aby niższy koszt oznaczał wyższy fitness.
    // Znajdź najgorszy (najwyższy) koszt w populacji
    double maxKoszt = 0;
    for (const Osobnik& osobnik  : populacja) { // Znajdź maksymalny koszt w populacji
        if (osobnik.koszt > maxKoszt) {
            maxKoszt = osobnik.koszt;
        }
    }

    vector<double> fitnessy; // Wektor do przechowywania wartości fitness każdego osobnika
    double sumaFitness = 0;
    for (const Osobnik& osobnik : populacja) { // Przekształcenie kosztu na fitness
        // Dodajemy 1, aby uniknąć dzielenia przez zero i dać szansę najgorszemu osobnikowi
        double fitness = (maxKoszt - osobnik.koszt) + 1;
        fitnessy.push_back(fitness);
        sumaFitness += fitness; // sumaFitness to suma wszystkich wartości fitness, czyli obwód całego naszego koła ruletki
        // Osobnik z najgorszym kosztem 1000 dostanie fitness: (1000 - 1000) + 1 = 1
    } // Osobnik z najlepszym kosztem, np. 200, dostanie fitness: (1000 - 200) + 1 = 801
      // Szansa wyboru najlepszego byłaby więc 801 / sumaFitness, a najgorszego 1 / sumaFitness

    // Krok 2: Wylosuj punkt na "kole ruletki"
    uniform_real_distribution<double> dist(0.0, sumaFitness); // Losowanie liczby z zakresu od 0 do sumaFitness
    double wylosowanaWartosc = dist(generator); // // To jest nasza "kulka", która wylądowała w jakimś punkcie na obwodzie koła.

    // Krok 3: Znajdź osobnika, na którego trafił wskaźnik
    double bieżącaSuma = 0;
    for (size_t i = 0; i < populacja.size(); ++i) { // size_t bo lepszy typ dla indeksów wektorów, a populacja.size() zwraca size_t (mniej konwersji typów)
        bieżącaSuma += fitnessy[i];
        if (bieżącaSuma >= wylosowanaWartosc) { // Jeśli bieżąca suma przekroczyła wylosowaną wartość, to znaczy, że kulka zatrzymała się na tym osobniku
            return populacja[i];
        }
    }

    // Zabezpieczenie na wypadek błędów numerycznych
    return populacja.back();
}
Osobnik Genetyczny::selekcjaRankingowa() {  // Ten kod działa pod kluczowym założeniem, że populacja jest już posortowana od najlepszego (indeks 0) do najgorszego.
    if (populacja.empty()) return Osobnik();

    long long sumaRang = (long long)populacja.size() * (populacja.size() + 1) / 2; // Szybszy sposób liczenia sumy rang - formuła n(n+1)/2 [wzór na sumę ciągu arytmetycznego]

    if (sumaRang == 0) return populacja.front(); // Zabezpieczenie przed dzieleniem przez zero - jeśli suma rang jest 0, zwróć pierwszego osobnika

    uniform_int_distribution<long long> dist(1, sumaRang); // Losowanie liczby z zakresu od 1 do sumaRang
    long long wylosowanaWartosc = dist(generator); // Ta liczba to nasz "cel", punkt na "kole rankingowym"

    long long bieżącaSuma = 0; // Bieżąca suma rang podczas iteracji
    for (size_t i = 0; i < populacja.size(); ++i) { // size_t bo lepszy typ dla indeksów wektorów, a populacja.size() zwraca size_t (mniej konwersji typów)
        bieżącaSuma += (populacja.size() - i); // Najlepszy (indeks 0) ma największą rangę - [i=0 (Najlepszy) - ranga N] vs [i = N-1 (Najgorszy) - ranga 1)
        if (bieżącaSuma >= wylosowanaWartosc) { // Jeśli bieżąca suma przekroczyła wylosowaną wartość, to znaczy, że kulka zatrzymała się na tym wylosowanym osobniku
            return populacja[i];
        }
    }
    return populacja.back();
}
// Główny algorytm genetyczny
void Genetyczny::algorytm(bool trybCichy) {
    double czas = 0;
    clock_t start = clock(); // Pobierz czas startu algorytmu

    populacja = wygenerujPopulacje(); // Inicjalizacja populacji
    sort(populacja.begin(), populacja.end(), porownajKoszty); // Sortowanie populacji dla pierwszej iteracji

    do {
        vector<Osobnik> nastepnaPopulacja; // Tworzymy nową populację na ten cykl

        // 1. Elitaryzm (populacja jest już posortowana z poprzedniej pętli)
        nastepnaPopulacja.push_back(populacja.front()); // Dodajemy najlepszego osobnika z poprzedniej populacji do nowej

        while (nastepnaPopulacja.size() < wielkoscPopulacji) { // Dopóki nowa populacja nie osiągnie wymaganej wielkości
            Osobnik tata, mama;
            switch (wybranaMetodaSelekcji) { // Wybór metody selekcji
                case MetodaSelekcji::TURNIEJOWA:
                    tata = selekcjaTurniejowa();
                    mama = selekcjaTurniejowa();
                    break;
                case MetodaSelekcji::RULETKOWA:
                    tata = selekcjaRuletkowa();
                    mama = selekcjaRuletkowa();
                    break;
                case MetodaSelekcji::RANKINGOWA:
                    tata = selekcjaRankingowa();
                    mama = selekcjaRankingowa();
                    break;
            }

            Osobnik dziecko;
            uniform_real_distribution<> dist(0.0, 1.0); // Dystrybucja do losowania liczb zmiennoprzecinkowych z zakresu [0.0, 1.0)

            // Wybór metody krzyżowania
            if (dist(generator) < wspolczynnikKrzyzowania) { //  Jeśli wylosowana liczba jest mniejsza niż współczynnik krzyżowania, wykonaj krzyżowanie
                switch (wybranaMetodaKrzyzowania) {
                    case MetodaKrzyzowania::OX:
                        dziecko = KrzyzowanieOX(tata, mama);
                        break;
                    case MetodaKrzyzowania::PBX:
                        dziecko = KrzyzowaniePBX(tata, mama);
                        break;
                }
            } else {
                dziecko = tata; // Jeśli nie krzyżujemy, dziecko jest kopią jednego z rodziców (taty)
            }

            if (dist(generator) < wspolczynnikMutacji) { // Kolejny raz losujemy inną liczbe i jeśli jest mniejsza niż współczynnik mutacji, wykonaj mutację
                switch (wybranaMetodaMutacji) {  // Wybór metody mutacji
                    case MetodaMutacji::ZAMIANA:
                        MutacjaZamiana(dziecko);
                        break;
                    case MetodaMutacji::INWERSJA:
                        MutacjaInwersja(dziecko);
                        break;
                    case MetodaMutacji::WSTAWIANIE:
                        MutacjaWstawienie(dziecko);
                        break;
                }
            }

            nastepnaPopulacja.push_back(dziecko); // Dodaj dziecko do nowej populacji
        }

        swap(populacja, nastepnaPopulacja); // Zamiana starych osobników na nowych - jest to bardziej efektywne niż kopiowanie elementów jeden po drugim przez populacja = nastepnaPopulacja;
        // Sortujemy nową populację tylko RAZ na koniec iteracji
        sort(populacja.begin(), populacja.end(), porownajKoszty);
        if (populacja.front().koszt < najlepszeRozwiazanie.koszt) {
            najlepszeRozwiazanie = populacja.front();
            czasNajlepszegoRozwiazania = (clock() - start) / (double)CLOCKS_PER_SEC; // Aktualizacja czasu znalezienia najlepszego rozwiązania
        }

        czas = (clock() - start) / (double)CLOCKS_PER_SEC; // Aktualizacja całkowitego czasu działania algorytmu

    } while (czas < czasTrwania); // Kontynuuj, dopóki czas działania nie przekroczy zadanego limitu

    if (!trybCichy) {
        cout << "------------------------------------------" << endl;
        cout << "Algorytm zakonczony." << endl;
        cout << "Najlepszy znaleziony koszt: " << najlepszeRozwiazanie.koszt
             << " w czasie: " << czasNajlepszegoRozwiazania << "s" << endl;
        cout << "------------------------------------------" << endl;
    }
}