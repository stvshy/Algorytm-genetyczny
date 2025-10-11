#include <cmath>
#include "Graf.h"

using namespace std;

Graf::Graf() {
    liczbaMiast = 0;
    macierzKosztow.clear();
}

Graf::~Graf() {
    liczbaMiast = 0;
    macierzKosztow.clear();
}


void Graf::dodajKrawedz(int skad, int dokad, int koszt){
    // Dodawanie krawędzi (ustawianie kosztu) między miastami.
    macierzKosztow[skad][dokad] = koszt;
}
struct Punkt {
    int id;
    double x, y;
};
void Graf::wczytajPlik() {
    macierzKosztow.clear();
    liczbaMiast = 0;
    string nazwaPliku;
    ifstream plikWejsciowy;

    cout << "Podaj plik z danymi wejsciowymi (z rozsz.): ";
    cin >> nazwaPliku;
    // Lepszy, przenośny sposób budowania ścieżki
    filesystem::path sciezkaDoPliku = filesystem::current_path().parent_path() / nazwaPliku;
    plikWejsciowy.open(sciezkaDoPliku, fstream::in);

    if (!plikWejsciowy.is_open()) {
        cout << endl << "Wczytanie pliku wejsciowego nie powiodlo sie!" << endl;
        return;
    }

    string line;
    string formatDanych = ""; // Zmienna do przechowywania formatu

    // --- ETAP 1: Czytanie nagłówka i identyfikacja formatu ---
    while (getline(plikWejsciowy, line)) {
        if (line.find("DIMENSION") != string::npos) {
            // Wytnij liczbę z linii "DIMENSION: X"
            istringstream iss(line);
            string temp;
            iss >> temp >> liczbaMiast;
        } else if (line.find("EDGE_WEIGHT_TYPE") != string::npos) {
            if (line.find("EUC_2D") != string::npos) {
                formatDanych = "EUC_2D";
            } else if (line.find("EXPLICIT") != string::npos) {
                formatDanych = "EXPLICIT";
            }
        } else if (line.find("NODE_COORD_SECTION") != string::npos) {
            // Znaleziono początek danych dla EUC_2D, można przerwać czytanie nagłówka
            break;
        } else if (line.find("EDGE_WEIGHT_SECTION") != string::npos) {
            // Znaleziono początek danych dla macierzy, można przerwać
            break;
        }
    }

    if (liczbaMiast == 0) {
        cout << "Nie udalo sie odczytac wymiaru problemu (DIMENSION)!" << endl;
        plikWejsciowy.close();
        return;
    }

    macierzKosztow.resize(liczbaMiast, vector<int>(liczbaMiast, 0));

    // --- ETAP 2: Wybór i uruchomienie odpowiedniej logiki ---
    if (formatDanych == "EUC_2D") {
        cout << "Wykryto format EUC_2D (wspolrzedne). Przetwarzanie..." << endl;

        vector<Punkt> punkty(liczbaMiast);
        int id;
        double x, y;

        // Wczytaj wszystkie współrzędne
        for (int i = 0; i < liczbaMiast; ++i) {
            plikWejsciowy >> id >> x >> y;
            // Indeksy w C++ są od 0, a w pliku od 1
            punkty[id - 1] = {id, x, y};
        }

        // Oblicz macierz kosztów na podstawie współrzędnych
        for (int i = 0; i < liczbaMiast; ++i) {
            for (int j = 0; j < liczbaMiast; ++j) {
                if (i == j) continue;
                double dx = punkty[i].x - punkty[j].x;
                double dy = punkty[i].y - punkty[j].y;
                double odleglosc = sqrt(dx*dx + dy*dy);
                // Standard TSPLIB wymaga zaokrąglenia do najbliższej liczby całkowitej
                macierzKosztow[i][j] = static_cast<int>(odleglosc + 0.5);
            }
        }

    } else if (formatDanych == "EXPLICIT") {
        cout << "Wykryto format EXPLICIT (pelna macierz). Przetwarzanie..." << endl;

        // Tutaj wklejasz swój stary kod do wczytywania macierzy
        for (int i = 0; i < liczbaMiast; ++i) {
            for (int j = 0; j < liczbaMiast; ++j) {
                plikWejsciowy >> macierzKosztow[i][j];
                // W plikach ATSP często jest duża liczba zamiast 0 na diagonali
                if (i == j) macierzKosztow[i][j] = 0;
            }
        }
    } else {
        cout << "Nieznany lub nieobslugiwany format danych!" << endl;
    }

    plikWejsciowy.close();
    cout << endl << "Pomyslnie wczytano i przetworzono plik." << endl;
}

void Graf::generujLosowo() {
    macierzKosztow.clear();
    cout << "Podaj liczbe miast: ";
    cin >> liczbaMiast;
    macierzKosztow.resize(liczbaMiast, vector<int>(liczbaMiast, -1)); // Inicjalizacja macierzy kosztów
    for (int i = 0; i < liczbaMiast; i++) {
        for (int j = 0; j < liczbaMiast; j++) {
            if (i != j) {
                macierzKosztow[i][j] = rand() % 101;
            }
            else {
                macierzKosztow[i][j] = -1;
            }
        }
    }
}

int Graf::obliczKoszt(int skad, int dokad) {
    return macierzKosztow[skad][dokad];
}

void Graf::wypiszGraf() {
    // Wyświetlenie grafu (macierzy kosztów).
    for (int i = 0; i < liczbaMiast; ++i) {
        for (int j = 0; j < liczbaMiast; ++j) {
            cout << macierzKosztow[i][j] << " ";
        }
        cout << endl;
    }
}



