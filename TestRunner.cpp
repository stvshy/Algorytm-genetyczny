#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>

#include "Graf.h"
#include "Genetyczny.h"

// --- Funkcje pomocnicze do konwersji enum na string (bez zmian) ---
string selekcjaToString(MetodaSelekcji metoda) {
    switch (metoda) {
        case MetodaSelekcji::TURNIEJOWA: return "Turniejowa";
        case MetodaSelekcji::RULETKOWA: return "Ruletkowa";
        case MetodaSelekcji::RANKINGOWA: return "Rankingowa";
        default: return "Nieznana";
    }
}
string krzyzowanieToString(MetodaKrzyzowania metoda) {
    switch (metoda) {
        case MetodaKrzyzowania::OX: return "OX";
        case MetodaKrzyzowania::PBX: return "PBX";
        default: return "Nieznana";
    }
}
string mutacjaToString(MetodaMutacji metoda) {
    switch (metoda) {
        case MetodaMutacji::ZAMIANA: return "Zamiana";
        case MetodaMutacji::INWERSJA: return "Inwersja";
        case MetodaMutacji::WSTAWIANIE: return "Wstawianie";
        default: return "Nieznana";
    }
}

// Funkcja pomocnicza, aby uniknąć powtarzania kodu
void uruchomTest(const string& nazwaPliku, Graf& graf, ofstream& plikWynikowy,
                 int kryteriumStopu, int wielkoscPopulacji, double wspolczynnikMutacji, double wspolczynnikKrzyzowania,
                 MetodaSelekcji selekcja, MetodaKrzyzowania krzyzowanie, MetodaMutacji mutacja, const string& faza) {

    cout << "Faza [" << faza << "]: Pop:" << wielkoscPopulacji << " Mut:" << wspolczynnikMutacji << " Krz:" << wspolczynnikKrzyzowania
         << " | " << selekcjaToString(selekcja) << " + " << krzyzowanieToString(krzyzowanie) << " + " << mutacjaToString(mutacja)
         << " | Czas: " << kryteriumStopu << "s ... " << flush;

    Genetyczny genetyczny(graf, kryteriumStopu, wielkoscPopulacji, wspolczynnikMutacji, wspolczynnikKrzyzowania,
                          selekcja, krzyzowanie, mutacja);
    genetyczny.algorytm(true);

    plikWynikowy << nazwaPliku << ";" << faza << ";" << kryteriumStopu << ";" << wielkoscPopulacji << ";"
                 << wspolczynnikMutacji << ";" << wspolczynnikKrzyzowania << ";"
                 << selekcjaToString(selekcja) << ";" << krzyzowanieToString(krzyzowanie) << ";"
                 << mutacjaToString(mutacja) << ";" << genetyczny.najlepszeRozwiazanie.koszt << ";"
                 << genetyczny.czasNajlepszegoRozwiazania << "\n";

    cout << "Koszt: " << genetyczny.najlepszeRozwiazanie.koszt << endl;
}


