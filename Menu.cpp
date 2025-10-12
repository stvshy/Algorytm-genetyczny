#include "Menu.h"
#include "Genetyczny.h"

using namespace std;

Menu::Menu() {
    wielkoscPopulacji = 50;  // wartości domyślne
    kryteriumStopu = 30;
    wspolczynnikMutacji = 0.01;
    wspolczynnikKrzyzowania = 0.8;
    graf.wczytajPlik("a280.tsp");
}

void Menu::wyswietlAktualneParametry() {
    cout << endl;
    cout << " ------------------------------------------ " << endl;
    cout << "         AKTUALNE PARAMETRY " << endl;
    cout << " ------------------------------------------ " << endl;
    cout << " 1. Zaladowany graf        : " << graf.nazwaZaladowanegoPliku << endl;
    cout << " 2. Kryterium stopu        : " << kryteriumStopu << "s" << endl;
    cout << " 3. Wielkosc populacji     : " << wielkoscPopulacji << endl;
    cout << " 4. Wspolczynnik mutacji   : " << wspolczynnikMutacji << endl;
    cout << " 5. Wspolczynnik krzyzowania : " << wspolczynnikKrzyzowania << endl;
    cout << " ------------------------------------------ " << endl;
}

int Menu::mainMenu() {

    // Ustawienie polskiej lokalizacji w celu poprawnego wyświetlania polskich znaków w konsoli
    setlocale(LC_CTYPE, "Polish");

    // Wyświetlenie informacji o autorze i projekcie
    cout << endl << "TIO, Projekt 1, Mateusz Staszkow, 263949" << endl;

    char wybor = 0;

    while (true) {
        // Wyświetlenie menu
        cout << endl;
        cout << "             Menu  " << endl;
        cout << " ----------------------------- " << endl;
        cout << "Wybierz opcje: " << endl
             << "1. Wczytaj dane z pliku" << endl
             << "2. Wprowadz kryterium stopu" << endl
             << "3. Ustaw wielkosc populacji poczatkowej" << endl
             << "4. Ustaw wspolczynnik mutacji" << endl
             << "5. Ustaw wspolczynnik krzyzowania" << endl
             << "6. Wyswietl aktualne parametry" << endl
             << "7. Rozwiaz algorytmem genetycznym" << endl
             << "0. Wyjscie" << endl << endl;
        cout << "Wpisz swoj wybor: ";
        // Odczytanie wyboru użytkownika
        cin >> wybor;
        cout << endl;
        // Wykonanie akcji zgodnie z wyborem użytkownika
        switch (wybor) {
            // W przypadku niepoprawnego wyboru
            default:
                cout << "Wybrana opcja jest niepoprawna!" << endl;
                break;
            case '1':
                graf.wczytajPlik();
                break;
            case '2':
                cout << "Wprowadz kryterium stopu: ";
                cin >> kryteriumStopu;
                cout << endl;
                break;
            case '3':
                cout << "Wprowadz wielkosc populacji poczatkowej: ";
                cin >> wielkoscPopulacji;
                cout << endl;
                break;
            case '4':
                cout << "Wprowadz wspolczynnik mutacji: ";
                cin >> wspolczynnikMutacji;
                cout << endl;
                break;
            case '5':
                cout << "Wprowadz wspolczynnik krzyzowania: ";
                cin >> wspolczynnikKrzyzowania;
                cout << endl;
                break;
            case '6':
                wyswietlAktualneParametry();
                break;
            case '7':
                if (!graf.macierzKosztow.empty()) { // Sprawdzenie, czy graf jest załadowany
                    Genetyczny genetyczny(graf, kryteriumStopu, wielkoscPopulacji, wspolczynnikMutacji, wspolczynnikKrzyzowania);
                    genetyczny.algorytm();
                    cout << "Koszt: " << genetyczny.najlepszeRozwiazanie.koszt << endl;
                    cout << "Czas najl. rozw.: " <<genetyczny.czasNajlepszegoRozwiazania << endl;
                    genetyczny.wypiszTrase(genetyczny.najlepszeRozwiazanie.chromosom);
                } else {
                    cout << "Graf nie jest zaladowany! Wczytaj dane z pliku." << endl;
                }
                break;
            case '0':
                cout << "Zakonczenie programu" << endl;
                return 0;
        }

    }
}
