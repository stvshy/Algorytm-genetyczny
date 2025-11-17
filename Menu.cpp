#include "Menu.h"
#include "Genetyczny.h"

using namespace std;

Menu::Menu() {
    wielkoscPopulacji = 50;  // wartości domyślne
    kryteriumStopu = 60;
    wspolczynnikMutacji = 0.01;
    wspolczynnikKrzyzowania = 0.8;
    graf.wczytajPlik("a280.tsp");

    wybranaMetodaSelekcji = MetodaSelekcji::TURNIEJOWA;
    wybranaMetodaKrzyzowania = MetodaKrzyzowania::OX;
    wybranaMetodaMutacji = MetodaMutacji::ZAMIANA;
}
string Menu::selekcjaToString(MetodaSelekcji metoda) {
    switch (metoda) {
        case MetodaSelekcji::TURNIEJOWA: return "Turniejowa";
        case MetodaSelekcji::RULETKOWA: return "Ruletkowa";
        case MetodaSelekcji::RANKINGOWA: return "Rankingowa";
        default: return "Nieznana";
    }
}

string Menu::krzyzowanieToString(MetodaKrzyzowania metoda) {
    switch (metoda) {
        case MetodaKrzyzowania::OX: return "Order Crossover (OX)";
        case MetodaKrzyzowania::PBX: return "Position-Based Crossover (PBX)";
        default: return "Nieznana";
    }
}

string Menu::mutacjaToString(MetodaMutacji metoda) {
    switch (metoda) {
        case MetodaMutacji::ZAMIANA: return "Przez zamiane (Swap)";
        case MetodaMutacji::INWERSJA: return "Przez inwersje (Inversion)";
        case MetodaMutacji::WSTAWIANIE: return "Przez wstawienie (Insert)";
        default: return "Nieznana";
    }
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
    cout << " 6. Metoda selekcji          : " << selekcjaToString(wybranaMetodaSelekcji) << endl;
    cout << " 7. Metoda krzyzowania       : " << krzyzowanieToString(wybranaMetodaKrzyzowania) << endl;
    cout << " 8. Metoda mutacji           : " << mutacjaToString(wybranaMetodaMutacji) << endl;
    cout << " ------------------------------------------ " << endl;
}