int main() {
    string nazwaPliku = "a280.tsp";
    Graf graf;
    graf.wczytajPlik(nazwaPliku);
    if (graf.macierzKosztow.empty()) {
        cerr << "Nie udalo sie wczytac grafu: " << nazwaPliku << endl;
        return 1;
    }

    ofstream plikWynikowy("wyniki_finalne.csv");
    plikWynikowy << "Plik;FazaTestu;CzasStopu;Populacja;Mutacja;Krzyzowanie;Selekcja;MetodaKrzyzowania;MetodaMutacji;NajlepszyKoszt;CzasZnalezienia\n";

    cout << "Rozpoczynam kompleksowe testy (~60 minut)..." << endl;

    // =================================================================================================
    // FAZA 1: PORÓWNANIE OPERATORÓW (36 konfiguracji * 60s = ~36 minut)
    // Cel: Znaleźć najlepszą kombinację metod selekcji, krzyżowania i mutacji.
    // Testujemy na średniej i dużej populacji, aby zobaczyć, jak metody skalują się z populacją.
    // =================================================================================================
    cout << "\n--- FAZA 1: Porownanie operatorow genetycznych ---\n" << endl;
    vector<int> populacje_faza1 = {150, 300};
    double mutacja_faza1 = 0.05;
    double krzyzowanie_faza1 = 0.8;
    vector<MetodaSelekcji> metodySelekcji = {MetodaSelekcji::TURNIEJOWA, MetodaSelekcji::RULETKOWA, MetodaSelekcji::RANKINGOWA};
    vector<MetodaKrzyzowania> metodyKrzyzowania = {MetodaKrzyzowania::OX, MetodaKrzyzowania::PBX};
    vector<MetodaMutacji> metodyMutacji = {MetodaMutacji::ZAMIANA, MetodaMutacji::INWERSJA, MetodaMutacji::WSTAWIANIE};

    for (auto pop : populacje_faza1) {
        for (auto sel : metodySelekcji) {
            for (auto krz_met : metodyKrzyzowania) {
                for (auto mut_met : metodyMutacji) {
                    uruchomTest(nazwaPliku, graf, plikWynikowy, 60, pop, mutacja_faza1, krzyzowanie_faza1, sel, krz_met, mut_met, "1_Operatory");
                }
            }
        }
    }

    // =================================================================================================
    // FAZA 2: DOSTRAJANIE PARAMETRÓW (9 konfiguracji * 60s = ~9 minut)
    // Cel: Dla najlepszej kombinacji metod, sprawdzić jak działają różne współczynniki mutacji i krzyżowania.
    // UWAGA: Wybieramy "kandydata na najlepszą metodę" - ogólnie Turniejowa+OX+Inwersja jest bardzo silna.
    // Jeśli Twoje wyniki z Fazy 1 wskażą coś innego, możesz tu podstawić inną kombinację.
    // =================================================================================================
    cout << "\n--- FAZA 2: Dostrajanie wspolczynnikow dla najlepszych metod ---\n" << endl;
    int populacja_faza2 = 300; // Używamy dużej populacji, bo daje lepsze wyniki
    MetodaSelekcji najlepsza_selekcja = MetodaSelekcji::TURNIEJOWA;
    MetodaKrzyzowania najlepsze_krzyzowanie = MetodaKrzyzowania::OX;
    MetodaMutacji najlepsza_mutacja = MetodaMutacji::INWERSJA;
    vector<double> wsp_mutacji_faza2 = {0.01, 0.05, 0.1};
    vector<double> wsp_krzyzowania_faza2 = {0.7, 0.8, 0.9};

    for (auto mut : wsp_mutacji_faza2) {
        for (auto krz : wsp_krzyzowania_faza2) {
            uruchomTest(nazwaPliku, graf, plikWynikowy, 60, populacja_faza2, mut, krz, najlepsza_selekcja, najlepsze_krzyzowanie, najlepsza_mutacja, "2_Dostrajanie");
        }
    }

    // =================================================================================================
    // FAZA 3: DŁUGIE TESTY DLA NAJLEPSZYCH (4 konfiguracje, łącznie ~16 minut)
    // Cel: Sprawdzić pełen potencjał najlepszych konfiguracji, dając im znacznie więcej czasu.
    // Testujemy 2 najlepsze wielkości populacji z najlepszymi metodami i parametrami z Fazy 2.
    // =================================================================================================
    cout << "\n--- FAZA 3: Dlugie testy dla najlepszych konfiguracji ---\n" << endl;
    vector<int> populacje_faza3 = {300, 500};
    vector<int> czasy_faza3 = {180, 300}; // 3 minuty i 5 minut
    double mutacja_faza3 = 0.05; // Zakładamy, że to dobry kompromis
    double krzyzowanie_faza3 = 0.8;

    for (auto pop : populacje_faza3) {
        for (auto czas : czasy_faza3) {
            uruchomTest(nazwaPliku, graf, plikWynikowy, czas, pop, mutacja_faza3, krzyzowanie_faza3, najlepsza_selekcja, najlepsze_krzyzowanie, najlepsza_mutacja, "3_DlugieTesty");
        }
    }

    plikWynikowy.close();
    cout << "\n----------------------------------------------------" << endl;
    cout << "Wszystkie testy zakonczone. Wyniki zapisano do pliku 'wyniki_finalne.csv'." << endl;

    return 0;
}