#include <cmath>
#include "Graf.h"

using namespace std;

Graf::Graf() {
    liczbaMiast = 0;
    macierzKosztow.clear();
    nazwaZaladowanegoPliku = "BRAK";
}

Graf::~Graf() {
    liczbaMiast = 0;
    macierzKosztow.clear();
}

void Graf::wczytajPlik(const string& nazwaPliku) {
    // Najpierw czyścimy stare dane
    macierzKosztow.clear();
    liczbaMiast = 0;
    nazwaZaladowanegoPliku = "Błąd wczytywania..."; // Tymczasowa wartość na wypadek błędu

    ifstream plikWejsciowy;  // ifstream (Input File Stream) - obiekt służący do odczytywania danych z plików

    filesystem::path sciezkaDoPlikuZrodlowego = __FILE__; // Ścieżka do bieżącego pliku (Graf.cpp)
    filesystem::path sciezkaDoFolderuGlownego = sciezkaDoPlikuZrodlowego.parent_path(); // Ścieżka do folderu nadrzędnego dla Graf.cpp (czyli folder projektu)
    filesystem::path pelnaSciezkaDoPliku = sciezkaDoFolderuGlownego / nazwaPliku; // Pełna ścieżka do pliku z danymi (TIO 1/[np.: a280.tsp])

    plikWejsciowy.open(pelnaSciezkaDoPliku, fstream::in); // Otwieramy plik do odczytu (fstream::in - tryb read-only)

    if (!plikWejsciowy.is_open()) { // Jeśli plik nie został poprawnie otwarty
        cout << endl << "Wczytanie pliku wejsciowego '" << pelnaSciezkaDoPliku.string() << "' nie powiodlo sie!" << endl;
        cout << "Upewnij sie, ze plik '" << nazwaPliku << "' znajduje sie w glownym folderze projektu." << endl;

        // resetujemy do stanu "BRAK"
        return;
    }
    string line;
    string formatDanych = "";

    while (getline(plikWejsciowy, line)) { // Czytamy plik linia po linii (line zawiera aktualnie czytaną linię)
        if (line.find("DIMENSION") != string::npos) { // Jeśli linia zawiera "DIMENSION" (string::npos oznacza, że find nie znalazł podłańcucha - i właśnie to zwraca)
            istringstream iss(line); // Stwórz strumień z tej linii - możemy z niego czytać jak z cin (słowo po słowie)
            string temp; // Zmienna śmietnik - wrzucimy tu pierwsze słowo ("DIMENSION" lub "DIMENSION:"), bo nas nie interesuje
            iss >> temp >> liczbaMiast; // Pierwsze słowo do temp, drugie (liczba) do liczbaMiast
        } else if (line.find("EDGE_WEIGHT_TYPE") != string::npos) { //czy w tej linii jest słowo "EDGE_WEIGHT_TYPE" (określa format danych w pliku)
            if (line.find("EUC_2D") != string::npos) { // Jeśli w tej samej linii jest "EUC_2D", to dane będą jako współrzędne punktów (x,y)
                formatDanych = "EUC_2D";
            } else if (line.find("EXPLICIT") != string::npos) { // Jeśli w tej samej linii jest "EXPLICIT", to dane będą jako pełna macierz kosztów
                formatDanych = "EXPLICIT";
            }
        } else if (line.find("NODE_COORD_SECTION") != string::npos) {  // Sekcja z danymi współrzędnych punktów (jeśli EUC_2D)
            break; // Przerywamy pętlę - zaraz zaczniemy czytać dane
        } else if (line.find("EDGE_WEIGHT_SECTION") != string::npos) {
            break;
        }
    }

    if (liczbaMiast == 0) {
        cout << "Nie udalo sie odczytac wymiaru problemu (DIMENSION)!" << endl;
        plikWejsciowy.close();
        return;
    }

    macierzKosztow.resize(liczbaMiast, vector<int>(liczbaMiast, 0)); // Inicjalizacja macierzy kosztów - NxN wypełniona zerami

    if (formatDanych == "EUC_2D") {
        struct Punkt { int id; double x, y; }; // Struktura do przechowywania punktów (miast) z ich współrzędnymi
        vector<Punkt> punkty(liczbaMiast); // Wektor punktów o rozmiarze liczbaMiast
        int id;
        double x, y;
        for (int i = 0; i < liczbaMiast; ++i) {
            plikWejsciowy >> id >> x >> y; // Wczytujemy id, x, y
            punkty[id - 1] = {id, x, y}; // Przechowujemy w wektorze (id-1, bo id zaczyna się od 1, a indeksy od 0)
        }
        for (int i = 0; i < liczbaMiast; ++i) { // Obliczamy odległości między punktami i wypełniamy macierz kosztów
            for (int j = 0; j < liczbaMiast; ++j) {
                if (i == j) continue; // Koszt z miasta do samego siebie to 0 (już jest 0 w macierzy)
                double dx = punkty[i].x - punkty[j].x; // Różnica współrzędnych x
                double dy = punkty[i].y - punkty[j].y; // Różnica współrzędnych y
                double odleglosc = sqrt(dx*dx + dy*dy) ; // Obliczamy odległość miast tworząc trójkąt prostokątny (twierdzenie Pitagorasa)
                macierzKosztow[i][j] = static_cast<int>(odleglosc + 0.5); // Zaokrąglamy do najbliższej liczby całkowitej (static_cast<int> obcina część ułamkową)
                // Zaokąglamy bo tak wymaga format EUC_2D w TSPLIB dla spójności z benchmarkami
            }
        }
    } else if (formatDanych == "EXPLICIT") { // Pełna macierz kosztów
        cout << "Wykryto format EXPLICIT (pelna macierz). Przetwarzanie..." << endl;
        for (int i = 0; i < liczbaMiast; ++i) { // Wczytujemy macierz kosztów
            for (int j = 0; j < liczbaMiast; ++j) {
                plikWejsciowy >> macierzKosztow[i][j]; // Wczytujemy koszt z i do j
                if (i == j) macierzKosztow[i][j] = 0;
            }
        }
    } else {
        cout << "Nieznany lub nieobslugiwany format danych!" << endl;
    }

    plikWejsciowy.close();

    // Jeśli wszystko się udało, zapisujemy nazwę pliku do pokazania aktualnie załadowanego pliku
    nazwaZaladowanegoPliku = nazwaPliku;

    cout << endl << "Pomyslnie wczytano i przetworzono plik" << nazwaPliku << endl;
}
// WERSJA 2: Interaktywna, dla użytkownika z menu (opcja '1')
void Graf::wczytajPlik() {
    string nazwaPliku;
    cout << "Wprowadz nazwe pliku z danymi: ";

    // Czyszczenie bufora po `cin >> wybor` z menu
    if (cin.peek() == '\n') {
        cin.ignore();
    }
    getline(cin, nazwaPliku);

    if (nazwaPliku.empty()) {
        cout << "Anulowano wczytywanie nowego pliku." << endl;
        return;
    }

    // Wywołaj wersję z argumentem, która wykonuje całą pracę
    wczytajPlik(nazwaPliku);
}