int Menu::mainMenu() {

    // Ustawienie polskiej lokalizacji w celu poprawnego wyświetlania polskich znaków w konsoli
    setlocale(LC_CTYPE, "Polish");

    // Wyświetlenie informacji o autorze i projekcie
    cout << endl << "TIO, Projekt 2, Mateusz Staszkow, 263949" << endl;

    char wybor = 0;
    int temp_int;
    double temp_double;

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
             << "6. Wybierz metode selekcji" << endl
             << "7. Wybierz metode krzyzowania" << endl
             << "8. Wybierz metode mutacji" << endl
             << "9. Wyswietl aktualne parametry" << endl
             << "s. Start algorytmu genetycznego" << endl
             << "0. Wyjscie" << endl << endl;
        cout << "Wpisz swoj wybor: ";
        // Odczytanie wyboru użytkownika
        cin >> wybor;
        cout << endl;
        // Wykonanie akcji zgodnie z wyborem użytkownika
        if (cin.fail()) { // Sprawdzenie, czy wystąpił błąd podczas wczytywania
            cout << "Wprowadzono niepoprawny znak. Sprobuj ponownie." << endl;
            cin.clear();// Resetowanie flagi błędu i "odblokowanie" strumienia cin
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Czyszczenie bufora wejściowego z błędnych danych
            continue; // Pomiń resztę pętli i wyświetl menu od nowa
        }

        switch (wybor) {
            default:
                cout << "Wybrana opcja jest niepoprawna!" << endl;
                // Czyszczenie bufora na wypadek wpisania opcji nie pasującej do poniższych
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            case '1':
                graf.wczytajPlik();
                break;
            case '2':
                cout << "Wprowadz kryterium stopu (w sekundach): ";
                cin >> temp_int;
                if (cin.fail() || temp_int <= 0) {
                    cout << "Blad: Wprowadzono niepoprawne dane. Wymagana dodatnia liczba calkowita." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                } else {
                    kryteriumStopu = temp_int;
                    cout << "Ustawiono kryterium stopu na: " << kryteriumStopu << "s" << endl;
                }
                break;
            case '3':
                cout << "Wprowadz wielkosc populacji: ";
                cin >> temp_int;
                if (cin.fail() || temp_int <= 1) {
                    cout << "Blad: Wprowadzono niepoprawne dane. Wymagana liczba calkowita wieksza od 1." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                } else {
                    wielkoscPopulacji = temp_int;
                    cout << "Ustawiono wielkosc populacji na: " << wielkoscPopulacji << endl;
                }
                break;
            case '4':
                cout << "Wprowadz wspolczynnik mutacji z przedzialu [0.0, 1.0]: ";
                cin >> temp_double;
                if (cin.fail() || temp_double < 0.0 || temp_double > 1.0) {
                    cout << "Blad: Wprowadzono niepoprawne dane. Wymagana liczba z przedzialu [0.0, 1.0]." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                } else {
                    wspolczynnikMutacji = temp_double;
                    cout << "Ustawiono wspolczynnik mutacji na: " << wspolczynnikMutacji << endl;
                }
                break;
            case '5':
                cout << "Wprowadz wspolczynnik krzyzowania z przedzialu [0.0, 1.0]: ";
                cin >> temp_double;
                if (cin.fail() || temp_double < 0.0 || temp_double > 1.0) {
                    cout << "Blad: Wprowadzono niepoprawne dane. Wymagana liczba z przedzialu [0.0, 1.0]." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                } else {
                    wspolczynnikKrzyzowania = temp_double;
                    cout << "Ustawiono wspolczynnik krzyzowania na: " << wspolczynnikKrzyzowania << endl;
                }
                break;
            case '6':
                cout << "Wybierz metode selekcji:" << endl << "1. Turniejowa" << endl << "2. Ruletkowa" << endl << "3. Rankingowa" << endl << "Wybor: ";
                cin >> temp_int;
                if (cin.fail()) {
                    cout << "Blad: Wprowadzono niepoprawne dane. Prosze wprowadzic liczbe." << endl;
                    cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); break;
                }
                if (temp_int == 1) { wybranaMetodaSelekcji = MetodaSelekcji::TURNIEJOWA; cout << "Wybrano selekcje turniejowa." << endl; }
                else if (temp_int == 2) { wybranaMetodaSelekcji = MetodaSelekcji::RULETKOWA; cout << "Wybrano selekcje ruletkowa." << endl; }
                else if (temp_int == 3) { wybranaMetodaSelekcji = MetodaSelekcji::RANKINGOWA; cout << "Wybrano selekcje rankingowa." << endl; }
                else { cout << "Niepoprawny numer opcji!" << endl; }
                break;
            case '7':
                cout << "Wybierz metode krzyzowania:" << endl << "1. Order Crossover (OX)" << endl << "2. Position-Based Crossover (PBX)" << endl << "Wybor: ";
                cin >> temp_int;
                if (cin.fail()) {
                    cout << "Blad: Wprowadzono niepoprawne dane. Prosze wprowadzic liczbe." << endl;
                    cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); break;
                }
                if (temp_int == 1) { wybranaMetodaKrzyzowania = MetodaKrzyzowania::OX; cout << "Wybrano krzyzowanie OX." << endl; }
                else if (temp_int == 2) { wybranaMetodaKrzyzowania = MetodaKrzyzowania::PBX; cout << "Wybrano krzyzowanie PBX." << endl; }
                else { cout << "Niepoprawny numer opcji!" << endl; }
                break;
            case '8':
                cout << "Wybierz metode mutacji:" << endl << "1. Przez zamiane (Swap)" << endl << "2. Przez inwersje (Inversion)" << endl << "3. Przez wstawienie (Insert)" << endl << "Wybor: ";
                cin >> temp_int;
                if (cin.fail()) {
                    cout << "Blad: Wprowadzono niepoprawne dane. Prosze wprowadzic liczbe." << endl;
                    cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); break;
                }
                if (temp_int == 1) { wybranaMetodaMutacji = MetodaMutacji::ZAMIANA; cout << "Wybrano mutacje przez zamiane." << endl; }
                else if (temp_int == 2) { wybranaMetodaMutacji = MetodaMutacji::INWERSJA; cout << "Wybrano mutacje przez inwersje." << endl; }
                else if (temp_int == 3) { wybranaMetodaMutacji = MetodaMutacji::WSTAWIANIE; cout << "Wybrano mutacje przez wstawienie." << endl; }
                else { cout << "Niepoprawny numer opcji!" << endl; }
                break;
            case '9':
                wyswietlAktualneParametry();
                break;
            case 's':
                if (!graf.macierzKosztow.empty()) {
                    Genetyczny genetyczny(graf, kryteriumStopu, wielkoscPopulacji, wspolczynnikMutacji, wspolczynnikKrzyzowania,
                                          wybranaMetodaSelekcji, wybranaMetodaKrzyzowania, wybranaMetodaMutacji);
                    genetyczny.algorytm();
                    cout << "Znaleziona trasa:" << endl;
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