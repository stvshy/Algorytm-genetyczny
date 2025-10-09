#include <vector>
#include "Menu.h"
#include "Menu.h"
#include "Genetyczny.h"

using namespace std;


int Menu::mainMenu() {

    // Ustawienie polskiej lokalizacji w celu poprawnego wyświetlania znaków
    setlocale(LC_CTYPE, "Polish");

    // Wyświetlenie informacji o autorze i projekcie
    cout << endl << "PEA, Projekt 3, Mateusz Staszkow, 263949" << endl;

    char wybor = 0;
    string fileName = "";


    int wielkoscPopulacji = 50;
    int kryteriumStopu = 15;
    double wspolczynnikMutacji = 0.01;
    double wspolczynnikKrzyzowania = 0.8;
    int metodaKrzyzowania = 1;
    string nazwa;

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
             << "6. Wybor metody krzyzowania" << endl
             << "7. Wybor metody mutacji" << endl
             << "8. Rozwiaz algorytmem genetycznym" << endl
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
                cout << "Podaj metode krzyzowania " << endl
                << "1. OX" << endl
                << "2. PBX" << endl << endl;
                cout << "Wpisz swoj wybor: ";
                cin >> metodaKrzyzowania;
                cout << endl;
                break;
            case '7':
                cout << "Podaj metode mutacji: ";

                cout << endl;
                break;
            case '8':
                if (!graf.macierzKosztow.empty()) {
                    Genetyczny
                    genetyczny(graf, kryteriumStopu, wielkoscPopulacji, wspolczynnikMutacji, wspolczynnikKrzyzowania, metodaKrzyzowania);
                    genetyczny.algorytm();
                    cout << "Koszt: " << genetyczny.najlepszeRozwiazanie.koszt << endl;
                    cout << "Czas najl. rozw.: " <<genetyczny.czasNajlepszegoRozwiazania << endl;
                    genetyczny.wypiszTrase(genetyczny.najlepszeRozwiazanie.chromosom);
                }
                break;
            case '9':
                if (!graf.macierzKosztow.empty()) {

                }
                break;
                // W przypadku wybrania opcji wyjścia zamknij, jeżeli wybrano 0
            case '0':
                cout << "Zakonczenie programu" << endl;
                return 0;
        }

    }
}
