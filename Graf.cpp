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

void Graf::wczytajPlik() {
    macierzKosztow.clear();
    string nazwaPliku;
    ifstream plikWejsciowy;
    cout << "Podaj plik z danymi wejsciowymi (z rozsz.): ";
    cin >> nazwaPliku;
    string katalogBiezacy = filesystem::current_path().parent_path().string();
    plikWejsciowy.open(katalogBiezacy + "\\" + nazwaPliku, fstream::in);

    if (!plikWejsciowy.is_open()) {
        cout << endl << "Wczytanie pliku wejsciowego nie powiodlo sie!" << endl;
        return;
    } else
    {
        cout << endl << "Pomyslnie wczytano plik." << endl;
    }

    string line;
    int dimension = 0;
    int lineNumber = 0;
    int currentRow = 0;
    int currentCol = 0;

    while (getline(plikWejsciowy, line)) {
        lineNumber++;
        if (lineNumber == 4) {
            size_t found = line.find("DIMENSION:");
            if (found != string::npos) {
                string dimensionStr = line.substr(found + 11);
                dimension = stoi(dimensionStr);
                liczbaMiast = dimension;
                macierzKosztow.resize(dimension, vector<int>(dimension, 0));
            }
        }
        if (lineNumber > 7) {
            istringstream iss(line);
            int cost;
            while (iss >> cost && currentRow < dimension) {
                macierzKosztow[currentRow][currentCol] = cost;
                currentCol++;
                if (currentCol == dimension) {
                    currentRow++;
                    currentCol = 0;
                }
            }
        }
    }

    plikWejsciowy.close();
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



